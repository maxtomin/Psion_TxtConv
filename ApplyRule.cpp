// ApplyRule.cpp: implementation of the CApplyRule class.
//
//////////////////////////////////////////////////////////////////////

#include "ApplyRule.h"

_LIT(KRuleStringParser, "Rule string parser");
enum {EEndOfLineAfterBackslash, EWrongHexNumber, EWrongDecNumber, EWrongCommand};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplyRule::CApplyRule(CReplaceRule &aReplaceRule) :
	CTextConversion(),
	iReplaceRule(aReplaceRule)
{
	PrevChar = ' ';
	iResult = EFalse;
}

CApplyRule::~CApplyRule()
{

}
void CApplyRule::ConstructL(RFile *aSource, RFile *aDest) {
	CTextConversion::ConstructL(aSource, aDest);

	NormalizeStringL(iNormalizedFind, iReplaceRule.iFindStr);
	NormalizeStringL(iNormalizedReplace, iReplaceRule.iReplaceStr);
}

TBool CApplyRule::DoStepL(TInt &aFilePos)
{
	if (!iFileReader.ReadChar(OldChar)) return ETrue;
	++aFilePos;
	
	if (iNormalizedFind[0] == OldChar || 
		!iReplaceRule.iCaseSensitive && LowerCase(iNormalizedFind[0]) == LowerCase(OldChar))  {
		iFileReader.StorePos();
		CurDesPos = 1;
		while (CurDesPos < iNormalizedFind.Length() && 
			iFileReader.ReadChar(Char) && 
			(iNormalizedFind[CurDesPos] == Char ||
			 !iReplaceRule.iCaseSensitive && LowerCase(iNormalizedFind[CurDesPos]) == LowerCase(Char))) CurDesPos++;
		TBool Match = (CurDesPos == iNormalizedFind.Length());
		TUint8 NextChar = ' ';
		if (Match && iReplaceRule.iMatchWholeWord) {
			iFileReader.ReadChar(NextChar);
			Match = (PrevChar == ' ' && NextChar == ' ');
		}
		
		if (Match) {
			aFilePos += iNormalizedFind.Length();
			if (!iReplaceRule.iMatchWholeWord)
				--aFilePos;

			iResult = ETrue;
			//writing replace sequence:
			for (TInt i = 0; i < iNormalizedReplace.Length(); i++)
				iFileWriter.WriteCharL(iNormalizedReplace[i]);
			if (iReplaceRule.iMatchWholeWord)
				iFileWriter.WriteCharL(NextChar);
		}
		else {
			iFileWriter.WriteCharL(OldChar);
			iFileReader.RestorePos();
		}
	}
	else {
		PrevChar = OldChar;
		iFileWriter.WriteCharL(OldChar);
	}
	return EFalse;
}

void CApplyRule::NormalizeStringL(TDes &aResult, TDesC &aSource)
{
	aResult.Zero();
	TUint8 Char = 0;
	for (TInt i = 0; i < aSource.Length(); i++) {
		if (aSource[i] != TUint8('\\'))
			aResult.Append(aSource[i]);
		else {
			i++;
			if (i >= aSource.Length()) User::Panic(KRuleStringParser, EEndOfLineAfterBackslash);

			switch (aSource[i]) {
			case '\\':
				aResult.Append('\\');
				break;
			case 'n':
				aResult.Append(13);
				aResult.Append(10);
				break;
			case 't':
				aResult.Append(9);
				break;
			case 'x':
				i++;
				if (i >= aSource.Length()) User::Panic(KRuleStringParser, EWrongHexNumber);

				if (aSource[i] >= TUint8('0') && aSource[i] <= TUint8('9')) 
					Char = TUint8(aSource[i] - '0');
				else if (aSource[i] >= TUint8('A') && aSource[i] <= TUint8('F'))
					Char = TUint8(aSource[i] - 'A' + 10);
				else
					User::Panic(KRuleStringParser, EWrongHexNumber);
				Char <<= 4;

				i++;
				if (i >= aSource.Length()) User::Panic(KRuleStringParser, EWrongHexNumber);

				if (aSource[i] >= TUint8('0') && aSource[i] <= TUint8('9')) 
					Char = TUint8(Char - '0'+ aSource[i]);
				else if (aSource[i] >= TUint8('A') && aSource[i] <= TUint8('F'))
					Char = TUint8(Char - 'A' + aSource[i] + 10);
				else
					User::Panic(KRuleStringParser, EWrongHexNumber);

				aResult.Append(Char);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				Char = TUint8((aSource[i] - '0') * 10);
				if (++i >= aSource.Length()) User::Panic(KRuleStringParser, EWrongDecNumber);
				if (aSource[i] < TUint8('0') || aSource[i] > TUint8('9')) User::Panic(KRuleStringParser, EWrongDecNumber);
				Char = TUint8(Char - '0' + aSource[i]);
				Char *= 10;
				if (++i >= aSource.Length()) User::Panic(KRuleStringParser, EWrongDecNumber);
				if (aSource[i] < TUint8('0') || aSource[i] > TUint8('9')) User::Panic(KRuleStringParser, EWrongDecNumber);
				Char = TUint8(Char - '0' + aSource[i]);
				aResult.Append(Char);
				break;
			default:
				if (++i >= aSource.Length()) User::Panic(KRuleStringParser, EWrongCommand);
			}
		}


	}
}

TUint8 CApplyRule::LowerCase(TUint8 Char)
{
	if (Char >= TUint8('À') && Char <= TUint8('ß'))
		return TUint8(Char - 'À' + 'à');
	else if (Char >= TUint8('A') && Char <= TUint8('Z'))
		return TUint8(Char - 'A' + 'a');
	else
		return Char;
}
