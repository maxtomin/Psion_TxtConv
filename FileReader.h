// FileReader.h: interface for the CFileReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADER_H__C9DEC2EB_CC6F_45CD_A19A_6E759E7444C5__INCLUDED_)
#define AFX_FILEREADER_H__C9DEC2EB_CC6F_45CD_A19A_6E759E7444C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileInterface.h"

class CFileReader : public CFileInterface  
{
private:
	TBuf <BUFFER_SIZE> iOldBuffer;
	TInt iOldBufPos, iOldFilePos;

public:
	void RestorePos();
	void StorePos();
	void AttachL(RFile &aFile);
	TInt ReadChar(TUint8 &aChar);
	TInt Eof() {return (iFileSize == iFilePos);}

protected:
	void Flush();
};

#endif // !defined(AFX_FILEREADER_H__C9DEC2EB_CC6F_45CD_A19A_6E759E7444C5__INCLUDED_)
