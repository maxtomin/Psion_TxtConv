// RuleEditDialog.h: interface for the CRuleEditDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RULEEDITDIALOG_H__715F64E5_758D_44DB_8A28_F5898F67D045__INCLUDED_)
#define AFX_RULEEDITDIALOG_H__715F64E5_758D_44DB_8A28_F5898F67D045__INCLUDED_

#include "ReplaceRule.h"
#include <eikdial.h>

class CRuleEditDialog : public CEikDialog  
{
private:
	CReplaceRule* iRule;
public:
	TBool OkToExitL(TInt aButtonId);
	CRuleEditDialog(CReplaceRule* aRule);
	virtual ~CRuleEditDialog();

protected:
	void PostLayoutDynInitL();
};

#endif // !defined(AFX_RULEEDITDIALOG_H__715F64E5_758D_44DB_8A28_F5898F67D045__INCLUDED_)
