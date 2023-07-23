// GetEncoding.h: interface for the CGetEncoding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETENCODING_H__8338EBDE_1C77_49CF_80E3_239102AE95C7__INCLUDED_)
#define AFX_GETENCODING_H__8338EBDE_1C77_49CF_80E3_239102AE95C7__INCLUDED_

#include "TextConversion.h"
#include "RulesetOptions.h"


class CGetEncoding : public CTextConversion  
{
private:
	TBool IsSpace(TUint8 Char);
	TBool IsVowel(TUint8 Char);
	TUint8 LowerCase(TUint8 Char);
	TBool IsSingleWord(TUint8 Char);
	TInt Encodings[5];
	TUint8 Char, PrevChar;
	TBool PrevPrevCharIsSpace;

	TUint8 DosChar, KoiChar;

public:
	TBool DoStepL(TInt &aFilePos);
	CGetEncoding();
	virtual ~CGetEncoding();

};

class CSetEncoding : public CTextConversion  
{
private:
	TDesC *iEncoding;
	TUint8 iChar;
public:
	TBool DoStepL(TInt &aFilePos);
	CSetEncoding(TEncodingType aEncodingType);
	virtual ~CSetEncoding();

};


class CSetOutEncoding : public CTextConversion  
{
private:
	TDesC *iEncoding;
	TUint8 iChar, iResult;
public:
	TBool DoStepL(TInt &aFilePos);
	CSetOutEncoding(TEncodingType aEncodingType);
	virtual ~CSetOutEncoding();

};

#endif // !defined(AFX_GETENCODING_H__8338EBDE_1C77_49CF_80E3_239102AE95C7__INCLUDED_)
