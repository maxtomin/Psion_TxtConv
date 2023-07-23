// ConverterActive.h: interface for the CConverterActive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTERACTIVE_H__E9C470E8_92ED_4D4E_87E6_5C96FA86F3EE__INCLUDED_)
#define AFX_CONVERTERACTIVE_H__E9C470E8_92ED_4D4E_87E6_5C96FA86F3EE__INCLUDED_

#include "ProgressBarActive.h"
#include "RulesetOptions.h"
#include "ReplaceRule.h"
#include "TextConversion.h"

class CConverterActive : public CProgressBarActive  
{
private:
	TBool iDebugMode;
	CTextConversion *iTextConversion;
	TInt iFilePos, iFileSize, iStepCount, iMaxStepCount;
	TInt iRuleNumber, iCurPass;
	TInt iTextLocation;	//-1 - InFile
						// 0 - TempFile
						// 1 - OutFile
	TEncodingType iInEncodingType, iOutEncodingType;
	void SwapFiles();
	RFile iInFile, iOutFile, iTempFile; 
	TFileName iTempName, iOutName, iDebugName;
	RFs iFs;

	CRulesetOptions *iRulesetOptions;
	CReplaceRules *iReplaceRules;

	RFile *iCurSource, *iCurDest;

	enum TConvertState {cnsParseHtml, cnsGetEncoding, cnsSetEncoding, cnsTrimFromTo, cnsApplyRules, cnsSetOutEncoding, cnsConvertToTcr, cnsCopyingToOut, cnsFinished} iConvertState;
	enum TInternalState {insInit, insMainLoop, insFinalize} iInternalState;
public:
	static TBool AskForRetry(TDesC &aFileName);
	void ConstructL(TDesC &aInFileName, TDesC &aOutFileName, TDesC &aTempFileName, TDesC &aDebugFileName);
	CConverterActive(CProgressDialog *aProgDlg,
					CRulesetOptions *aRulesetOptions, 
					CReplaceRules *aReplaceRules);
	virtual ~CConverterActive();

protected:
	virtual void DoStepL();
};

#endif // !defined(AFX_CONVERTERACTIVE_H__E9C470E8_92ED_4D4E_87E6_5C96FA86F3EE__INCLUDED_)
