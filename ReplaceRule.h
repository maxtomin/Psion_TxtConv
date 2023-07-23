// ReplaceRule.h: interface for the CReplaceRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPLACERULE_H__E92E2E6F_438E_40C6_8BAC_25A96AA5AD79__INCLUDED_)
#define AFX_REPLACERULE_H__E92E2E6F_438E_40C6_8BAC_25A96AA5AD79__INCLUDED_

#include <e32std.h>
#include <e32base.h>
#include <s32strm.h>

#define CReplaceRules CArrayVarFlat<CReplaceRule> 
#define RULE_NAME_LENGTH 64
#define FIND_REPLACE_LENGTH 64

class CReplaceRule  
{
public:
	void InternalizeL(RReadStream &aStream);
	void ExternalizeL(RWriteStream &aStream);
	TBool iMatchWholeWord, iCaseSensitive, iDoWhilePossible;
	TBuf <RULE_NAME_LENGTH> iRuleName;
	TBuf <FIND_REPLACE_LENGTH> iFindStr, iReplaceStr;
	
	
	CReplaceRule();
	CReplaceRule(TPtrC iRuleName, TPtrC iFindStr, TPtrC iReplaceStr);
	virtual ~CReplaceRule();

};

#endif // !defined(AFX_REPLACERULE_H__E92E2E6F_438E_40C6_8BAC_25A96AA5AD79__INCLUDED_)
