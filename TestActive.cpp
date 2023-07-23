// TestActive.cpp: implementation of the CTestActive class.
//
//////////////////////////////////////////////////////////////////////

#include "TestActive.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestActive::CTestActive(CProgressDialog *aProgDlg): 
	CActive(EPriorityUserInput), 
	iProgDlg(aProgDlg)
{
	iState = 0;
	CActiveScheduler::Add(this);
	SetActive();
	TRequestStatus* status = &iStatus ;
	User::RequestComplete(status, KErrNone) ;
}

CTestActive::~CTestActive()
{

}

void CTestActive::RunL()
{
	iCompleted = EFalse;
	TRAPD(err, DoStepL());
	if (err) {
		delete iProgDlg;
		DequeAndDelete();
		User::Leave(err);
	}
	
	if (iCompleted) {
		delete iProgDlg;
		DequeAndDelete();
	}
	else {
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
	}
}

void CTestActive::DoCancel()
{
}

void CTestActive::DoStepL()
{
	for (TInt i = 0; i < 1000000; i++) ;
	++iState;
	iProgDlg->SetValue(iState/100);
	
	iCompleted = (iState > 1000);
}

void CTestActive::DequeAndDelete()
{
		Deque();
		delete this;
}
