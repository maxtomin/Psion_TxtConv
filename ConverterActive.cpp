// ConverterActive.cpp: implementation of the CConverterActive class.
//
//////////////////////////////////////////////////////////////////////


#include <eikinfo.h>
#include <eikon.rsg>
#include "ConverterActive.h"
#include "ParseHtmlConvertion.h"
#include "CopyToOutConvertion.h"
#include "GetEncoding.h"
#include "TrimHeadTail.h"
#include "ApplyRule.h"
#include "CompressTcr.h"
#include "TextConverter.rsg"
#include "Utf2Win.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConverterActive::CConverterActive(
					CProgressDialog *aProgDlg,
					CRulesetOptions *aRulesetOptions, 
					CReplaceRules *aReplaceRules):
	CProgressBarActive(aProgDlg),
	iRulesetOptions(aRulesetOptions),
	iReplaceRules(aReplaceRules)
{
	iTextConversion = NULL;
	iCurPass = iRuleNumber = 0;

	iTextLocation = -1;
	iConvertState = cnsParseHtml;
	iInternalState = insInit;
}

CConverterActive::~CConverterActive()
{
	if (iTextConversion) delete iTextConversion;

	iInFile.Close(); 
	iOutFile.Close();
	iTempFile.SetSize(0);
	iTempFile.Close();

	if (!iCompleted)
		iFs.Delete(iOutName);
}

/*_LIT(KParsingHtml, "Parsing html...");
_LIT(KCopyToOut, "Copying to output file...");
_LIT(KGetEncoding, "Revealing encoding type...");
_LIT(KSetEncoding, "Changing encoding...");
_LIT(KTrim, "Trimming text...");
_LIT(KApplyingRuleTemplate, "Applying rule %d, pass %d...");
_LIT(KCompressingTcr, "TCR compressing...");*/


void CConverterActive::DoStepL()
{
	switch (iInternalState) {
		case insInit:
			switch (iConvertState) {
			case cnsParseHtml:
				if (!iRulesetOptions->iParseHTML) {
					((TInt &)iConvertState)++;
					return;
				}
				iProgDlg->SetLabelResourceL(R_PARSING_HTML);
				iTextConversion = new CParseHtmlConvertion();
				break;
			case cnsGetEncoding:
				if (iRulesetOptions->iInEncoding != ET_AUTO) {
					iInEncodingType = iRulesetOptions->iInEncoding;
					((TInt &)iConvertState)++;
					return;
				}
				iProgDlg->SetLabelResourceL(R_GET_ENCODING);
				iTextConversion = new CGetEncoding();
				break;
			case cnsSetEncoding:
				switch (iInEncodingType) {
				case ET_WIN:
					((TInt &)iConvertState)++;
					return;
				case ET_UTF8:
					iProgDlg->SetLabelResourceL(R_SET_ENCODING);
					iTextConversion = new CUtf2Win();
					break;
				default:
					iProgDlg->SetLabelResourceL(R_SET_ENCODING);
					iTextConversion = new CSetEncoding(iInEncodingType);
				}
				break;
			case cnsTrimFromTo:
				if (!iRulesetOptions->iTrimTo.Length() && !iRulesetOptions->iTrimFrom.Length()) {
					((TInt &)iConvertState)++;
					return;
				}
				iProgDlg->SetLabelResourceL(R_TRIM_TEXT);
				iTextConversion = new CTrimHeadTail(iRulesetOptions->iTrimTo, iRulesetOptions->iTrimFrom);
				break;
			case cnsApplyRules: 
				{
					if (!iReplaceRules->Count()) {
						((TInt &)iConvertState)++;
						return;
					}
			
					iProgDlg->SetLabelResourceIntIntL(R_APPLYING_RULE_TEMPLATE, iRuleNumber + 1, iCurPass + 1);
					CApplyRule *iApplyRule;
					iTextConversion = iApplyRule = new CApplyRule((*iReplaceRules)[iRuleNumber]);
					break;
				}
			case cnsSetOutEncoding:
				iOutEncodingType = (iRulesetOptions->iOutEncoding == ET_AUTO) ? iInEncodingType : iRulesetOptions->iOutEncoding;

				switch (iOutEncodingType) {

				case ET_WIN:
					((TInt &)iConvertState)++;
					return;
				case ET_UTF8:
					iProgDlg->SetLabelResourceL(R_SET_OUT_ENCODING);
					iTextConversion = new CWin2Utf();
					break;
				default:
					iProgDlg->SetLabelResourceL(R_SET_OUT_ENCODING);
					iTextConversion = new CSetOutEncoding(iOutEncodingType);
				}

				break;
			case cnsConvertToTcr:
				if (!iRulesetOptions->iConvertToTCR) {
					((TInt &)iConvertState)++;
					return;
				}
				iProgDlg->SetLabelResourceL(R_COMPRESSING_TCR);
				iTextConversion = new CCompressTcr(&iOutFile);
				break;
			case cnsCopyingToOut:
				switch (iTextLocation) {
				case -1:
					//create output file:
					iOutFile.Replace(iFs, iOutName, EFileRead |  EFileWrite);

					iCurDest = &iOutFile;		//copy in->out
				case 0: 
					break;						//copy temp->out
				case 1:
					((TInt &)iConvertState)++;	//do nothing
					return;
				}

				iProgDlg->SetLabelResourceL(R_COPYING_TO_OUT);
				iTextConversion = new CCopyToOutConvertion();
				break;
			default: 
				iCompleted = ETrue;
				return;
			}

			iProgDlg->SetValue(0);
			iFilePos = 0;
			User::LeaveIfError(iCurSource->Size(iFileSize));
			iMaxStepCount = iFileSize / 100 + 1;
			iTextConversion->ConstructL(iCurSource, iCurDest);
			iInternalState = insMainLoop;
			iPaused = iDebugMode;

			return;
		case insMainLoop:
			for (iStepCount = 0; iStepCount < iMaxStepCount; iStepCount++) 
				if (iTextConversion->DoStepL(iFilePos)) {
					iInternalState = insFinalize;
					break;
				}
			if (iFileSize)
				iProgDlg->SetValue(iFilePos * 100 / iFileSize);
			return;
		case insFinalize: 
			{
				TBool Swapped = EFalse;
				switch (iConvertState) {
				case cnsApplyRules:
					if (iTextConversion->iResult && (*iReplaceRules)[iRuleNumber].iDoWhilePossible) {
						++iCurPass;
						--((TInt &)iConvertState); //repeat last step
					}
					else {
						iCurPass = 0;
						++iRuleNumber;
						if (iRuleNumber != iReplaceRules->Count())
							--((TInt &)iConvertState); //repeat last step
					}
				case cnsParseHtml:
				case cnsSetEncoding:
				case cnsSetOutEncoding:
				case cnsTrimFromTo:
				case cnsConvertToTcr:
					Swapped = ETrue;
					SwapFiles();
				case cnsCopyingToOut:
					break;
				case cnsGetEncoding:
					iInEncodingType = TEncodingType(iTextConversion->iResult);
					break;
				case cnsFinished: ;
				}

				iTextConversion->DestructL();
				delete iTextConversion;
				iTextConversion = NULL;
				
				if (iConvertState == cnsConvertToTcr)
					//do not need swap files after converting:
					iConvertState = cnsFinished;
				else
					++((TInt &)iConvertState);
				
				iCompleted = (iConvertState == cnsFinished);
		
				iInternalState = insInit;

				if (iDebugMode && Swapped && !iCompleted) {
					CFileMan *FileMan = CFileMan::NewL(iFs);
					TInt err;
					CleanupStack::PushL(FileMan);
					if (iTextLocation) {
						//text in output file:
						iOutFile.Close();
						do {
							err = FileMan->Copy(iOutName, iDebugName);
						} while (err && AskForRetry(iDebugName));

						User::LeaveIfError(iOutFile.Open(iFs, iOutName, EFileRead |  EFileWrite));
					}
					else {
						//text in temp file:
						iTempFile.Close();
						do {
							err = FileMan->Copy(iTempName, iDebugName);
						} while (err && AskForRetry(iDebugName));
						User::LeaveIfError(iTempFile.Open(iFs, iTempName, EFileRead |  EFileWrite));
					}
					CleanupStack::PopAndDestroy(); //FileMan

				}
			}
	}
	return;

	
	
/*	if (aRulesetOptions->iParseHTML) {
		ParseHtmlL();
		SwapFiles();
	}

	if (aRulesetOptions->iEncodingType != ET_WIN) {
		ChangeEncodingL(GetEncoding(aRulesetOptions->iEncodingType));
		SwapFiles();
	}

	if (aRulesetOptions->iTrimFrom.Length() || aRulesetOptions->iTrimTo.Length()) {
		TrimText(aRulesetOptions->iTrimFrom, aRulesetOptions->iTrimTo);
		SwapFiles();
	}

	for (TInt i = 0; i < aReplaceRules->Count(); i++) 
	{
		ApplyRuleL((*aReplaceRules)[i]);
		SwapFiles();
	}

	if (aRulesetOptions->iConvertToTCR) {
		ConvertToTCRL();
		SwapFiles();
	}

	if (iDestIsOut) CopyTempToOutL();*/
}

void CConverterActive::SwapFiles()
{
	if (iCurSource == &iInFile) {
		iInFile.Close();
//		iOutFile.Replace(iFs, iOutName, EFileRead |  EFileWrite);
		iCurSource = &iTempFile;
		iCurDest = &iOutFile;
	}
	else
	{
		RFile *R = iCurSource;
		iCurSource = iCurDest;
		iCurDest = R; 
	}
	iTextLocation = !iTextLocation; //-1 -> 0, 0 -> 1, 1 -> 0
}

void CConverterActive::ConstructL(TDesC &aInFileName, TDesC &aOutFileName, TDesC &aTempFileName, TDesC &aDebugFileName)
{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iInFile.Open(iFs, aInFileName, EFileRead |  EFileShareReadersOnly));
	User::LeaveIfError(iTempFile.Open(iFs, aTempFileName, EFileRead | EFileWrite));
	User::LeaveIfError(iOutFile.Open(iFs, aOutFileName, EFileRead | EFileWrite));
	iTempName = aTempFileName;
	iOutName = aOutFileName;
	iDebugName = aDebugFileName;
	iDebugMode = iDebugName.Length();

	iCurSource = &iInFile;
	iCurDest = &iTempFile;
}

_LIT(KRetryTitle, "Error");
_LIT(KRetryMessage, "File %S is in use, retry?");
TBool CConverterActive::AskForRetry(TDesC &aFileName)
{
	TBuf <512> buf;
	buf.Format(KRetryMessage, &aFileName);
	CEikInfoDialog *dlg = new CEikInfoDialog(KRetryTitle, buf);
	return dlg->ExecuteLD(R_EIK_DIALOG_QUERY);
}
