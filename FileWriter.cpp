// FileWriter.cpp: implementation of the CFileWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "FileWriter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void CFileWriter::WriteCharL(TUint8 Char)
{
	iBuffer.Append(Char);
	++iFilePos;
	if (iBuffer.Length() == BUFFER_SIZE) 
		FlushL();
}

void CFileWriter::WriteDesL(const TDesC &aDes) {
	for (TInt i = 0; i < aDes.Length(); i++)
		WriteCharL(aDes[i]);
}


void CFileWriter::FlushL()
{
	User::LeaveIfError(iFile.Write(iBuffer)); //leave if not enough memory
/*	iFilePos += iBuffer.Length();
	if (iFilePos > iFileSize) iFileSize = iFilePos;*/
	iBuffer.Zero();
}

void CFileWriter::AttachL(RFile &aFile) 
{
	CFileInterface::AttachL(aFile);
	User::LeaveIfError(iFile.SetSize(0));
}

void CFileWriter::CommitL()
{
	FlushL();
	if (iFilePos < iFileSize)
		iFile.SetSize(iFilePos);
	iFile.Flush();
}

void CFileWriter::ResetOutputL()
{
	AttachL(iFile);
}
