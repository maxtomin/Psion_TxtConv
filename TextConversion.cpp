// TextConversion.cpp: implementation of the CTextConversion class.
//
//////////////////////////////////////////////////////////////////////

#include "TextConversion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextConversion::CTextConversion() : iResult(0)
{
}

CTextConversion::~CTextConversion()
{
}


void CTextConversion::ConstructL(RFile *aSource, RFile *aDest)
{
	iFileReader.AttachL(*aSource);
	iFileWriter.AttachL(*aDest);
}

void CTextConversion::DestructL()
{
	iFileWriter.CommitL();
}
