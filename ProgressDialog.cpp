// ProgressDialog.cpp: implementation of the CProgressDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "ProgressDialog.h"
#include "eikdialg.hrh"
#include "TextConverter.hrh"
#include "eikprogi.h"
#include <e32math.h>
#include <e32def.h>
#include <eiklabel.h>
#include "TextConverter.rsg"
#include <eikenv.h>
#include <eikbtpan.h>
#include "ProgressBarActive.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgressDialog::CProgressDialog()
{
}

CProgressDialog::~CProgressDialog()
{

}

void CProgressDialog::SetLabelL(const TDesC &aText)
{
	CEikLabel *Label = static_cast <CEikLabel *> (Control(EProgressLabel));
    

	Label->SetTextL(aText);
	Label->DrawNow();
}	

void CProgressDialog::SetLabelResourceL(TInt aResourceId) {
	TBuf <64> Buf;
	STATIC_CAST(CEikonEnv*,iCoeEnv)->ReadResource(Buf, aResourceId);
	SetLabelL(Buf);

}

void CProgressDialog::SetLabelResourceIntIntL(TInt aResourceId, TInt Param1, TInt Param2) {
	TBuf <64> Template, Label;
	STATIC_CAST(CEikonEnv*,iCoeEnv)->ReadResource(Template, aResourceId);
	Label.Format(Template, Param1, Param2);
	SetLabelL(Label);
}

void CProgressDialog::SetValue(TReal aValue)
{
	
	CEikProgressInfo *ProgressInfo = static_cast <CEikProgressInfo *> (Control(EProgressBar));
	TInt16 Res;
	Math::Int(Res, aValue);
	ProgressInfo->SetAndDraw(Res);
  	
}

TBool CProgressDialog::OkToExitL(TInt aButtonCode)
{
	switch (aButtonCode) {
	case EEikBidOk:
		STATIC_CAST (CProgressBarActive *, iService) -> DoContinue();
		return EFalse;
	case EEikBidCancel:
		iService->Deque();
		delete iService;
	
		STATIC_CAST(CEikonEnv*,iCoeEnv)->InfoMsg(R_CONVERTION_CANCELLED);
		return ETrue;
	default:
		return EFalse;
	}


}

void CProgressDialog::DoExecuteLD(CActive *aService, TBool aDebugMode) {
	iDebugMode = aDebugMode;
	iService = aService;
	if (iDebugMode)
		ExecuteLD(R_PROGRESS_DEBUG_DIALOG);
	else
		ExecuteLD(R_PROGRESS_DIALOG);
}


void CProgressDialog::ProcessCompleteD()
{
	STATIC_CAST(CEikonEnv*,iCoeEnv)->InfoMsg(R_CONVERTION_COMPLETE);
	delete this;
}

//DEL void CProgressDialog::PreLayoutDynInitL()
//DEL {
//DEL 	if (!iDebugMode) {
//DEL 		MakePanelButtonVisible(EEikBidOk, EFalse);
//DEL 		ButtonPanel()->ResetMinimumSize();
//DEL 	}
//DEL }

//DEL void CProgressDialog::SetSizeAndPositionL(const TSize &aSize)
//DEL {
//DEL 	TInt i = 0;
//DEL 	//CEikDialog::SetSizeAndPositionL(aSize);
//DEL }
