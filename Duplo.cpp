/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Duplo.h"

#include <fstream>
#include <time.h>

#include <algorithm>
#include <cassert>

#include "SourceFile.h"

#include "StringUtil.h"
#include "TextFile.h"
#include "ArgumentParser.h"
#include "TextGenerator.h"
#include "XMLGenerator.h"

using std::cout;
using std::endl;

Duplo::Duplo(
    const std::string& listFileName, 
    unsigned int minBlockSize, 
    unsigned int blockPercentThreshold,
    unsigned int minChars, 
    bool ignorePrepStuff, bool ignoreSameFilename, bool Xml) :
    m_listFileName(listFileName),
    m_minBlockSize(minBlockSize),
    m_blockPercentThreshold(blockPercentThreshold),
    m_minChars(minChars),
    m_ignorePrepStuff(ignorePrepStuff),
    m_ignoreSameFilename(ignoreSameFilename),
    m_maxLinesPerFile(0),
    m_DuplicateLines(0),
    m_Xml(Xml),
    m_pMatrix(),
    _report_generator( )
{
}

Duplo::~Duplo(){
}

void Duplo::reportSeq(int line1, 
                      int line2, 
                      int count, 
                      const SourceFile& pSource1, 
                      const SourceFile& pSource2, 
                      std::ostream& outFile){

    _report_generator->reportSeq( line1, line2, count, pSource1, pSource2 );
    m_DuplicateLines += count;
}

int Duplo::process(const SourceFile& pSource1, const SourceFile& pSource2, std::ostream& outFile) 
{

    const unsigned int m = pSource1.getNumOfLinesOfCode();
    const unsigned int n = pSource2.getNumOfLinesOfCode();


    //const unsigned char NONE = 0;
    //const unsigned char MATCH = 1;

    long long index = m * n;
    assert( ( index ) < matrix_size );

    // Reset matrix data
    std::fill( m_pMatrix.begin( ), m_pMatrix.begin( ) + ( m * n ) , false );

    // Compute matrix
    for(unsigned int y=0; y<m; y++){
        const auto & pSLine = pSource1.getLine(y);
        for(unsigned int x=0; x<n; x++){

            if( pSLine.equals( pSource2.getLine( x ) ) ) {
                m_pMatrix.at( x+n*y ) = true;
            }
        }
    }

    // support reporting filtering by both:
    // - "lines of code duplicated", &
    // - "percentage of file duplicated"
    const unsigned int lMinBlockSize = std::max(
        1u, std::min(
            m_minBlockSize, 
            (std::max(n,m)*100)/m_blockPercentThreshold
        )
    );
    
    int blocks=0;

    // Scan vertical part
    for(unsigned int y=0; y<m; y++){

        unsigned int seqLen=0;
        int maxX = std::min(n, m-y);
        for(int x=0; x<maxX; x++){

            if(m_pMatrix.at( x+n*(y+x) ) == true ){


                seqLen++;
            } else {

                if(seqLen >= lMinBlockSize){

                    int line1 = y+x-seqLen;

                    int line2 = x-seqLen;

                    if (!((line1 == line2) && (pSource1.getFilename( ) == pSource2.getFilename( ) ) ) ) {

                        reportSeq(line1, line2, seqLen, pSource1, pSource2, outFile);
                        blocks++;
                    }
                }
                seqLen=0;
            }
        }

        if(seqLen >= lMinBlockSize){
            int line1 = m-seqLen;
            int line2 = n-seqLen;
            if (!((line1 == line2) && (pSource1.getFilename( ) == pSource2.getFilename( ) ) ) ) {
                reportSeq(line1, line2, seqLen, pSource1, pSource2, outFile);
                blocks++;
            }
        }
    }

    if (pSource1.getFilename( ) != pSource2.getFilename( ) )
    {
        // Scan horizontal part
        for(unsigned int x=1; x<n; x++){
            unsigned int seqLen=0;
            int maxY = std::min(m, n-x);
            for(int y=0; y<maxY; y++){
                if(m_pMatrix.at( x+y+n*y ) == true ){
                    seqLen++;
                } else {
                    if(seqLen >= lMinBlockSize){
                        reportSeq(y-seqLen, x+y-seqLen, seqLen, pSource1, pSource2, outFile);
                        blocks++;
                    }
                    seqLen=0;
                }
            }

            if(seqLen >= lMinBlockSize){
                reportSeq(m-seqLen, n-seqLen, seqLen, pSource1, pSource2, outFile);
                blocks++;
            }
        }
    }

    return blocks;
}

const std::string Duplo::getFilenamePart(const std::string& fullpath) const {
    std::string path = StringUtil::substitute('\\', '/', fullpath);

    std::string filename = path;

    std::string::size_type idx = path.rfind('/');
    if(idx != std::string::npos){
        filename = path.substr(idx+1, path.size()-idx-1);
    }

    return filename;
}

bool Duplo::isSameFilename(const std::string& filename1, const std::string& filename2) const {
    return (getFilenamePart(filename1) == getFilenamePart(filename2));
}

void Duplo::run(std::string outputFileName) {

    std::ofstream outfile(outputFileName.c_str(), std::ios::out|std::ios::binary);
    
    if(!outfile.is_open()) {
        std::cout << "Error: Can't open file: " << outputFileName << std::endl;
        return;
    }

    if( m_Xml )
    {
        _report_generator = std::make_unique<XMLGenerator>( outfile );
    }
    else
    {
        _report_generator = std::make_unique<TextGenerator>( outfile );   
    }

    _report_generator->writeHeader( m_minBlockSize, m_minChars, m_ignorePrepStuff, m_ignoreSameFilename, VERSION );


    clock_t start, finish;
    double  duration;

    start = clock();

    std::cout << "Loading and hashing files ... ";
    std::cout.flush();

    
    TextFile listOfFiles(m_listFileName.c_str());
    std::vector<std::string> lines;
    listOfFiles.readLines(lines, true);
    
    std::vector<SourceFile> sourceFiles;
    sourceFiles.reserve( lines.size( ) );
    
    int files = 0;
    int locsTotal = 0;


    //Set values for processing of files.
    SourceFile::setMinChars( m_minChars );
    SourceFile::setIgnorePreprocessor( m_ignorePrepStuff );

    // Create vector with all source files
    for( auto & line: lines ) {

        if(line.size() > 5){

            SourceFile sf( line );
            int numLines = sf.getNumOfLinesOfFile();

            if(numLines > 0 ) {

                files++;
                sourceFiles.push_back( std::move( sf ) );
                locsTotal+=numLines;

            }
            
        }
    }

    auto it= std::max_element( sourceFiles.begin( ), sourceFiles.end( ), [ ] ( SourceFile & sf1, SourceFile & sf2 ) -> bool
            {
              return sf1.getNumOfLinesOfFile( ) < sf2.getNumOfLinesOfFile( );
            });

    m_maxLinesPerFile = it->getNumOfLinesOfFile( );

    std::cout << "done.\n\n";

    // Generate matrix large enough for all files
    matrix_size = (long)m_maxLinesPerFile * m_maxLinesPerFile;
    m_pMatrix = std::vector<bool>( matrix_size, false );
    std::cout << "Max size of long = " << std::numeric_limits<long>::max( ) << endl;
    std::cout << "Try to reserve a vector with " << matrix_size << " elements" << endl;
    std::cout << "Maximum size of a 'vector' is " << m_pMatrix.max_size() << "\n";


    int blocksTotal = 0;

    try
    {

    // Compare each file with each other
    for(int i=0;i<(int)sourceFiles.size();i++){

        std::cout << sourceFiles[i].getFilename();
        int blocks = 0;
        
        blocks+=process( sourceFiles[i], sourceFiles[i], outfile );
        for(int j=i+1;j<(int)sourceFiles.size();j++){

            if ( ( m_ignoreSameFilename && isSameFilename( sourceFiles[ i ].getFilename(), sourceFiles[j].getFilename() ) ) == false ) {

                blocks+=process( sourceFiles[ i ], sourceFiles[ j ], outfile );
            }
        }

        if(blocks > 0){
            std::cout << " found: " << blocks << " block(s)" << std::endl;
        } else {
            std::cout << " nothing found." << std::endl;
        }

        blocksTotal+=blocks;
    }
    }
    catch( std::out_of_range & exc )
    {
        cout << "Out range error " << exc.what( ) << endl << endl;
    }



    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << "Time: "<< duration << " seconds" << std::endl;

    _report_generator->writeSummary( files, blocksTotal, locsTotal, m_DuplicateLines, duration );
}

int Clamp (int upper, int lower, int value)
{
    return std::max( lower, std::min( upper, value ) );
}

/**
 * Main routine
 *
 * @param argc  number of arguments
 * @param argv  arguments
 */
const int MIN_BLOCK_SIZE = 4;
const int MIN_CHARS = 3;

void DisplayHelp( );

int main(int argc, const char* argv[]){
    ArgumentParser ap(argc, argv);


    if(!ap.is("--help") && argc > 2){
        Duplo duplo(
            argv[argc-2], 
            ap.getInt("-ml", MIN_BLOCK_SIZE), 
            Clamp( 100, 0, ap.getInt("-pt", 100) ),
            ap.getInt("-mc", MIN_CHARS), 
            ap.is("-ip"), ap.is("-d"), ap.is("-xml")
        );
        duplo.run(argv[argc-1]);
    } else {
        DisplayHelp( );
    }

    return 0;
}

void DisplayHelp( )
{
    std::cout << "\nNAME\n";
    std::cout << "       Duplo " << VERSION << " - duplicate source code block finder\n\n";

    std::cout << "\nSYNOPSIS\n";
    std::cout << "       duplo [OPTIONS] [INTPUT_FILELIST] [OUTPUT_FILE]\n";

    std::cout << "\nDESCRIPTION\n";
    std::cout << "       Duplo is a tool to find duplicated code blocks in large\n";
    std::cout << "       C/C++/Java/C#/VB.Net software systems.\n\n";

    std::cout << "       -ml              minimal block size in lines (default is " << MIN_BLOCK_SIZE << ")\n";
    std::cout << "       -pt              percentage of lines of duplication threshold to override -ml\n";
    std::cout << "                        (default is 100%)\n";
    std::cout << "                        useful for identifying whole file class duplication\n";
    std::cout << "       -mc              minimal characters in line (default is " << MIN_CHARS << ")\n";
    std::cout << "                        lines with less characters are ignored\n";
    std::cout << "       -ip              ignore preprocessor directives\n";
    std::cout << "       -d               ignore file pairs with same name\n";
    std::cout << "       -xml             output file in XML\n";
    std::cout << "       INTPUT_FILELIST  input filelist\n";
    std::cout << "       OUTPUT_FILE      output file\n";

    std::cout << "\nVERSION\n";
    std::cout << "       " << VERSION << "\n";

    std::cout << "\nAUTHORS\n";
    std::cout << "       Christian M. Ammann (cammann@giants.ch)\n";    
    std::cout << "       Trevor D'Arcy-Evans (tdarcyevans@hotmail.com)\n\n";    
}

