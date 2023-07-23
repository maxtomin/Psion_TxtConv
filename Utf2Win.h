// Utf2Win.h: interface for the CUtf2Win class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTF2WIN_H__A13213DB_FFE2_4797_835A_904C31E3FABF__INCLUDED_)
#define AFX_UTF2WIN_H__A13213DB_FFE2_4797_835A_904C31E3FABF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TextConversion.h"

class CUtf2Win : public CTextConversion  
{
private:
	TUint8 iStartChar;
	TUint8 iCharLength;
	TUint iCode;
	TUint Codes[128];
protected:
	virtual TBool DoStepL(TInt &aFilePos);
public:
	CUtf2Win();
	virtual ~CUtf2Win();

};

class CWin2Utf : public CTextConversion  
{
private:
	TUint iCode;
public:
	CWin2Utf();
	virtual ~CWin2Utf();

protected:
	TBool DoStepL(TInt &aFilePos);
};

#endif // !defined(AFX_UTF2WIN_H__A13213DB_FFE2_4797_835A_904C31E3FABF__INCLUDED_)
