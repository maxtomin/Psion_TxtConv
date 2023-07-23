// TestActive.h: interface for the CTestActive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_)
#define AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_

#include <e32base.h>
#include "ProgressDialog.h"

class CTestActive : public CActive  
{
//CActive support methods:
private: 
	void DequeAndDelete();
	TBool iCompleted;
	CProgressDialog *iProgDlg;

protected:
	virtual void DoCancel();
	virtual void RunL();

public:
	void DoStepL();
	CTestActive(CProgressDialog *aProgDlg);
	virtual ~CTestActive();

//service methods:
private: 
	TInt iState;
};

#endif // !defined(AFX_TESTACTIVE_H__C16C1CFE_B8D7_4846_BAB2_435BD817E02D__INCLUDED_)
