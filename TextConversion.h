// TextConversion.h: interface for the CTextConversion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTCONVERSION_H__25580BEC_4CCF_43C3_AC9E_F6B88F7B94DF__INCLUDED_)
#define AFX_TEXTCONVERSION_H__25580BEC_4CCF_43C3_AC9E_F6B88F7B94DF__INCLUDED_

#include "f32file.h"
#include "FileReader.h"
#include "FileWriter.h"
#define STEP_LENGTH 1024

class CTextConversion  
{
protected:
	CFileReader iFileReader;
	CFileWriter iFileWriter;
	TUint8 iChar;
public:
	TInt iResult;
	void DestructL();
	virtual void ConstructL(RFile *aSource, RFile *aDest);
	virtual TBool DoStepL(TInt &aFilePos) = 0;
	
	CTextConversion();
	virtual ~CTextConversion();

};

#endif // !defined(AFX_TEXTCONVERSION_H__25580BEC_4CCF_43C3_AC9E_F6B88F7B94DF__INCLUDED_)
