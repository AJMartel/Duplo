
#if!defined __TEXT_GENERATOR__
#define __TEXT_GENERATOR__

#include "IOutGenerator.h"
#include <iostream>

class TextGenerator : public IOutGenerator
{
    public:

    TextGenerator( std::ofstream & outfile );
    virtual void writeHeader( unsigned int m_minBlockSize, 
                      unsigned int m_minChars,
		      bool m_ignorePrepStuff, 
                      bool m_ignoreSameFilename,
                      const std::string & version) override;
    virtual void reportSeq(int line1, 
		   int line2, 
		   int count, 
		   const SourceFile& pSource1, 
		   const SourceFile& pSource2 ) override; 

    virtual void writeSummary( int num_files, 
                       int blocks_total,
                       int locks_total,
                       int num_duplicate_lines, 
                       double duration
                       ) override;
    private:
    std::ofstream & outfile_;
};



#endif
