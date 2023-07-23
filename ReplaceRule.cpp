// ReplaceRule.cpp: implementation of the CReplaceRule class.
//
//////////////////////////////////////////////////////////////////////

#include "ReplaceRule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReplaceRule::CReplaceRule() : iMatchWholeWord(EFalse), iCaseSensitive(EFalse), iDoWhilePossible(EFalse) {
	
}

CReplaceRule::CReplaceRule(TPtrC aRuleName, TPtrC aFindStr, TPtrC aReplaceStr) :
	iRuleName(aRuleName),
	iFindStr(aFindStr),
	iReplaceStr(aReplaceStr)
{
		CReplaceRule();
}

CReplaceRule::~CReplaceRule()
{

}

/*
Stream structure (version 1):
Size	Value
1		reserved, =0
4		Length of the following stream
4		iRuleName length
X		iRuleName
4		iFindStr length
X		iFindStr
4		iReplaceStr length
X		iReplaceStr
1		iCaseSensitive
1		iMatchWholeWord
1		iDoWhilePossible
*/
void CReplaceRule::ExternalizeL(RWriteStream &aStream)
{
	aStream.WriteInt8L(0);
	aStream.WriteInt32L(4 + iRuleName.Length() + 4 + iFindStr.Length() + 4 + iReplaceStr.Length() + 1 + 1 + 1);

	aStream.WriteInt32L(iRuleName.Length());
	aStream.WriteL(iRuleName);
	aStream.WriteInt32L(iFindStr.Length());
	aStream.WriteL(iFindStr);
	aStream.WriteInt32L(iReplaceStr.Length());
	aStream.WriteL(iReplaceStr);
	aStream.WriteInt8L(iCaseSensitive);
	aStream.WriteInt8L(iMatchWholeWord);
	aStream.WriteInt8L(iDoWhilePossible);
}

void CReplaceRule::InternalizeL(RReadStream &aStream)
{
	TInt Len = aStream.ReadInt8L();
	TInt aStreamSize = aStream.ReadInt32L();

	if (Len == 0) {
		if ((Len = aStream.ReadInt32L()) != 0) {
			iRuleName.SetLength(Len);
			aStream.ReadL(iRuleName, Len);
		}
		if (!(aStreamSize -= 4 + Len)) return;

		if ((Len = aStream.ReadInt32L()) != 0) {
			iFindStr.SetLength(Len);
			aStream.ReadL(iFindStr, Len);
		}
		if (!(aStreamSize -= 4 + Len)) return;
		
		if ((Len = aStream.ReadInt32L()) != 0) {
			iReplaceStr.SetLength(Len);
			aStream.ReadL(iReplaceStr, Len);
		}
		if (!(aStreamSize -= 4 + Len)) return;
		
		iCaseSensitive  = aStream.ReadInt8L();
		if (!--aStreamSize) return;
		iMatchWholeWord = aStream.ReadInt8L();
		if (!--aStreamSize) return;
		iDoWhilePossible = aStream.ReadInt8L();
		--aStreamSize;
	}
	if (aStreamSize) aStream.ReadL(aStreamSize); //discard remains
}
