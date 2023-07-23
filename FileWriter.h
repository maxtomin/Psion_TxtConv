// FileWriter.h: interface for the CFileWriter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEWRITER_H__52AD2DB3_233B_4B8A_BBCC_83EE183BFA02__INCLUDED_)
#define AFX_FILEWRITER_H__52AD2DB3_233B_4B8A_BBCC_83EE183BFA02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileInterface.h"

class CFileWriter : public CFileInterface  
{
public:
	void ResetOutputL();
	void CommitL();
	void AttachL(RFile &aFile);
	void WriteCharL(TUint8 Char);
	void WriteDesL(const TDesC &aDes);
	void FlushL();
};

#endif // !defined(AFX_FILEWRITER_H__52AD2DB3_233B_4B8A_BBCC_83EE183BFA02__INCLUDED_)
