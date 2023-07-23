// RulesetOptions.cpp: implementation of the CRulesetOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "RulesetOptions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRulesetOptions::CRulesetOptions()
{
	iInEncoding = ET_AUTO;
	iOutEncoding = ET_WIN;
	iParseHTML = iConvertToTCR = EFalse;
}

CRulesetOptions::~CRulesetOptions()
{

}

/*
Stream structure (version 1):
Size	Value
4		Length of the following stream
4		iRuleDescription length
X		iRuleDescription
1		iEncodingType
1		iParseHTML
4		iTrimTo length
X		iTrimTo
4		iTrimFrom length
X		iTrimFrom
1		iConvertToTCR
*/

void CRulesetOptions::ExternalizeL(RWriteStream &aStream)
{
	aStream.WriteInt32L(4 + iRuleDescription.Length() + 1 + 1 + 4 + iTrimTo.Length() + 4 + iTrimFrom.Length() + 1 + 1);

	aStream.WriteInt32L(iRuleDescription.Length());
	aStream.WriteL(iRuleDescription);
	
	aStream.WriteInt8L(iInEncoding);
	aStream.WriteInt8L(iParseHTML);

	aStream.WriteInt32L(iTrimTo.Length());
	aStream.WriteL(iTrimTo);

	aStream.WriteInt32L(iTrimFrom.Length());
	aStream.WriteL(iTrimFrom);

	aStream.WriteInt8L(iConvertToTCR);
	
	aStream.WriteInt8L(iOutEncoding);
}

void CRulesetOptions::InternalizeL(RReadStream &aStream)
{
	TInt StreamSize = aStream.ReadInt32L();
	TInt Len;

	if ((Len = aStream.ReadInt32L()) != 0) {
		iRuleDescription.SetLength(Len);
		aStream.ReadL(iRuleDescription, Len);
	}
	if (!(StreamSize -= 4 + Len)) return;

	iInEncoding = (TEncodingType)aStream.ReadInt8L();
	if (!--StreamSize) return;
	iParseHTML = aStream.ReadInt8L();
	if (!--StreamSize) return;

	if ((Len = aStream.ReadInt32L()) != 0) {
		iTrimTo.SetLength(Len);
		aStream.ReadL(iTrimTo, Len);
	}
	if (!(StreamSize -= 4 + Len)) return;

	if ((Len = aStream.ReadInt32L()) != 0) {
		iTrimFrom.SetLength(Len);
		aStream.ReadL(iTrimFrom, Len);
	}
	if (!(StreamSize -= 4 + Len)) return;

	iConvertToTCR = aStream.ReadInt8L();
	if (!--StreamSize) return;

	iOutEncoding = (TEncodingType)aStream.ReadInt8L();
	if (--StreamSize) aStream.ReadL(StreamSize);
}

void CRulesetOptions::Reset()
{
	iInEncoding = ET_AUTO;
	iOutEncoding = ET_WIN;
	iParseHTML = iConvertToTCR = EFalse;
	iRuleDescription.SetLength(0);
	iTrimTo.SetLength(0);
	iTrimFrom.SetLength(0);
}
