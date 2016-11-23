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

#include "SourceFile.h"

#include "TextFile.h"
#include "StringUtil.h"

#include <algorithm>
#include <assert.h>


unsigned int SourceFile::m_minChars = 3;
bool SourceFile::m_ignorePrepStuff = false;

SourceFile::SourceFile(const std::string& fileName ) :
    m_fileName(fileName),
    m_FileType(FileType::GetFileType(fileName))
{
	TextFile listOfFiles(m_fileName.c_str());

    std::vector<std::string> lines;
    listOfFiles.readLines(lines, false);

    //Get lines that the file has.
    m_linesOfFile = lines.size( );
	
    int openBlockComments = 0;
    int index = 0;
    for( auto & line : lines ){
        
        int lineSize = static_cast<int>(line.size() );
        std::string tmp;
        tmp.reserve( lineSize );

        // Remove block comments
        if (FileType::FILETYPE_C    == m_FileType ||
            FileType::FILETYPE_CPP  == m_FileType ||
            FileType::FILETYPE_CXX  == m_FileType ||
            FileType::FILETYPE_H    == m_FileType ||
            FileType::FILETYPE_HPP  == m_FileType ||
            FileType::FILETYPE_JAVA == m_FileType ||
            FileType::FILETYPE_CS   == m_FileType ||
            FileType::FILETYPE_QML  == m_FileType){

            for( int j=0 ; j< lineSize ; j++ ) {

                //if(line[j] == '/' && line[std::min(lineSize-1, j+1)] == '*'){
                if( j < ( lineSize - 1 ) && line.substr( j, 2 ) == "/*" ) {
                //if( j< ( lineSize -1 ) && line.substr( j, 2 ) == "/*" ) {

                    openBlockComments++;
                }

                if(openBlockComments <= 0) {

                    tmp.push_back(line[j]);
                }

                //if(line[std::max(0, j-1)] == '*' && line[j] == '/'){
                //if( j < ( lineSize - 1 ) && line.substr( j, 2 ) == "*/" ) {
                if( j > 0 && j < ( lineSize ) && line.substr( j - 1, 2 ) == "*/" ) {

                    openBlockComments--;
                }
            }
        }
        if (FileType::FILETYPE_VB == m_FileType) {

            tmp = line;
        }

        AddToLines( tmp , index );

        index++;
	}
}

void SourceFile::AddToLines( const std::string & tmp ,int index )
{
    std::string cleaned;
    getCleanLine(tmp, cleaned);
    
    if(isSourceLine(cleaned)){
        m_sourceLines.push_back(new SourceLine(cleaned, index));
    }
}

void SourceFile::getCleanLine(const std::string& line, std::string& cleanedLine){
    // Remove single line comments
	int lineSize = (int)line.size();

    cleanedLine.reserve( lineSize );

    for( int i=0;i< lineSize; i++ ) {

        switch (m_FileType)
        {
            case FileType::FILETYPE_C   :
            case FileType::FILETYPE_CPP :
            case FileType::FILETYPE_CXX :
            case FileType::FILETYPE_H   :
            case FileType::FILETYPE_HPP :
            case FileType::FILETYPE_JAVA:
            case FileType::FILETYPE_CS  :
            case FileType::FILETYPE_QML :
                if( ( i < lineSize -1 ) && line.substr( i, 2 ) == "//" ) {
                //if(i < lineSize-1 && line[i] == '/' && line[i+1] == '/'){
                    return;
                }
                break;

            case FileType::FILETYPE_VB  :
                if(i < lineSize-1 && line[i] == '\''){
                    return;
                }
                break;
            default:
                break;
        }
        cleanedLine.push_back(line[i]);
    }
}

bool SourceFile::isSourceLine(const std::string& line){
    std::string tmp = StringUtil::trim(line);

    // filter min size lines
    if (tmp.size() < m_minChars)
    {
        return false;
    }

    std::transform(tmp.begin(), tmp.end(), tmp.begin(), tolower);

    if(m_ignorePrepStuff){
        switch (m_FileType)
        {
            case FileType::FILETYPE_C   :
            case FileType::FILETYPE_CPP :
            case FileType::FILETYPE_CXX :
            case FileType::FILETYPE_H   :
            case FileType::FILETYPE_HPP :
            case FileType::FILETYPE_JAVA:
            case FileType::FILETYPE_QML:
                if(tmp[0] == '#')
                {
                    return false;
                }
                break;

            case FileType::FILETYPE_CS  :
                {
                if(tmp[0] == '#')
                {
                    return false;
                }
                // look for other markers to avoid
                const std::string PreProc_CS[] = { "using", "private", "protected", "public" };

                for (int i=0; i<4; i++ ) {
                 if (tmp.find(PreProc_CS[i].c_str(), 0, PreProc_CS[i].length())!=std::string::npos)
                    return false;
                  }
               }
               break;

            case FileType::FILETYPE_VB  :
                {
                // look for preprocessor marker in start of string
                const std::string PreProc_VB = "imports";

                return std::string::npos == tmp.find(PreProc_VB.c_str(), 0, PreProc_VB.length());
                }
                break;
            default:
                break;
        }
    }

    bool bRet = (tmp.size() >= m_minChars);
    assert(bRet);
    
    return bRet && std::find_if(tmp.begin(), tmp.end(), isalpha)!=tmp.end();
}

int SourceFile::getNumOfLinesOfCode(){
	return (int)m_sourceLines.size();
}

SourceLine* SourceFile::getLine(const int index){
	return m_sourceLines[index];
}

const std::string& SourceFile::getFilename () const {
	return m_fileName;
}

int SourceFile::getNumOfLinesOfFile( )
{
    return m_linesOfFile;
}
    
void SourceFile::setMinChars( unsigned int a_min_chars )
{
    m_minChars = a_min_chars;
}
void SourceFile::setIgnorePreprocessor( bool a_ignore )
{
    m_ignorePrepStuff = a_ignore;
}
