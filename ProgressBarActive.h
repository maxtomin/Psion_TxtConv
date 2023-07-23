// TestActive.h: interface for the CTestActive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_)
#define AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_

#include <e32base.h>
#include "ProgressDialog.h"

class CProgressBarActive : public CActive  
{
//CActive support methods:

protected:
	CProgressDialog *iProgDlg;
	TBool iCompleted, iPaused;
	virtual void DoCancel();
	virtual void RunL();
	virtual void DoStepL() = 0;

public:
	void DoContinue();
	void FinishNormal();
	void FinishFailure();
	CProgressBarActive(CProgressDialog *aProgDlg);
	virtual ~CProgressBarActive();

//service methods:
private: 
	TInt iState;
};

#endif // !defined(AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_)
