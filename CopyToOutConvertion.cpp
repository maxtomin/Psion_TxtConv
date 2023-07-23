// CopyToOutConvertion.cpp: implementation of the CCopyToOutConvertion class.
//
//////////////////////////////////////////////////////////////////////

#include "CopyToOutConvertion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCopyToOutConvertion::CCopyToOutConvertion() : CTextConversion()
{

}

CCopyToOutConvertion::~CCopyToOutConvertion()
{

}

TBool CCopyToOutConvertion::DoStepL(TInt &aFilePos)
{
	//User::LeaveNoMemory();
	if (iFileReader.Eof()) return ETrue;
	iFileReader.ReadChar(iChar);
	++aFilePos;
	iFileWriter.WriteCharL(iChar);
	return EFalse;
}
