// TestActive.cpp: implementation of the CProgressBarActive class.
//
//////////////////////////////////////////////////////////////////////

#include "ProgressBarActive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgressBarActive::CProgressBarActive(CProgressDialog *aProgDlg): 
	CActive(EPriorityUserInput), 
	iProgDlg(aProgDlg)
{
	iState = 0;
	CActiveScheduler::Add(this);
	iPaused = EFalse;
	SetActive();
	TRequestStatus* status = &iStatus ;
	User::RequestComplete(status, KErrNone) ;
}

CProgressBarActive::~CProgressBarActive()
{

}

void CProgressBarActive::RunL()
{
	iCompleted = EFalse;
	TRAPD(err, DoStepL());
	if (err) {
		delete iProgDlg;
		Deque();
		delete this;
		User::Leave(err);
	}
	
	if (iCompleted) {
		iProgDlg->ProcessCompleteD();
		Deque();
		delete this;
	}
	else {
		if (!iPaused) {
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
		}
	}
}

void CProgressBarActive::DoCancel()
{
}


void CProgressBarActive::DoContinue()
{
	if (iPaused) {
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		iPaused = EFalse;
	}
}
