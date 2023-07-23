// ParseHtmlConvertion.h: interface for the CParseHtmlConvertion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEHTMLCONVERTION_H__B5C92D38_A5CC_4BAB_A226_2D7E1CBF61A9__INCLUDED_)
#define AFX_PARSEHTMLCONVERTION_H__B5C92D38_A5CC_4BAB_A226_2D7E1CBF61A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TextConversion.h"

class CParseHtmlConvertion : public CTextConversion  
{
private:
	TUint8 Char;
	TUint8 iNoOut;
	TBool iInPre;
	TBuf <16> iTag;
	TBool iPrevSpace, iInDiv;
public:
	virtual TBool DoStepL(TInt &aFilePos);
	CParseHtmlConvertion();
	virtual ~CParseHtmlConvertion();

};

#endif // !defined(AFX_PARSEHTMLCONVERTION_H__B5C92D38_A5CC_4BAB_A226_2D7E1CBF61A9__INCLUDED_)
