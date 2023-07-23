// RulesetOptionsDialog.h: interface for the CRulesetOptionsDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RULESETOPTIONSDIALOG_H__F5A83246_59F1_41C7_AF32_0AFB8D4A43C2__INCLUDED_)
#define AFX_RULESETOPTIONSDIALOG_H__F5A83246_59F1_41C7_AF32_0AFB8D4A43C2__INCLUDED_

#include "RulesetOptions.h"
#include "Eikdial.h"


class CRulesetOptionsDialog : public CEikDialog  
{
private:
	CRulesetOptions *iRulesetOptions;
public:
	CRulesetOptionsDialog(CRulesetOptions *aRulesetOptions);
	virtual ~CRulesetOptionsDialog();

protected:
	TBool OkToExitL(TInt);
	void PostLayoutDynInitL();
};

#endif // !defined(AFX_RULESETOPTIONSDIALOG_H__F5A83246_59F1_41C7_AF32_0AFB8D4A43C2__INCLUDED_)
