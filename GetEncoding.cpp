// GetEncoding.cpp: implementation of the CGetEncoding class.
//
//////////////////////////////////////////////////////////////////////

#include "GetEncoding.h"

_LIT(KDos2Win, "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF\xA8\xB8\xAA\xBA\xAF\xBF\xA1\xA2\xB0\x95\xB7\x00\xB9\xA4\x00\xA0");
_LIT(KKoi2Win, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x95\x00\x00\x00\x00\xA0\x00\xB0\x00\xB7\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xA8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xA9\xFE\xE0\xE1\xF6\xE4\xE5\xF4\xE3\xF5\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xFF\xF0\xF1\xF2\xF3\xE6\xE2\xFC\xFB\xE7\xF8\xFD\xF9\xF7\xFA\xDE\xC0\xC1\xD6\xC4\xC5\xD4\xC3\xD5\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xDF\xD0\xD1\xD2\xD3\xC6\xC2\xDC\xDB\xC7\xD8\xDD\xD9\xD7\xDA");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetEncoding::CGetEncoding() : 
	CTextConversion(),
	PrevChar(' '), 
	DosChar(0), 
	KoiChar(0)
{
  for (TInt i = 0; i < 5; i++)
	  Encodings[i] = 0;
}

CGetEncoding::~CGetEncoding()
{

}

TBool CGetEncoding::DoStepL(TInt &aFilePos)
{
	if (!iFileReader.ReadChar(Char)) {
		//finalize:
		//Last char also can be single:
		if (PrevPrevCharIsSpace ) {
			if (IsSingleWord(PrevChar)) Encodings[ET_WIN] += 10;
			if (IsSingleWord(DosChar)) Encodings[ET_DOS] += 10;
			if (IsSingleWord(KoiChar)) Encodings[ET_KOI] += 10;
		}

		TEncodingType MaxType = ET_WIN;
		if (Encodings[MaxType] < Encodings[ET_DOS]) MaxType = ET_DOS;
		if (Encodings[MaxType] < Encodings[ET_KOI]) MaxType = ET_KOI;
		if (Encodings[MaxType] < Encodings[ET_UTF8]) MaxType = ET_UTF8;
		iResult = MaxType;

		return ETrue;
	}
	++aFilePos;

	if (Char >= 128) {
		//Lower case:
		Char = (TUint8)User::LowerCase(Char);
		DosChar = LowerCase(((TDesC8&)KDos2Win)[Char - 128]);
		KoiChar = LowerCase(((TDesC8&)KKoi2Win)[Char - 128]);

		if (IsVowel(Char)) 
			Encodings[ET_WIN]++;
		if (IsVowel(DosChar)) 
			Encodings[ET_DOS]++;
		if (IsVowel(KoiChar)) 
			Encodings[ET_KOI]++;
		if (Char == TUint8('ð') || Char == TUint8('ñ')) 
			Encodings[ET_UTF8]++;
	
		if (PrevPrevCharIsSpace && IsSpace(Char)) {
			if (IsSingleWord(PrevChar)) 
				Encodings[ET_WIN] += 10;
			if (IsSingleWord(DosChar)) 
				Encodings[ET_DOS] += 10;
			if (IsSingleWord(KoiChar)) 
				Encodings[ET_KOI] += 10;
		}
	}
	
	PrevPrevCharIsSpace = IsSpace(Char);
	PrevChar = Char;
	return EFalse;
}

TBool CGetEncoding::IsSingleWord(TUint8 Char)
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

TUint8 CGetEncoding::LowerCase(TUint8 Char)
{
	if (Char >= TUint8('À') && Char <= TUint8('ß'))
		return TUint8(Char - 'À' + 'à');
	else if (Char >= TUint8('A') && Char <= TUint8('Z'))
		return TUint8(Char - 'A' + 'a');
	else
		return Char;
}

TBool CGetEncoding::IsVowel(TUint8 Char)
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

TBool CGetEncoding::IsSpace(TUint8 Char)
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


CSetEncoding::CSetEncoding(TEncodingType aEncodingType) : CTextConversion() {
	switch (aEncodingType) {
	case ET_DOS: iEncoding = (TDesC8 *)&KDos2Win; break;
	case ET_KOI: iEncoding = (TDesC8 *)&KKoi2Win; break;
	default: iEncoding = NULL;
	}
}

CSetEncoding::~CSetEncoding() {
}

TBool CSetEncoding::DoStepL(TInt &aFilePos) {
	if (!iFileReader.ReadChar(iChar)) return ETrue;
	++aFilePos;

	if (iChar >= 128 && iEncoding) 
		iChar = (*iEncoding)[iChar - 128];
	iFileWriter.WriteCharL(iChar);

	
	return EFalse;
}


CSetOutEncoding::CSetOutEncoding(TEncodingType aEncodingType) : CTextConversion() {
	switch (aEncodingType) {
	case ET_DOS: iEncoding = (TDesC8 *)&KDos2Win; break;
	case ET_KOI: iEncoding = (TDesC8 *)&KKoi2Win; break;
	default: iEncoding = NULL;
	}
}

CSetOutEncoding::~CSetOutEncoding() {
}

TBool CSetOutEncoding::DoStepL(TInt &aFilePos) {
	if (!iFileReader.ReadChar(iChar)) return ETrue;
	++aFilePos;

	if (iChar >= 128 && iEncoding) {
		for (iResult = 0; iResult < 128 && (*iEncoding)[iResult] != iChar; ++iResult);
		if (iResult == 128) iChar = '?';
		else iChar = TUint8(iResult + 128);
	}
	iFileWriter.WriteCharL(iChar);

	
	return EFalse;
}
