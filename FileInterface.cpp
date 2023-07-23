// FileInterface.cpp: implementation of the CFileInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "FileInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileInterface::CFileInterface()
{

}

CFileInterface::~CFileInterface()
{

}

void CFileInterface::AttachL(RFile &aFile)
{
	iFile = aFile;
	iFilePos = 0;
	iFile.Seek(ESeekStart, iFilePos);

	iBufPos = 0;
	iBuffer.Zero();
	User::LeaveIfError(iFile.Size(iFileSize));
}