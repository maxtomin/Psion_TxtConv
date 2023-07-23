// FileReader.cpp: implementation of the CFileReader class.
//
//////////////////////////////////////////////////////////////////////

#include "FileReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TInt CFileReader::ReadChar(TUint8 &aChar)
{
	if (iFilePos == iFileSize) 
		return EFalse;
	if (iBufPos == BUFFER_SIZE)	
		Flush();
	aChar = iBuffer[iBufPos++];
	iFilePos++;

	return ETrue;
}

void CFileReader::Flush()
{
	TInt ReadSize = BUFFER_SIZE;
	if (ReadSize > (iFileSize - iFilePos)) ReadSize = iFileSize - iFilePos;
	iFile.Seek(ESeekStart, iFilePos);
	iFile.Read(iBuffer, ReadSize);
	iBufPos = 0;
}



void CFileReader::AttachL(RFile &aFile)
{
	CFileInterface::AttachL(aFile);
	Flush();
}

void CFileReader::StorePos()
{
	iOldBuffer = iBuffer;
	iOldBufPos = iBufPos;
	iOldFilePos = iFilePos;
	
}

void CFileReader::RestorePos()
{
	iBuffer = iOldBuffer;
	iBufPos = iOldBufPos;
	
	iFilePos = iOldFilePos;

}
