// TrimHeadTail.h: interface for the CTrimHeadTail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIMHEADTAIL_H__E98FD2CE_8287_421B_936D_254AB780E95D__INCLUDED_)
#define AFX_TRIMHEADTAIL_H__E98FD2CE_8287_421B_936D_254AB780E95D__INCLUDED_

#include "TextConversion.h"
#include "RulesetOptions.h"

class CTrimHeadTail : public CTextConversion  
{
private:
	TInt CurDesPos;
	TBool iTrimmingTail;
	TUint8 OldChar;
	TDesC &iTrimToOriginal, &iTrimFromOriginal;
	TBuf <TRIM_LENGTH> iTrimTo, iTrimFrom;
public:
	virtual void ConstructL(RFile *aSource, RFile *aDest);
	CTrimHeadTail(TDesC &aTrimTo, TDesC &aTrimFrom);
	virtual ~CTrimHeadTail();

protected:
	virtual TBool DoStepL(TInt &aFilePos);
};

#endif // !defined(AFX_TRIMHEADTAIL_H__E98FD2CE_8287_421B_936D_254AB780E95D__INCLUDED_)
