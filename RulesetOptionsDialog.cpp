// RulesetOptionsDialog.cpp: implementation of the CRulesetOptionsDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "RulesetOptionsDialog.h"
#include "TextConverter.hrh"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRulesetOptionsDialog::CRulesetOptionsDialog(CRulesetOptions *aRulesetOptions) : iRulesetOptions(aRulesetOptions)
{

}

CRulesetOptionsDialog::~CRulesetOptionsDialog()
{

}

void CRulesetOptionsDialog::PostLayoutDynInitL()
{
	SetEdwinTextL(EEdwinRulesetDescription, &iRulesetOptions->iRuleDescription);
	SetChoiceListCurrentItem(EChoiceListInEncoding, iRulesetOptions->iInEncoding);
	SetChoiceListCurrentItem(EChoiceListOutEncoding, iRulesetOptions->iOutEncoding);
	SetEdwinTextL(EEdwinTrimTo, &iRulesetOptions->iTrimTo);
	SetEdwinTextL(EEdwinTrimFrom, &iRulesetOptions->iTrimFrom);
	if (iRulesetOptions->iParseHTML)
		SetCheckBoxState(ECheckBoxParseHTML, CEikButtonBase::ESet);
	if (iRulesetOptions->iConvertToTCR)
		SetCheckBoxState(ECheckBoxConvertToTCR, CEikButtonBase::ESet);
}

TBool CRulesetOptionsDialog::OkToExitL(TInt)
{
	GetEdwinText(iRulesetOptions->iRuleDescription, EEdwinRulesetDescription);
	iRulesetOptions->iInEncoding = (TEncodingType)ChoiceListCurrentItem(EChoiceListInEncoding);
	iRulesetOptions->iOutEncoding = (TEncodingType)ChoiceListCurrentItem(EChoiceListOutEncoding);
	GetEdwinText(iRulesetOptions->iTrimTo, EEdwinTrimTo);
	GetEdwinText(iRulesetOptions->iTrimFrom, EEdwinTrimFrom);
	iRulesetOptions->iParseHTML = CheckBoxState(ECheckBoxParseHTML) == CEikButtonBase::ESet; 
	iRulesetOptions->iConvertToTCR = CheckBoxState(ECheckBoxConvertToTCR) == CEikButtonBase::ESet; 
	return ETrue;
}
