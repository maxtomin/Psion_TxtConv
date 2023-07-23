// ParseHtmlConvertion.cpp: implementation of the CParseHtmlConvertion class.
//
//////////////////////////////////////////////////////////////////////

#include "ParseHtmlConvertion.h"
#define IN_COMMENT 0x01
#define IN_STYLE 0x02
#define IN_SCRIPT 0x04

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseHtmlConvertion::CParseHtmlConvertion(): CTextConversion()
{
	iPrevSpace = ETrue;
	iNoOut = 0;
	iInPre = EFalse;
}

CParseHtmlConvertion::~CParseHtmlConvertion()
{

}

_LIT(KStartStyle, "style");
_LIT(KStartScript, "script");
_LIT(KEndStyle, "/style");
_LIT(KEndScript, "/script");
_LIT(KStartPre, "pre");
_LIT(KStartXmp, "xmp");
_LIT(KEndPre, "/pre");
_LIT(KEndXmp, "/xmp");

_LIT(KStartHR, "hr");
_LIT(KStartLI, "li");
_LIT(KStartP, "p");
_LIT(KStartDIV, "div");
_LIT(KStartBR, "br");
_LIT(KStartDD, "dd");
_LIT(KStartDT, "dt");
_LIT(KStartUL, "ul");
_LIT(KOutputHR, "\n\t---\n\t");
_LIT(KOutputLI, "\n\t* ");
_LIT(KOutputP, "\n");
_LIT(Knbsp, "nbsp");
_LIT(Kquot, "quot");
_LIT(Klaquo, "laquo");
_LIT(Kraquo, "raquo");
_LIT(Kldquo, "ldquo");
_LIT(Krdquo, "rdquo");
_LIT(Kbdquo, "bdquo");
_LIT(Klt, "lt");
_LIT(Kgt, "gt");
_LIT(Kndash, "ndash");
_LIT(Kmdash, "mdash");
_LIT(Kcopy, "copy");
_LIT(Kreg, "reg");
_LIT(Ktrade, "trade");
_LIT(Khellip, "hellip");
_LIT(Ktimes, "times");
_LIT(Kdivide, "divide");
_LIT(Kamp, "amp");

TBool CParseHtmlConvertion::DoStepL(TInt &aFilePos) {
	if (iFileReader.Eof()) return ETrue;

	iFileReader.ReadChar(Char);
	++aFilePos;
	switch (Char) {
	case '\r':
		return EFalse;
	case '\n':
		if (iInPre) 
			iFileWriter.WriteCharL(Char);
		else if (!iPrevSpace) {
			iFileWriter.WriteCharL(' ');
			iPrevSpace = ETrue;
		}
		return EFalse;
	case '<':
		iTag.Zero();
		for (;;) {
			if (!iFileReader.ReadChar(Char)) return ETrue;
			++aFilePos;
			if (Char == '>') break;
			if (iTag.Length() < 16)	iTag.Append(User::LowerCase(Char));
		}
		if (!iTag.Compare(KStartStyle))
			iNoOut |= IN_STYLE;
		else if (!iTag.Compare(KStartScript))
			iNoOut |= IN_SCRIPT;
		else if (!iNoOut) {
			if (!iTag.Compare(KStartPre) || !iTag.Compare(KStartXmp))
				iInPre = ETrue;
			else if (iFileWriter.FilePos()) {
				if (!iTag.Compare(KStartHR)) 
					iFileWriter.WriteDesL(KOutputHR);
				else if (!iTag.Compare(KStartLI)) 
					iFileWriter.WriteDesL(KOutputLI);
				else if (!iTag.Compare(KStartP) || !iTag.Compare(KStartDIV) || !iTag.Compare(KStartBR) || !iTag.Compare(KStartDD) || !iTag.Compare(KStartDT) || !iTag.Compare(KStartUL) ||
					iTag.Length() == 2 && iTag[0] == 'h' && iTag[1] >= '0' && iTag[1] <= '9' ||
					iTag.Length() == 3 && iTag[0] == '/' && iTag[1] == 'h' && iTag[2] >= '0' && iTag[2] <= '9'
					) 
				
					iFileWriter.WriteDesL(KOutputP);
			}
		}
		else {
			if (!iTag.Compare(KEndPre) || !iTag.Compare(KEndXmp))
				iInPre = EFalse;
			else if (!iTag.Compare(KEndStyle))
				iNoOut &= ~IN_STYLE;
			else if (!iTag.Compare(KEndScript))
				iNoOut &= ~IN_SCRIPT;
		}

		return EFalse;
	default:
		if (!iNoOut)
			if (Char == '&') {
				iTag.Zero();
				for (;;) {
					if (!iFileReader.ReadChar(Char)) break;
					++aFilePos;
					if (Char == ';') break;
					iTag.Append(User::LowerCase(Char));
					if (iTag.Length() >= 16) break;
				}
				if (iTag[0] == '#') {
					iTag.Delete(0, 1);
					TInt Code = 0;
					iTag.Num(Code);
					iFileWriter.WriteCharL((TUint8)Code);
					iPrevSpace = (Code == 32);
				}
				else {
					iPrevSpace = EFalse;
					if (!iTag.Compare(Knbsp)) 
						iFileWriter.WriteCharL(' ');
					else if (!iTag.Compare(Kquot) ||
						!iTag.Compare(Klaquo) ||
						!iTag.Compare(Kraquo) ||
						!iTag.Compare(Kldquo) ||
						!iTag.Compare(Krdquo) ||
						!iTag.Compare(Kbdquo))
						iFileWriter.WriteCharL('\"');
					else if (!iTag.Compare(Klt))
						iFileWriter.WriteCharL('<');
					else if (!iTag.Compare(Kgt)) 
						iFileWriter.WriteCharL('>');
					else if (!iTag.Compare(Kndash) || 
						!iTag.Compare(Kmdash)) 
						iFileWriter.WriteCharL('_');
					else if (!iTag.Compare(Kcopy)) {
						iFileWriter.WriteCharL('(');
						iFileWriter.WriteCharL('C');
						iFileWriter.WriteCharL(')');
					}
					else if (!iTag.Compare(Kreg)) {
						iFileWriter.WriteCharL('(');
						iFileWriter.WriteCharL('R');
						iFileWriter.WriteCharL(')');
					}
					else if (!iTag.Compare(Ktrade)) {
						iFileWriter.WriteCharL('(');
						iFileWriter.WriteCharL('T');
						iFileWriter.WriteCharL('M');
						iFileWriter.WriteCharL(')');
					}
					else if (!iTag.Compare(Khellip)) {
						iFileWriter.WriteCharL('.');
						iFileWriter.WriteCharL('.');
						iFileWriter.WriteCharL('.');
					}
					else if (!iTag.Compare(Ktimes)) 
						iFileWriter.WriteCharL('x');
					else if (!iTag.Compare(Kdivide)) 
						iFileWriter.WriteCharL(':');
					else if (!iTag.Compare(Kamp)) 
						iFileWriter.WriteCharL('&');
					else 
						iFileWriter.WriteDesL(iTag);
				}
			}
			else if (!iPrevSpace || Char != ' ' || iInPre) {
				iPrevSpace = Char == ' ';
				iFileWriter.WriteCharL(Char);
			}
	}

	return EFalse;
/*		
	while (!iFileReader.Eof() && StepLength++ < STEP_LENGTH) {
		while (iFileReader.ReadChar(Char) && Char != '<' && Char != '&') 
			iFileWriter.WriteCharL(Char);
		
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
	aFilePos = iFileReader.FilePos();
	return iFileReader.Eof(); */
}
