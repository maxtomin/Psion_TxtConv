// ProgressDialog.h: interface for the CProgressDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESSDIALOG_H__01D541A5_45E9_4BFC_827D_4BCBF06B725B__INCLUDED_)
#define AFX_PROGRESSDIALOG_H__01D541A5_45E9_4BFC_827D_4BCBF06B725B__INCLUDED_

#include <eikdial.h>

class CProgressDialog : public CEikDialog  
{
private:
	CActive *iService;
	TBool iDebugMode;

public:
	void ProcessCompleteD();
	void SetValue(TReal aValue);
	void SetLabelL(const TDesC &aText);
	void SetLabelResourceL(TInt aResourceId);
	void SetLabelResourceIntIntL(TInt aResourceId, TInt Param1, TInt Param2);
	void DoExecuteLD(CActive *aService, TBool aDebugMode);
	CProgressDialog();
	virtual ~CProgressDialog();
protected:
	virtual TBool OkToExitL(TInt aButtonId);
};

#endif // !defined(AFX_PROGRESSDIALOG_H__01D541A5_45E9_4BFC_827D_4BCBF06B725B__INCLUDED_)
