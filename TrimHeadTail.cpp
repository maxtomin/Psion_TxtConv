// TrimHeadTail.cpp: implementation of the CTrimHeadTail class.
//
//////////////////////////////////////////////////////////////////////

#include "TrimHeadTail.h"
#include "ApplyRule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrimHeadTail::CTrimHeadTail(TDesC &aTrimTo, TDesC &aTrimFrom) :
	CTextConversion(),
	iTrimToOriginal(aTrimTo),
	iTrimFromOriginal(aTrimFrom)
{
}

CTrimHeadTail::~CTrimHeadTail()
{

}

void CTrimHeadTail::ConstructL(RFile *aSource, RFile *aDest) {
	CTextConversion::ConstructL(aSource, aDest);
	CApplyRule::NormalizeStringL(iTrimTo, iTrimToOriginal);
	CApplyRule::NormalizeStringL(iTrimFrom, iTrimFromOriginal);
	iTrimmingTail = (iTrimTo.Length() == 0);
}

TBool CTrimHeadTail::DoStepL(TInt &aFilePos)
{
	//aTrimTo - head
	//aTrimFrom - tail
	if (!iFileReader.ReadChar(iChar)) 
		return ETrue;
	++aFilePos;

	if (iTrimmingTail) {
		if ((iTrimFrom.Length() > 0) && iTrimFrom[0] == iChar)  {
			iFileReader.StorePos();
			OldChar = iChar;
			CurDesPos = 1;
			while (CurDesPos < iTrimFrom.Length() && 
				iFileReader.ReadChar(iChar) && 
				iTrimFrom[CurDesPos] == iChar) CurDesPos++;
			if (CurDesPos == iTrimFrom.Length()) {
				aFilePos += CurDesPos - 1;
				return ETrue;
			}
			else {
				iFileReader.RestorePos();
				iFileWriter.WriteCharL(OldChar);
			}
		}
		else iFileWriter.WriteCharL(iChar);
	}
	else
		if (iTrimTo[0] == iChar)  {
			iFileReader.StorePos();
			OldChar = iChar;
			CurDesPos = 1;
			while (CurDesPos < iTrimTo.Length() && 
				iFileReader.ReadChar(iChar) && 
				iTrimTo[CurDesPos] == iChar) CurDesPos++;
			if (CurDesPos == iTrimTo.Length()) {
				aFilePos += CurDesPos - 1;
				iFileWriter.ResetOutputL();
				iTrimmingTail = !iTrimmingTail;
			}
			else {
				iFileReader.RestorePos();
				iFileWriter.WriteCharL(OldChar);
			}
		}
		else iFileWriter.WriteCharL(iChar);
/*		if (iTrimTo[0] == iChar)  {
			iFileReader.StorePos();
			CurDesPos = 1;
			while (CurDesPos < iTrimTo.Length() && 
				iFileReader.ReadChar(iChar) && 
				iTrimTo[CurDesPos] == iChar) CurDesPos++;
			if (CurDesPos == iTrimTo.Length()) { 
				aFilePos += CurDesPos - 1;
				iTrimmingTail = !iTrimmingTail;
			}
			else iFileReader.RestorePos();
		}*/
	return EFalse;
}
