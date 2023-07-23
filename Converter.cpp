// Converter.cpp: implementation of the CConverter class.
//
//////////////////////////////////////////////////////////////////////

#include "Converter.h"
#include "TcrCompressor.h"
#include "ConverterActive.h"
#include "TextConverter.rsg"

_LIT(KDos2Win, "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF\xA8\xB8\xAA\xBA\xAF\xBF\xA1\xA2\xB0\x95\xB7\x00\xB9\xA4\x00\xA0");
_LIT(KKoi2Win, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x95\x00\x00\x00\x00\xA0\x00\xB0\x00\xB7\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xA8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xA9\xFE\xE0\xE1\xF6\xE4\xE5\xF4\xE3\xF5\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xFF\xF0\xF1\xF2\xF3\xE6\xE2\xFC\xFB\xE7\xF8\xFD\xF9\xF7\xFA\xDE\xC0\xC1\xD6\xC4\xC5\xD4\xC3\xD5\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xDF\xD0\xD1\xD2\xD3\xC6\xC2\xDC\xDB\xC7\xD8\xDD\xD9\xD7\xDA");
_LIT(KRuleStringParser, "Rule string parser");
enum {EEndOfLineAfterBackslash, EWrongHexNumber, EWrongDecNumber, EWrongCommand};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConverter::CConverter(RFile *aInFile, 
					   RFile *aOutFile, 
					   RFile *aTempFile
					   ):
	iInFile((RFile *)&aInFile),
	iOutFile((RFile *)&aOutFile),
	iTempFile((RFile *)&aTempFile)
//owns 	aInFile, aOutFile, aTempFile

{

	
}

CConverter::~CConverter()
{
	delete iInFile;
	delete iOutFile;
	delete iTempFile;
}

void CConverter::DoConvertL(CRulesetOptions *aRulesetOptions, CReplaceRules *aReplaceRules)
{
	iCurSource = (RFile *)iInFile;
	iCurDest = (RFile *)iTempFile;
	iDestIsOut = EFalse;
	
	/*CActiveScheduler *testAS = new CActiveScheduler();
	CActiveScheduler::Install(NULL);
	CActiveScheduler::Install(testAS);
	*/
	

//	CProgressBarActive *testActive = new CConverterActive(iInFile, iOutFile, iTempFile, iProgressDialog, aRulesetOptions, aReplaceRules); //and also add to AS
//	iInFile = iOutFile = iTempFile = NULL;

	//iProgressDialog->DoExecuteLD(testActive);
	return;


	if (aRulesetOptions->iParseHTML) {
		ParseHtmlL();
		SwapFiles();
	}

	if (aRulesetOptions->iInEncoding != ET_WIN) {
		ChangeEncodingL(GetEncoding(aRulesetOptions->iInEncoding));
		SwapFiles();
	}

	if (aRulesetOptions->iTrimFrom.Length() || aRulesetOptions->iTrimTo.Length()) {
		TrimText(aRulesetOptions->iTrimFrom, aRulesetOptions->iTrimTo);
		SwapFiles();
	}

	for (TInt i = 0; i < aReplaceRules->Count(); i++) 
	{
		ApplyRuleL((*aReplaceRules)[i]);
		SwapFiles();
	}

	if (aRulesetOptions->iConvertToTCR) {
		ConvertToTCRL();
		SwapFiles();
	}

	if (iDestIsOut) CopyTempToOutL();
}

void CConverter::SwapFiles()
{
	if (iCurSource == iInFile) iCurSource = iOutFile;
	RFile *R = iCurSource;
	iCurSource = iCurDest;
	iCurDest = R; 
	iDestIsOut = !iDestIsOut;
}

void CConverter::ParseHtmlL()
{
	InitStreamsL();

	TUint8 Char;
	TBuf <16> Tag;
	TBool PrevSpace = ETrue, InDiv;

	while (!iFileReader.Eof()) {
		while (iFileReader.ReadChar(Char) && Char != '<' && Char != '&') {
			iFileWriter.WriteCharL(Char);
			/*switch (Char) {
				case 9:
				case 10:
				case 13:
				case 32:
					if (!PrevSpace) {
						iFileWriter.WriteCharL(' ');
						PrevSpace = ETrue;
					}
					break;
				default:
					iFileWriter.WriteCharL(Char);
					PrevSpace = EFalse;
			}*/
		}
		
		Tag.Zero();
		switch (Char) {
		case '<':
			while (iFileReader.ReadChar(Char) && Char != '>') 
				if (Tag.Length() < 16)
					Tag.Append(Char);
			Tag.LowerCase();
			if (Tag == _L("/p") || Tag == _L("br") || Tag == _L("/div")) {
				iFileWriter.WriteCharL(13);
				iFileWriter.WriteCharL(10);
				PrevSpace = ETrue;
			}
			else if (Tag == _L("div"))
				InDiv = ETrue;
			
			break;
		case '&':
			while (iFileReader.ReadChar(Char) && Char != ';') 
				if (Tag.Length() < 16)
					Tag.Append(Char);
			if (Tag == _L("nbsp")) 
				iFileWriter.WriteCharL(' ');
			else if (Tag == _L("quot")) 
				iFileWriter.WriteCharL('"');
		}
	}
	ReleaseStreams();
}

void CConverter::ChangeEncodingL(TEncodingType aEncodingType)
{
	TUint8 Char;
	TDesC8 *pEncoding;
	
	
	switch (aEncodingType) {
	case ET_DOS: pEncoding = (TDesC8 *)&KDos2Win; break;
	case ET_KOI: pEncoding = (TDesC8 *)&KKoi2Win; break;
	default: pEncoding = NULL;
	}

	InitStreamsL();
	while (iFileReader.ReadChar(Char)) {
		if (Char >= 128 && pEncoding) 
			Char = (*pEncoding)[Char - 128];
		iFileWriter.WriteCharL(Char);
	}

	ReleaseStreams();
}

TEncodingType CConverter::GetEncoding(TEncodingType aEncodingType)
{
	if (aEncodingType != ET_AUTO) return aEncodingType;
	return ET_AUTO;
}

void CConverter::TrimText(TDesC &aTrimFrom, TDesC &aTrimTo)
{
	//aTrimTo - head
	//aTrimFrom - tail
	TInt CurDesPos;
	TUint8 Char, OldChar;

	InitStreamsL();
	
	//trim head
	while (iFileReader.ReadChar(Char))
		if (aTrimTo[0] == Char)  {
			iFileReader.StorePos();
			CurDesPos = 1;
			while (CurDesPos < aTrimTo.Length() && 
				iFileReader.ReadChar(Char) && 
				aTrimTo[CurDesPos] == Char) CurDesPos++;
			if (CurDesPos == aTrimTo.Length()) 
				break;
			else iFileReader.RestorePos();
		}

	//write until tail:
	while (iFileReader.ReadChar(Char)) {
		if (aTrimFrom[0] == Char)  {
			iFileReader.StorePos();
			OldChar = Char;
			CurDesPos = 1;
			while (CurDesPos < aTrimFrom.Length() && 
				iFileReader.ReadChar(Char) && 
				aTrimFrom[CurDesPos] == Char) CurDesPos++;
			if (CurDesPos == aTrimFrom.Length()) {
				ReleaseStreams(); //found it
				return;
			}
			else {
				iFileReader.RestorePos();
				iFileWriter.WriteCharL(OldChar);
			}
		}
		else iFileWriter.WriteCharL(Char);
	}
	
	ReleaseStreams();
}

void CConverter::ApplyRuleL(CReplaceRule &aReplaceRule)
{
	TBuf <FIND_REPLACE_LENGTH> iNormalizedFind, iNormalizedReplace;
	NormalizeStringL(iNormalizedFind, aReplaceRule.iFindStr);
	NormalizeStringL(iNormalizedReplace, aReplaceRule.iReplaceStr);

	TUint8 Char, OldChar, PrevChar = ' ';
	TInt CurDesPos;
	TBool IsExists;
	
	for (;;) {
		IsExists = EFalse;
		InitStreamsL();
		while (iFileReader.ReadChar(OldChar)) {
			if (iNormalizedFind[0] == OldChar || 
				!aReplaceRule.iCaseSensitive && LowerCase(iNormalizedFind[0]) == LowerCase(OldChar))  {
				iFileReader.StorePos();
				CurDesPos = 1;
				while (CurDesPos < iNormalizedFind.Length() && 
					iFileReader.ReadChar(Char) && 
					(iNormalizedFind[CurDesPos] == Char ||
					 !aReplaceRule.iCaseSensitive && LowerCase(iNormalizedFind[CurDesPos]) == LowerCase(Char))) CurDesPos++;
				TBool Match = (CurDesPos == iNormalizedFind.Length());
				TUint8 NextChar = ' ';
				if (Match && aReplaceRule.iMatchWholeWord) {
					iFileReader.ReadChar(NextChar);
					Match = (PrevChar == ' ' && NextChar == ' ');
				}
				
				if (Match) {
					IsExists = ETrue;
					//writing replace sequence:
					for (TInt i = 0; i < iNormalizedReplace.Length(); i++)
						iFileWriter.WriteCharL(iNormalizedReplace[i]);
					if (aReplaceRule.iMatchWholeWord)
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
		}

		ReleaseStreams();
		if (IsExists && aReplaceRule.iDoWhilePossible) SwapFiles();
		else break;
	}
}

void CConverter::ConvertToTCRL()
{
	InitStreamsL();
	CTcrCompressor TcrCompressor(&iFileReader, &iFileWriter);
	TcrCompressor.CompressL();
	ReleaseStreams();
}

void CConverter::CopyTempToOutL()
{
	TUint8 Char;
	InitStreamsL();	
	while (iFileReader.ReadChar(Char))
		iFileWriter.WriteCharL(Char);
	ReleaseStreams();	
}


void CConverter::InitStreamsL()
{
	iFileReader.AttachL(*iCurSource);
	iFileWriter.AttachL(*iCurDest);
}

void CConverter::ReleaseStreams()
{
	iFileWriter.CommitL();
}

//DEL TInt CConverter::ReadCharL(TUint8 &aChar)
//DEL {
//DEL 	return ;
//DEL }
	


TBool CConverter::IsSpace(TUint8 Char)
{
	switch (Char) {
	case TUint8(' '):
	case 13:
	case 10:
	case 9:
		return ETrue;
	default:
		return EFalse;
	}
}

TBool CConverter::IsSingleWord(TUint8 Char)
{
	switch (Char) {
	case TUint8('ó'):
	case TUint8('ÿ'):
	case TUint8('è'):
	case TUint8('î'):
	case TUint8('à'):
	case TUint8('â'):
	case TUint8('ê'):
	case TUint8('ñ'):
		return ETrue;
	default:
		return EFalse;
	}
}

TBool CConverter::IsVowel(TUint8 Char)
{
	switch (Char) {
	case TUint8('à'):
	case TUint8('î'):
	case TUint8('å'):
	case TUint8('è'):
		return ETrue;
	default:
		return EFalse;
	}
}

TUint8 CConverter::LowerCase(TUint8 Char)
{
	if (Char >= TUint8('À') && Char <= TUint8('ß'))
		return TUint8(Char - 'À' + 'à');
	else if (Char >= TUint8('A') && Char <= TUint8('Z'))
		return TUint8(Char - 'A' + 'a');
	else
		return Char;
}

void CConverter::NormalizeStringL(TDes &aResult, TDesC &aSource)
{
	aResult.Zero();
	TUint8 Char;
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

				Char = 0;
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
					Char = TUint8(Char - '0' + aSource[i]);
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