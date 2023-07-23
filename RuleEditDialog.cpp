// RuleEditDialog.cpp: implementation of the CRuleEditDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "RuleEditDialog.h"
#include "TextConverter.hrh"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRuleEditDialog::CRuleEditDialog(CReplaceRule* aRule) : iRule(aRule)
{

}

CRuleEditDialog::~CRuleEditDialog()
{

}

void CRuleEditDialog::PostLayoutDynInitL()
{
	SetEdwinTextL(EEdwinRuleName, &iRule->iRuleName);
	SetEdwinTextL(EEdwinSearchFor, &iRule->iFindStr);
	SetEdwinTextL(EEdwinReplaceWith, &iRule->iReplaceStr);
	if (iRule->iMatchWholeWord)
		SetCheckBoxState(ECheckBoxMatchWholeWord, CEikButtonBase::ESet);
	if (iRule->iCaseSensitive)
		SetCheckBoxState(ECheckBoxCaseSensitive, CEikButtonBase::ESet);
	if (iRule->iDoWhilePossible)
		SetCheckBoxState(ECheckBoxDoWhilePossible, CEikButtonBase::ESet);
}

TBool CRuleEditDialog::OkToExitL(TInt)
{
	GetEdwinText(iRule->iRuleName, EEdwinRuleName);
	GetEdwinText(iRule->iFindStr, EEdwinSearchFor);
	GetEdwinText(iRule->iReplaceStr, EEdwinReplaceWith);
	iRule->iMatchWholeWord = CheckBoxState(ECheckBoxMatchWholeWord) == CEikButtonBase::ESet;
	iRule->iCaseSensitive = CheckBoxState(ECheckBoxCaseSensitive) == CEikButtonBase::ESet;
	iRule->iDoWhilePossible = CheckBoxState(ECheckBoxDoWhilePossible) == CEikButtonBase::ESet;
	return ETrue;
}
