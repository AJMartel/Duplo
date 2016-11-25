
#include "TextGenerator.h"

void TextGenerator::writeHeader( unsigned int m_minBlockSize, 
				 unsigned int m_minChars,
				 bool m_ignorePrepStuff, 
				 bool m_ignoreSameFilename) 
{
}
void TextGenerator::reportSeq(int line1, 
			      int line2, 
			      int count, 
			      const SourceFile& pSource1, 
			      const SourceFile& pSource2 ) 
{
}

void TextGenerator::writeSummary( int num_files, 
			          int blocks_total,
			          int locks_total,
			          int num_duplicate_lines, 
			          double duration) 
{
}
