// Utf2Win.cpp: implementation of the CUtf2Win class.
//
//////////////////////////////////////////////////////////////////////

#include "Utf2Win.h"
#define UNKNOWN_CHAR 0x98

const TUint UtfCodes[128] =
	  {
	  0xD082,
      0xD083,
      0xE2809A,
      0xD193,
      0xE2809E,
      0xE280A6,
      0xE280A0,
      0xE280A1,
      0xE282AC,
      0xE280B0,
      0xD089,
      0xE280B9,
      0xD08A,
      0xD08C,
      0xD08B,
      0x00,
      0xD08F,
      0xD192,
      0xE28098,
      0xE28099,
      0xE2809C,
      0xE2809D,
      0xE280A2,
      0xE28093,
      0xE28094,
      0xE284A2,
      0xD199,
      0xE280BA,
      0xD19A,
      0xD19C,
      0xD19B,
      0xD19F,
      0xC2A0,
      0xD08E,
      0xD19E,
      0xD088,
      0xC2A4,
      0xD089,
      0xC2A6,
      0xC2A7,
      0xD081,
      0xC2A9,
      0xD084,
      0xC2AB,
      0xC2AC,
      0xC2AD,
      0xC2AE,
      0xD087,
      0xC2B0,
      0xC2B1,
      0xD086,
      0xD196,
      0xD291,
      0xC2B5,
      0xC2B6,
      0xC2B7,
      0xD191,
      0xE28496,
      0xD194,
      0xC2BB,
      0xD198,
      0xD085,
      0xD195,
      0xD197,
      0xD090,
      0xD091,
      0xD092,
      0xD093,
      0xD094,
      0xD095,
      0xD096,
      0xD097,
      0xD098,
      0xD099,
      0xD09A,
      0xD09B,
      0xD09C,
      0xD09D,
      0xD09E,
      0xD09F,
      0xD0A0,
      0xD0A1,
      0xD0A2,
      0xD0A3,
      0xD0A4,
      0xD0A5,
      0xD0A6,
      0xD0A7,
      0xD0A8,
      0xD0A9,
      0xD0AA,
      0xD0AB,
      0xD0AC,
      0xD0AD,
      0xD0AE,
      0xD0AF,
      0xD0B0,
      0xD0B1,
      0xD0B2,
      0xD0B3,
      0xD0B4,
      0xD0B5,
      0xD0B6,
      0xD0B7,
      0xD0B8,
      0xD0B9,
      0xD0BA,
      0xD0BB,
      0xD0BC,
      0xD0BD,
      0xD0BE,
      0xD0BF,
      0xD180,
      0xD181,
      0xD182,
      0xD183,
      0xD184,
      0xD185,
      0xD186,
      0xD187,
      0xD188,
      0xD189,
      0xD18A,
      0xD18B,
      0xD18C,
      0xD18D,
      0xD18E,
      0xD18F
	  };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtf2Win::CUtf2Win()
{

}

CUtf2Win::~CUtf2Win()
{

}

TBool CUtf2Win::DoStepL(TInt &aFilePos)
{
	if (!iFileReader.ReadChar(iChar)) return ETrue;
	++aFilePos;

	iCode = iStartChar = iChar;
	iCharLength = 1;
	if (iStartChar & 0x80) {
		iStartChar <<= 1;
		while (iStartChar & 0x80) {
			iStartChar <<= 1;
			++iCharLength;
		}
	}
	else {
		iFileWriter.WriteCharL(iChar);
		return EFalse;
	}
	
	for (iStartChar = 1; iStartChar < iCharLength; ++iStartChar) {
		if (!iFileReader.ReadChar(iChar)) return ETrue;
		++aFilePos;
		if (iStartChar < 4) {
			iCode <<= 8;
			iCode += iChar;
		}
	}

	if (iCharLength > 4) {
		iFileWriter.WriteCharL(UNKNOWN_CHAR);
		return EFalse;
	}

	for (iChar = 0; iChar < 128 && UtfCodes[iChar] != iCode; ++iChar) ;
	if (iChar == 128)  
		iFileWriter.WriteCharL(UNKNOWN_CHAR);
	else
		iFileWriter.WriteCharL(TUint8(iChar + 128));
	
	return EFalse;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWin2Utf::CWin2Utf()
{

}

CWin2Utf::~CWin2Utf()
{

}

TBool CWin2Utf::DoStepL(TInt &aFilePos)
{
	if (!iFileReader.ReadChar(iChar)) return ETrue;
	++aFilePos;

	if (iChar < 128)
		iFileWriter.WriteCharL(iChar);
	else {
		iCode = UtfCodes[iChar - 128];
		iChar = TUint8(iCode >> 24);
		if (iChar) iFileWriter.WriteCharL(iChar);
		iChar = TUint8(iCode >> 16);
		if (iChar) iFileWriter.WriteCharL(iChar);
		iChar = TUint8(iCode >> 8);
		if (iChar) iFileWriter.WriteCharL(iChar);
		iChar = TUint8(iCode);
		if (iChar) iFileWriter.WriteCharL(iChar);
	}
	return EFalse;
}
