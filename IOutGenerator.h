
#if!defined __I_OUT_GENERATOR__
#define __I_OUT_GENERATOR__

class IOutGenerator
{
    void writeHeader( unsigned int m_minBlockSize, 
                      unsigned int m_minChars,
		      bool m_ignorePrepStuff, 
                      bool m_ignoreSameFilename) = 0;
    void reportSeq(int line1, 
		   int line2, 
		   int count, 
		   const SourceFile& pSource1, 
		   const SourceFile& pSource2 ) = 0; 

    void writeSummary( int num_files, 
                       int blocks_total,
                       int locks_total,
                       int num_duplicate_lines, 
                       double duration
                       ) = 0;
};



#endif

