
#include "XMLGenerator.h"
#include "SourceFile.h"
#include "StringUtil.h"
#include "TextGenerator.h"
#include <fstream>

XMLGenerator::XMLGenerator( std::ofstream & outfile ) :
    outfile_( outfile )
{
}
void XMLGenerator::writeHeader( unsigned int m_minBlockSize, 
				 unsigned int m_minChars,
				 bool m_ignorePrepStuff, 
				 bool m_ignoreSameFilename,
                                 const std::string & version ) 
{
    outfile_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    outfile_ << "<?xml-stylesheet href=\"duplo.xsl\" type=\"text/xsl\"?>" << std::endl;
    outfile_ << "<duplo version=\"" << version << "\">" << std::endl;
    outfile_ << "    <check Min_block_size=\"" << m_minBlockSize << 
        "\" Min_char_line=\"" << m_minChars << 
        "\" Ignore_prepro=\"" << (m_ignorePrepStuff ? "true" : "false") << 
        "\" Ignore_same_filename=\"" << (m_ignoreSameFilename ? "true" : "false") << "\">" << std::endl;
}
void XMLGenerator::reportSeq(int line1, 
			      int line2, 
			      int count, 
			      const SourceFile& pSource1, 
			      const SourceFile& pSource2 ) 
{
    outfile_ << "    <set LineCount=\"" << count << "\">" << std::endl;
    outfile_ << "        <block SourceFile=\"" << pSource1.getFilename() << "\" StartLineNumber=\"" << pSource1.getLine(line1).getLineNumber() << "\"/>" << std::endl;
    outfile_ << "        <block SourceFile=\"" << pSource2.getFilename() << "\" StartLineNumber=\"" << pSource2.getLine(line2).getLineNumber() << "\"/>" << std::endl;
    outfile_ << "        <lines xml:space=\"preserve\">" << std::endl;
    for(int j = 0; j < count; j++)
    {
        // replace various characters/ strings so that it doesn't upset the XML parser
        std::string tmpstr = pSource1.getLine(j+line1).getLine();
    
        // " --> '
        StringUtil::StrSub(tmpstr, "\'", "\"", -1);
    
        // & --> &amp;
        StringUtil::StrSub(tmpstr, "&amp;", "&", -1);
    
        // < --> &lt;
        StringUtil::StrSub(tmpstr, "&lt;", "<", -1);
    
        // > --> &gt;
        StringUtil::StrSub(tmpstr, "&gt;", ">", -1);
    
        outfile_ << "            <line Text=\"" << tmpstr << "\"/>" << std::endl;
    }
    outfile_ << "        </lines>" << std::endl;
    outfile_ << "    </set>" << std::endl;
}

void XMLGenerator::writeSummary( int num_files, 
			          int blocks_total,
			          int locks_total,
			          int num_duplicate_lines, 
			          double duration) 
{
    outfile_ << "        <summary Num_files=\"" << num_files <<
        "\" Duplicate_blocks=\"" << blocks_total <<
        "\" Total_lines_of_code=\"" << locks_total <<
        "\" Duplicate_lines_of_code=\"" << num_duplicate_lines <<
        "\" Time=\"" << duration <<
        "\"/>" << std::endl;
    outfile_ << "    </check>" << std::endl;
    outfile_ << "</duplo>" << std::endl;
}
