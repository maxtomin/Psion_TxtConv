// TcrCompressor.h: interface for the CTcrCompressor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCRCOMPRESSOR_H__97B0F742_99A2_481B_B7C5_13D26DFF59E9__INCLUDED_)
#define AFX_TCRCOMPRESSOR_H__97B0F742_99A2_481B_B7C5_13D26DFF59E9__INCLUDED_

#include "FileReader.h"
#include "FileWriter.h"
#include "FreqInfo.h"
#define CFreqArray CArrayFixFlat <CFreqInfo>
#define iBuffer (pBuffer->Des())

class CTcrCompressor  
{
private: //Data
	CFileReader *iInStream;
	CFileWriter	*iOutStream;
	char *pBuffer;
public:
	CTcrCompressor(CFileReader *aInStream, CFileWriter	*iOutStream);
	void CompressL();
};

#endif // !defined(AFX_TCRCOMPRESSOR_H__97B0F742_99A2_481B_B7C5_13D26DFF59E9__INCLUDED_)
