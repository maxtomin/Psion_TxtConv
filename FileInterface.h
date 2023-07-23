// FileInterface.h: interface for the CFileInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEINTERFACE_H__0F6A332A_C113_4C97_8126_8B825B9A8F7A__INCLUDED_)
#define AFX_FILEINTERFACE_H__0F6A332A_C113_4C97_8126_8B825B9A8F7A__INCLUDED_

#include <f32file.h>
#define BUFFER_SIZE 256

class CFileInterface  
{
protected:
	RFile iFile;
	TBuf <BUFFER_SIZE> iBuffer;
	TInt iBufPos;
	TInt iFileSize, iFilePos;
public:
	TInt FileSize() const {return iFileSize;}
	TInt FilePos() const {return iFilePos;}
	virtual void AttachL(RFile &aFile);
	CFileInterface();
	virtual ~CFileInterface();
};

#endif // !defined(AFX_FILEINTERFACE_H__0F6A332A_C113_4C97_8126_8B825B9A8F7A__INCLUDED_)
