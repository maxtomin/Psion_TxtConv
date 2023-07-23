// Converter.h: interface for the CConverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTER_H__06FC7293_E0EB_4812_A5D8_8FA7A862CB12__INCLUDED_)
#define AFX_CONVERTER_H__06FC7293_E0EB_4812_A5D8_8FA7A862CB12__INCLUDED_

#include <f32file.h>
#include <s32file.h>
#include "RulesetOptions.h"
#include "ReplaceRule.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "ProgressDialog.h"
#include "coecobs.h"

class CConverter
{
private: //data
	enum TConvertStage {csParseHtml, csChangeEncoding, csTrim, csApplyRule, csConvertToTcr, csCopyToOut} iConvertStage;

	RFile *iInFile, *iOutFile, *iTempFile; //owns
	RFile *iCurSource, *iCurDest;
	TBool iDestIsOut;
	TBool iUserCanceled;

	CFileReader iFileReader;
	CFileWriter iFileWriter;

private:
	void NormalizeStringL(TDes &aResult, TDesC &aSource);
	TUint8 LowerCase(TUint8 Char);
	TBool IsVowel(TUint8 Char);
	TBool IsSingleWord(TUint8 Char);
	TBool IsSpace(TUint8 Char);
protected:
	void ReleaseStreams();
	void InitStreamsL();
	void ConvertToTCRL();
	void CopyTempToOutL();
	void ApplyRuleL(CReplaceRule &aReplaceRule);
	void TrimText(TDesC &aTrimFrom, TDesC &aTrimTo);
	TEncodingType GetEncoding(TEncodingType aEncodingType);
	void ChangeEncodingL(TEncodingType aEncodingType);
	void ParseHtmlL();
	void SwapFiles();
public:

	CConverter(
		RFile *aInFile, 
		RFile *aOutFile, 
		RFile *aTempFile
		);
	virtual ~CConverter();

	void DoConvertL(CRulesetOptions *aRulesetOptions, CReplaceRules *aReplaceRules);

};

#endif // !defined(AFX_CONVERTER_H__06FC7293_E0EB_4812_A5D8_8FA7A862CB12__INCLUDED_)
