// RulesetOptions.h: interface for the CRulesetOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RULESETOPTIONS_H__F49C8170_2777_4D3D_BEDE_32BFE5D0DCCC__INCLUDED_)
#define AFX_RULESETOPTIONS_H__F49C8170_2777_4D3D_BEDE_32BFE5D0DCCC__INCLUDED_

#include <e32std.h>
#include <e32base.h>
#include <s32strm.h>
#define RULESET_DESCRIPTION_LENGTH 128
#define TRIM_LENGTH 64

enum TEncodingType {ET_AUTO, ET_WIN, ET_KOI, ET_DOS, ET_UTF8};
#define FIND_REPLACE_LENGTH 64

class CRulesetOptions  
{
public:
	void Reset();
	void InternalizeL(RReadStream &aStream);
	void ExternalizeL(RWriteStream &aStream);
	TBuf <RULESET_DESCRIPTION_LENGTH> iRuleDescription;
	TEncodingType iInEncoding, iOutEncoding;
	TBuf <TRIM_LENGTH> iTrimFrom, iTrimTo;
	TBool iParseHTML, iConvertToTCR;
	CRulesetOptions();
	virtual ~CRulesetOptions();

};

#endif // !defined(AFX_RULESETOPTIONS_H__F49C8170_2777_4D3D_BEDE_32BFE5D0DCCC__INCLUDED_)
