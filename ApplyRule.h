// ApplyRule.h: interface for the CApplyRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLYRULE_H__61379CD8_E972_4B3E_A8FF_B668D9E44E49__INCLUDED_)
#define AFX_APPLYRULE_H__61379CD8_E972_4B3E_A8FF_B668D9E44E49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TextConversion.h"
#include "ReplaceRule.h"

class CApplyRule : public CTextConversion  
{
private:
	TUint8 LowerCase(TUint8 Char);

	CReplaceRule &iReplaceRule;
	TBuf <FIND_REPLACE_LENGTH> iNormalizedFind, iNormalizedReplace;
	TUint8 Char, OldChar, PrevChar;
	TInt CurDesPos;
public:
	static void NormalizeStringL(TDes &aResult, TDesC &aSource);
	CApplyRule(CReplaceRule &aReplaceRule);
	virtual void ConstructL(RFile *aSource, RFile *aDest);
	virtual ~CApplyRule();
	virtual TBool DoStepL(TInt &aFilePos);
};

#endif // !defined(AFX_APPLYRULE_H__61379CD8_E972_4B3E_A8FF_B668D9E44E49__INCLUDED_)
