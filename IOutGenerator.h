
#if!defined __I_OUT_GENERATOR__
#define __I_OUT_GENERATOR__

#include <string>

class SourceFile;

class IOutGenerator
{
    public:
    virtual void writeHeader( unsigned int m_minBlockSize, 
                      unsigned int m_minChars,
		      bool m_ignorePrepStuff, 
                      bool m_ignoreSameFilename,
                      const std::string & version) = 0;
    virtual void reportSeq(int line1, 
		   int line2, 
		   int count, 
		   const SourceFile& pSource1, 
		   const SourceFile& pSource2 ) = 0; 

    virtual void writeSummary( int num_files, 
                       int blocks_total,
                       int locks_total,
                       int num_duplicate_lines, 
                       double duration
                       ) = 0;
};



#endif

