#include <eikapp.h>
#include <eikdoc.h>
#include <eikappui.h>
#include <coecntrl.h>
#include <coeccntx.h>
#include <eiktxlbx.h>

#include "ReplaceRule.h"
#include "RulesetOptions.h"



#ifdef _UNICODE
const TUid KUidTextConverter = { 0x101F7999 };
#else
const TUid KUidTextConverter = { 0x101F7998 };
#endif


class CTextConverterDoc;
class CTextConverterAppUi;

class CTextConverterAppView: public CCoeControl, public MCoeControlBrushContext {
private:
	CTextConverterAppUi *iAppUi;
	CEikTextListBox *iListBox;
	CTextConverterDoc *iDocument;
protected:
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

public:
	void HandleMoveRuleUpL();
	void HandleMoveRuleDownL();
	void HandleItemRemovalL();
	void HandleItemAdditionL();
	CReplaceRule * GetSelectedRule();
	TInt GetSelectedRuleIndex();
	void ConstructL(const TRect& aRect, CTextConverterAppUi *aAppUi, CTextConverterDoc *aDocument);
	TInt CountComponentControls() const;
	CCoeControl * ComponentControl(TInt aIndex) const;
	~CTextConverterAppView();
};

class CTextConverterAppUi: public CEikAppUi, public MEikListBoxObserver {
private:
	TFileName CurrentFilePath()const;
	void SetInitialPathL(TFileName& aFileName) const;
	CReplaceRule TestRule;
	CTextConverterAppView *iAppView;
public: //from MEikListBoxObserver

public:
	void StartConvertFileL(TDesC &aInFileName, TDesC &aOutFileName, TDesC &aDebugFileName);
	void OpenFileL(const TDesC& aFileName);
	void CreateFileL(const TDesC& aFileName);
	void HandleModelChangeL();
	void ConstructL();
	void HandleCommandL(TInt aCommand);
	~CTextConverterAppUi();
protected:
	virtual void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
	void CmdConvertTextL(TBool aDebugMode);
	TBool ProcessCommandParametersL(TApaCommand aCommand,TFileName& aDocumentName,const TDesC& /*aTail*/);
	void CmdFileNewL();
	void CmdFileOpenL();
	void CmdFileSaveL();
	void CmdFileSaveAsL();
	void CmdFileRevertL();
};

class CTextConverterDoc : public CEikDocument, public MDesCArray {
private: //data
	CReplaceRules iReplaceRules;
	CRulesetOptions iRulesetOptions;
public: //serialization:
	void NewDocumentL();
	void StoreL(CStreamStore& aStore, CStreamDictionary& aStreamDic) const;
	void RestoreL(const CStreamStore& aStore, const CStreamDictionary& aStreamDic);
	void InternalizeL(RStoreReadStream &stream);
	void ExternalizeL(RWriteStream& aStream) const;
public:
	TBool MoveRuleUp(TInt aIndex);
	TBool MoveRuleDown(TInt aIndex);
	TBool DeleteRule(TInt aIndex);
	TInt InsertRule(CReplaceRule &rule, TInt aIndex = -1);
	CReplaceRule * GetReplaceRule(TInt aIndex);
	CTextConverterDoc(CEikApplication& aApp);
	CReplaceRules *ReplaceRules() {return &iReplaceRules;}
	CRulesetOptions *RulesetOptions() {return &iRulesetOptions;}
private: // from CEikDocument
	CEikAppUi* CreateAppUiL();

private: // from MDesCArray
	TInt MdcaCount() const;
	TPtrC MdcaPoint(TInt anIndex) const;
public:
	void DoFileRevertL();
	void DoFileSaveToNewL(const TFileName& aNewFileName);
	TBool IsCurrentFileName(const TFileName& aFileName)const;
	CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);
	void DoOpenFileL(const TFileName &filename);
	void DoNewFileL(const TFileName& aFileName);
};

class CTextConverterApp : public CEikApplication
	{
private: // from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;

	};
