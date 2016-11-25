
#include "SourceFile.h"
#include "TextGenerator.h"
#include <fstream>

TextGenerator::TextGenerator( std::ofstream & outfile ) :
   outfile_( outfile )
{
}

void TextGenerator::writeHeader( unsigned int m_minBlockSize, 
				 unsigned int m_minChars,
				 bool m_ignorePrepStuff, 
				 bool m_ignoreSameFilename,
                                 const std::string & version ) 
{
    outfile_ << "duplo version=\"" << version << "\"" << std::endl;
    outfile_ << "    check Min_block_size=\"" << m_minBlockSize << 
        "\" Min_char_line=\"" << m_minChars << 
        "\" Ignore_prepro=\"" << (m_ignorePrepStuff ? "true" : "false") << 
        "\" Ignore_same_filename=\"" << (m_ignoreSameFilename ? "true" : "false") << "\"" << std::endl << std::endl;
}
void TextGenerator::reportSeq(int line1, 
			      int line2, 
			      int count, 
			      const SourceFile& pSource1, 
			      const SourceFile& pSource2 ) 
{
    outfile_ << pSource1.getFilename() << "(" << pSource1.getLine(line1).getLineNumber() << ")" << std::endl;
    outfile_ << pSource2.getFilename() << "(" << pSource2.getLine(line2).getLineNumber() << ")" << std::endl;
    for(int j=0;j<count;j++){
        outfile_ << pSource1.getLine(j+line1).getLine() << std::endl;
    }
    outfile_ << std::endl;
}

void TextGenerator::writeSummary( int num_files, 
			          int blocks_total,
			          int locks_total,
			          int num_duplicate_lines, 
			          double duration) 
{
    outfile_ << "Configuration: " << std::endl;
    outfile_ << "  Number of files: " << num_files << std::endl;
    outfile_ << std::endl;
    outfile_ << "Results: " << std::endl;
    outfile_ << "  Lines of code: " << locks_total << std::endl;
    outfile_ << "  Duplicate lines of code: " << num_duplicate_lines << std::endl;
    outfile_ << "  Total " << blocks_total << " duplicate block(s) found." << std::endl << std::endl;
    outfile_ << "  Time: " << duration << " seconds" << std::endl;
}
