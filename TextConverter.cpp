#include "TextConverter.h"

#include "TextConverter.rsg"
#include "TextConverter.hrh"
#include <eikfnlab.h>
#include "eiktbar.h"
#include <eikcmds.hrh>
#include <eiktxlbm.h>
#include "RuleEditDialog.h"
#include <eikinfo.h>
#include <eikon.rsg>
#include "RulesetOptionsDialog.h"
#include <eikcfdlg.h>
#include <coeutils.h>
#include <eikproc.h>
#include <s32file.h>
#include <eikenv.h>
#include "RFileWithCleanup.h"
#include "ConverterActive.h"

_LIT(KTextConverterDefaultFilePath, "C:\\");


//
// EXPORTed functions
//

EXPORT_C CApaApplication* NewApplication()
	{
	return new CTextConverterApp;
	}

GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}


//
// CTextConverterApp class
//

TUid CTextConverterApp::AppDllUid() const
	{
	return KUidTextConverter;
	}

CApaDocument* CTextConverterApp::CreateDocumentL()
	{
	return new (ELeave) CTextConverterDoc(*this);
	}

//
// CTextConverterDoc class
//

CTextConverterDoc::CTextConverterDoc(CEikApplication& aApp) : 
	CEikDocument(aApp),
	iReplaceRules(10) 
{

}


CEikAppUi* CTextConverterDoc::CreateAppUiL()
	{
    return new (ELeave) CTextConverterAppUi;
	}


//
// CTextConverterAppUi class
//

void CTextConverterAppUi::ConstructL()
{

    BaseConstructL();
    iAppView=new(ELeave) CTextConverterAppView;
    iAppView->ConstructL(ClientRect(), this, (CTextConverterDoc *)Document());

	// Display the application name at the top of the toolbar
	// First get the control to display it in.
	CEikFileNameLabel* filenameLabel=STATIC_CAST(CEikFileNameLabel*, iToolBar->ControlById(EExampleCmdFileName));
	
	// Then display the application name. UpdateL() displays the application's 
	// main document file name in the filename label control. However, in this
	// application, there is no main document file (because the application is
	// not file based). In this case, UpdateL() displays the application name by
	// default.
	filenameLabel->UpdateL();
	AddToStackL(iAppView);
	/*for (TInt i = 0; i < iAppView->CountComponentControls(); i++)
		AddToStackL(iAppView->ComponentControl(i));*/
}


void CTextConverterAppUi::HandleCommandL(TInt aCommand)
{
	CTextConverterDoc *doc = (CTextConverterDoc *)Document();
	switch (aCommand) {
	case ECmdAddRule: 
		{
			CReplaceRule rule;
			CRuleEditDialog *dlg = new (ELeave) CRuleEditDialog(&rule);
			if (!dlg->ExecuteLD(R_RULE_EDIT)) break;
			
			CTextConverterDoc *doc = (CTextConverterDoc *)Document();
			doc->InsertRule(rule);
			doc->SetChanged(ETrue);
			iAppView->HandleItemAdditionL();
		}
	break;
	case ECmdInsertRule: 
		{		
			CReplaceRule rule;
			CRuleEditDialog *dlg = new (ELeave) CRuleEditDialog(&rule);
			if (!dlg->ExecuteLD(R_RULE_EDIT)) break;
			
			CTextConverterDoc *doc = (CTextConverterDoc *)Document();
			doc->InsertRule(rule, iAppView->GetSelectedRuleIndex());
			doc->SetChanged(ETrue);
			iAppView->HandleItemAdditionL();
			iAppView->DrawNow();
		}
		break;
	case ECmdEditRule: 
		if (!doc->ReplaceRules()->Count()) break;
		if (!(new (ELeave) CRuleEditDialog(iAppView->GetSelectedRule()))->ExecuteLD(R_RULE_EDIT)) break;
		doc->SetChanged(ETrue);
		iAppView->DrawNow();
		break;
	case ECmdDeleteRule: 
		if (!doc->ReplaceRules()->Count()) break;
		if (!(new (ELeave) CEikInfoDialog(_L("Delete current rule?")))->ExecuteLD(R_EIK_DIALOG_QUERY)) break;
		doc->DeleteRule(iAppView->GetSelectedRuleIndex());
		doc->SetChanged(ETrue);
		iAppView->HandleItemRemovalL();
		iAppView->DrawNow();
		break;
	case ECmdMoveRuleDown: 
		if (!doc->ReplaceRules()->Count()) break;
		if (!(doc->MoveRuleDown(iAppView->GetSelectedRuleIndex()))) break;
		doc->SetChanged(ETrue);
		iAppView->HandleMoveRuleDownL();
		iAppView->DrawNow();
		break;
	case ECmdMoveRuleUp: 
		if (!doc->ReplaceRules()->Count()) break;
		if (!(doc->MoveRuleUp(iAppView->GetSelectedRuleIndex()))) break;
		doc->SetChanged(ETrue);
		iAppView->HandleMoveRuleUpL();
		iAppView->DrawNow();
		break;
	case ECmdRulesetOptions: 
		if (!(new (ELeave) CRulesetOptionsDialog(doc->RulesetOptions()))->ExecuteLD(R_RULESET_OPTIONS)) break;
		doc->SetChanged(ETrue);
		break;
	case ECmdConvertText:
		CmdConvertTextL(EFalse);
		break;
	case ECmdConvertTextDebug:
		CmdConvertTextL(ETrue);
		break;
	case EEikCmdFileNew:
		CmdFileNewL(); 
		break;
	case EEikCmdFileOpen:
		CmdFileOpenL(); 
		break;
	case EEikCmdFileSave:
		CmdFileSaveL();
		break;
	case EEikCmdFileSaveAs:
		CmdFileSaveAsL();
		break;
	case EEikCmdFileRevert:
		CmdFileRevertL();
		break;
	case EEikCmdExit:
		CmdFileSaveL(); 
		Exit();
	}

}

CTextConverterAppUi::~CTextConverterAppUi() {
	delete iAppView;
}

void CTextConverterAppView::ConstructL(const TRect &aRect, CTextConverterAppUi *aAppUi, CTextConverterDoc *aDocument) {
	iAppUi = aAppUi;
	iDocument = aDocument;
    CreateWindowL();
    SetRectL(aRect);
	
	//construct control
	iListBox = new (ELeave) CEikTextListBox;
	iListBox->ConstructL(this, CEikTextListBox::EItemDoubleClicked);
	iListBox->Model()->SetItemTextArray(iDocument);
	iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

	iListBox->SetListBoxObserver(iAppUi);

	// set horizontal and vertical scroll bars to appear when required
	CEikScrollBarFrame* scrollbarFrame = iListBox->CreateScrollBarFrameL();
	scrollbarFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
		CEikScrollBarFrame::EAuto);

	iListBox->UpdateScrollBarsL();

	iListBox->SetRectL(aRect);

	iListBox->SetFocus(ETrue, EDrawNow);
	

	ActivateL();
}


TInt CTextConverterAppView::CountComponentControls() const
{
	return 1;
}

CTextConverterAppView::~CTextConverterAppView() {
	delete iListBox;
}

CCoeControl * CTextConverterAppView::ComponentControl(TInt aIndex) const
{
	switch (aIndex)	{
	case 0: return iListBox;
	default: return 0;
	}

}

TInt CTextConverterDoc::MdcaCount() const
{
	TInt i = iReplaceRules.Count(); 
	return i;
}

TPtrC CTextConverterDoc::MdcaPoint(TInt anIndex) const
{
	return iReplaceRules[anIndex].iRuleName;
		
}

void CTextConverterAppUi::HandleListBoxEventL(CEikListBox *, TListBoxEvent aEventType)
{
	switch (aEventType) {
	case EEventEnterKeyPressed:
	case EEventItemDoubleClicked:
		HandleCommandL(ECmdEditRule);
		break;
	default: ;
	}
}

CReplaceRule * CTextConverterAppView::GetSelectedRule()
{
	return iDocument->GetReplaceRule(GetSelectedRuleIndex());
}

TInt CTextConverterAppView::GetSelectedRuleIndex()
{
	return iListBox->CurrentItemIndex();
}

CReplaceRule * CTextConverterDoc::GetReplaceRule(TInt aIndex)
{
	return &iReplaceRules[aIndex];

}

TInt CTextConverterDoc::InsertRule(CReplaceRule &rule, TInt aIndex)
{
	if (aIndex == -1) {
		iReplaceRules.AppendL(rule, sizeof(CReplaceRule));
		return iReplaceRules.Count() - 1;
	}
	else {
		iReplaceRules.InsertL(aIndex, rule, sizeof(CReplaceRule));
		return aIndex;
	}
	SetChanged(ETrue);
}		

void CTextConverterAppView::HandleItemAdditionL()
{
	iListBox->HandleItemAdditionL();
}

TBool CTextConverterDoc::DeleteRule(TInt aIndex)
{
	if (aIndex >= iReplaceRules.Count()) return EFalse;
	iReplaceRules.Delete(aIndex);
	return ETrue;
}

void CTextConverterAppView::HandleItemRemovalL()
{
	iListBox->HandleItemRemovalL();
}


TBool CTextConverterDoc::MoveRuleDown(TInt aIndex)
{
	if (aIndex >= iReplaceRules.Count() - 1) return EFalse;
	CReplaceRule Rule = iReplaceRules[aIndex + 1];
	iReplaceRules[aIndex + 1] = iReplaceRules[aIndex];
	iReplaceRules[aIndex] = Rule;
	return ETrue;
}

TBool CTextConverterDoc::MoveRuleUp(TInt aIndex)
{
	if (aIndex == 0) return EFalse;
	CReplaceRule Rule = iReplaceRules[aIndex - 1];
	iReplaceRules[aIndex - 1] = iReplaceRules[aIndex];
	iReplaceRules[aIndex] = Rule;
	return ETrue;
}

void CTextConverterAppView::HandleMoveRuleDownL()
{
	iListBox->SetCurrentItemIndex(iListBox->CurrentItemIndex() + 1);
}

void CTextConverterAppView::HandleMoveRuleUpL()
{
	iListBox->SetCurrentItemIndex(iListBox->CurrentItemIndex() - 1);
}


void CTextConverterDoc::StoreL(CStreamStore &aStore, CStreamDictionary &aStreamDic) const
{
	RStoreWriteStream stream; // Declare a stream
	TStreamId id = stream.CreateLC(aStore); // Create the stream
	
	ExternalizeL(stream); // Write the data to the stream
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // pop and destroy the stream
	

    aStreamDic.AssignL(KUidTextConverter,id); 
	                                    // Write the stream ID,
                                        // together with a UID,
                                        // to the stream dictionary
}

void CTextConverterDoc::RestoreL(const CStreamStore &aStore, const CStreamDictionary &aStreamDic)
{
	TStreamId id=aStreamDic.At(KUidTextConverter); // find the stream ID
                                                   // of the model data from
                                                   // the stream dictionary
	

    RStoreReadStream stream; // declare a read stream
    stream.OpenLC(aStore, id); // Open the stream
    InternalizeL(stream); // Read the data from the stream
    CleanupStack::PopAndDestroy(); // pop and destroy the stream

    SetChanged(EFalse); // We've just restored this document,
                        // so it can't have changed.
}

/*
Stream structure (version 1):
Size	Value
4		Count in iReplaceRules array
X		iReplaceRules array (universal)
1		iRulesetOptions (universal)
*/

void CTextConverterDoc::ExternalizeL(RWriteStream &aStream) const
{
	aStream.WriteInt32L(iReplaceRules.Count());
	for (TInt i = 0; i < iReplaceRules.Count(); i++)
		((CTextConverterDoc *)this)->iReplaceRules[i].ExternalizeL(aStream);
	
	((CTextConverterDoc *)this)->iRulesetOptions.ExternalizeL(aStream);
}

void CTextConverterDoc::InternalizeL(RStoreReadStream &stream)
{
	iReplaceRules.Reset();
	TInt RuleCount = stream.ReadInt32L();
	
	for (TInt i = 0; i < RuleCount; i++) {
		CReplaceRule rule;
		rule.InternalizeL(stream);
		iReplaceRules.AppendL(rule, sizeof(rule));
	}

	iRulesetOptions.Reset();
	iRulesetOptions.InternalizeL(stream);
}

void CTextConverterDoc::NewDocumentL()
{
	iRulesetOptions.Reset();
	iReplaceRules.Reset();
}

TBool CTextConverterAppUi::ProcessCommandParametersL(TApaCommand aCommand, TFileName &aDocumentName, const TDesC &)
{
	return CEikAppUi::ProcessCommandParametersL(aCommand,aDocumentName);
}


void CTextConverterAppUi::CmdFileSaveL()
{
	if (Document()->HasChanged())
		{
		SaveL();
		iEikonEnv->InfoMsg(R_DOCUMENT_SAVED);
		}
	else
		// Inform the user that the document was unchanged.
		iEikonEnv->InfoMsg(R_DOCUMENT_UNCHANGED);
}

void CTextConverterAppUi::HandleModelChangeL()
{
	iAppView->HandleItemAdditionL();

	// Update the file name in the task list to show the current file.
	iEikonEnv->UpdateTaskNameL();
	// Display the current document file name on the toolbar.
	// Get the label's control using its ID as defined in the resource file
	CEikFileNameLabel* filenameLabel=STATIC_CAST(CEikFileNameLabel*, iToolBar->ControlById(EExampleCmdFileName));
	// CEikFileNameLabel::UpdateL() sets the label to display the application's
	// main document file name. If the main document file name is empty, it sets
	// the label to the application name.
	filenameLabel->UpdateL();
	// Redraw the label
	filenameLabel->DrawNow();

}

void CTextConverterAppUi::CmdFileNewL()
{
	// Set an initial value for the folder to contain the new file
	// Pass this to the dialog box to display in the "Folder" field.
	TFileName fileName;
	SetInitialPathL(fileName);
	// Launch a standard EIKON dialog for the user to enter a filename
	// for the new file.
	CEikDialog* dialog=new(ELeave) CEikFileSaveAsDialog(&fileName,NULL,R_EIK_TBUF_CREATE_NEW_FILE);
	// Call CreateFileL() when the dialog returns a filename value
	// Note: R_EIK_DIALOG_FILE_SAVEAS means that the dialog will request the user to
	// confirm the action if the file already exists
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS)) 
		CreateFileL(fileName);
	
}

void CTextConverterAppUi::CmdFileOpenL()
{
	// Set an initial value for the folder to contain the new file
	// Pass this to the dialog box to display in the "Folder" field.
	TFileName filename;
	SetInitialPathL(filename);
	// Launch a standard EIKON dialog for the user to enter a filename
	CEikFileOpenDialog* dialog=new(ELeave) CEikFileOpenDialog(&filename);
	dialog->RestrictToNativeDocumentFiles();
	// If the dialog returns a filename, call OpenFileL() to open the file.
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN))
		OpenFileL(filename);
}

void CTextConverterAppUi::CmdFileSaveAsL()
{
	// Set an initial value for the folder to contain the file to save as.
	// Pass this to the dialog box to display in the "Folder" field.
 	TFileName fileName;
	SetInitialPathL(fileName);
	// Launch a standard EIKON dialog for the user to enter a filename
	CEikDialog* dialog=new(ELeave) CEikFileSaveAsDialog(&fileName,NULL,NULL);
	// ExecuteLD returns true only if the user entered a valid filename
	// The R_EIK_DIALOG_FILE_SAVEAS parameter means the dialog will ask
	// the user to verify an overwite if the file already exists.
	if (dialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS))
		{
		// If the dialog returns a filename that is the same as
		// the current file, we're really doing a "Save", not a "Save as".
		if (((CTextConverterDoc*)Document())->IsCurrentFileName(fileName))
			{
			CmdFileSaveL();
			return;
			}
		// If the file already exists, delete it.
		TEntry entry;
		if (!iEikonEnv->FsSession().Entry(fileName,entry))
			User::LeaveIfError(iEikonEnv->FsSession().Delete(fileName));
		// Call the document function to save to a new file
		((CTextConverterDoc*)Document())->DoFileSaveToNewL(fileName);
		iEikonEnv->InfoMsg(R_EIK_TBUF_FILE_SAVED);
		
		// Update the UI to reflect the changes
		HandleModelChangeL();
		iEikonEnv->InfoMsg(R_DOCUMENT_SAVED);
		}

}

void CTextConverterAppUi::CmdFileRevertL()
{
	if (Document()->HasChanged())
		{
		// Ask the user to confirm the file revert, because changes will be lost
		if (iEikonEnv->QueryWinL(R_REVERT_TITLE, R_REVERT_TEXT))
			{
			((CTextConverterDoc*)Document())->DoFileRevertL();
			// Update the UI to reflect the changes
			HandleModelChangeL();
			}
		}
	else
		// Inform the user that the document was unchanged.
		iEikonEnv->InfoMsg(R_DOCUMENT_UNCHANGED);
}

void CTextConverterAppUi::SetInitialPathL(TFileName &aFileName) const
{
	aFileName=CurrentFilePath();
	TRAPD(ret,ConeUtils::EnsurePathExistsL(aFileName) );
	if (ret!=KErrNone)
		{// if the path is eg read-only default to C:
		aFileName=KTextConverterDefaultFilePath;
		ConeUtils::EnsurePathExistsL(aFileName);
		}
}

TFileName CTextConverterAppUi::CurrentFilePath() const
{
	TParsePtrC parser(Document()->Process()->MainDocFileName());
	return parser.DriveAndPath();
}

void CTextConverterAppUi::CreateFileL(const TDesC &aFileName)
{
	// Save any changes to the current file before creating the new one
	SaveAnyChangesL();
	// Call the document function to create a new file
	STATIC_CAST(CTextConverterDoc*,iDocument)->DoNewFileL(aFileName);
	// Update the UI to reflect the changes
	HandleModelChangeL();
}

void CTextConverterDoc::DoNewFileL(const TFileName &aFileName)
{
	CFileStore* store = ((CEikProcess*)Process())->MainStore();
	RFile file = store->File();
	store->Detach();
	file.Close();
	
	file.Replace(Process()->FsSession(), aFileName, EFileRead|EFileWrite);
	store->Reattach(file);

	NewDocumentL();
	((CEikProcess*)Process())->SetMainStore(store);
	Process()->SetMainDocFileName(aFileName);
	SaveL();


/*	CFileStore* store =  CreateFileStoreLC(Process()->FsSession(),aFileName);
	  // Reset the document back to its default settings
//	NewDocumentL();
	  // Remove store from the cleanup stack
	CleanupStack::Pop();  
	  // Delete the old store object (closes the file)
	delete ((CEikProcess*)Process())->MainStore();
	  // Set the new store as the main store.
	((CEikProcess*)Process())->SetMainStore(store);
	Process()->SetMainDocFileName(aFileName);*/
}

void CTextConverterDoc::DoOpenFileL(const TFileName &aFileName)
{
    
	  // Open the store and construct the file store object and the stream
	  // dictionary object
   	CFileStore* store = NULL;
	CStreamDictionary* dic = CApaProcess::ReadRootStreamLC(Process()->FsSession(),store,aFileName,EFileShareExclusive|EFileWrite);
	  // Push the store onto the cleanup stack 
	CleanupStack::PushL(store);
	  // Restore content from this store
	RestoreL(*store,*dic);
	  // Can now remove the store from the cleanup stack
	CleanupStack::Pop();
	  // Cleanup the stream dictionary
	CleanupStack::PopAndDestroy();
	   // Delete the old store object (closes the file)
	delete(((CEikProcess*)Process())->MainStore());
	  // Set the new store as the main store.
	((CEikProcess*)Process())->SetMainStore(store);
	  // Set the file name of the new store as the main file name
	Process()->SetMainDocFileName(aFileName);
}

void CTextConverterAppUi::OpenFileL(const TDesC& aFileName)
{
	SaveAnyChangesL(); // This is implemented by CEikAppUi
	// Call the document function to open a file
	STATIC_CAST(CTextConverterDoc*,iDocument)->DoOpenFileL(aFileName);
	// Update the UI to reflect the changes
	HandleModelChangeL();
}

CFileStore* CTextConverterDoc::OpenFileL(TBool aDoOpen, const TDesC &aFilename, RFs &aFs)
{
	return CEikDocument::OpenFileL(aDoOpen, aFilename, aFs);
}

TBool CTextConverterDoc::IsCurrentFileName(const TFileName &aFileName) const
{
	return (aFileName==Process()->MainDocFileName());
}

void CTextConverterDoc::DoFileSaveToNewL(const TFileName &aNewFileName)
{
	// Constructs a new store of this name, closes the current store,
	// and makes the new store the main one.
	((CEikProcess*)Process())->SaveToDirectFileStoreL(this,&aNewFileName);
	SetChanged(EFalse);

}

void CTextConverterDoc::DoFileRevertL()
{
	CStreamDictionary* streamDic=CStreamDictionary::NewL();
	CleanupStack::PushL(streamDic);
	CFileStore* mainStore=REINTERPRET_CAST(CEikProcess*,Process())->MainStore();
	RStoreReadStream root;
	root.OpenLC(*mainStore,mainStore->Root());
	root>> *streamDic;
	root.Close();
	CleanupStack::PopAndDestroy(); // root
	RestoreL(*mainStore,*streamDic);
	CleanupStack::PopAndDestroy(); // streamDic
}

_LIT(KDebugFileName, "debug.txt");
void CTextConverterAppUi::CmdConvertTextL(TBool aDebugMode)
{
	// Set an initial value for the folder to contain the new file
	// Pass this to the dialog box to display in the "Folder" field.
	TFileName InFileName(KTextConverterDefaultFilePath);
	ConeUtils::EnsurePathExistsL(InFileName);


	RFile File;
	// Launch a standard EIKON dialog for the user to enter a input filename
	//STATIC_CAST(CEikonEnv*,iCoeEnv)->ReadResource(Title, R_SELECT_SOURCE_TITLE);
	CEikFileOpenDialog* OpenDialog=new(ELeave) CEikFileOpenDialog(&InFileName, R_SELECT_SOURCE_TITLE);
	
	// If the dialog returns a filename, call OpenFileL() to open the file.
	if (!OpenDialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN)) return;
	//try opening file:
	User::LeaveIfError(File.Open(iEikonEnv->FsSession(), InFileName, EFileRead | EFileShareReadersOnly));
	File.Close();

	TFileName OutFileName = InFileName;
	CEikFileSaveAsDialog* SaveDialog=new (ELeave) CEikFileSaveAsDialog(&OutFileName, NULL, R_SELECT_DEST_TITLE);
	if (!SaveDialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS)) return;
	//try open file:
	User::LeaveIfError(File.Replace(iEikonEnv->FsSession(), OutFileName, EFileRead | EFileWrite));
	File.Close();
	if (!OutFileName.Compare(InFileName)) {
		STATIC_CAST(CEikonEnv*,iCoeEnv)->InfoWinL(R_ERROR_TITLE, R_IN_OUT_SIMILAR);
		return;
	}

	TFileName DebugFileName;
	if (aDebugMode) {
		DebugFileName = OutFileName;
		TInt SlashPos = DebugFileName.LocateReverse('\\');
		DebugFileName.SetLength(SlashPos + 1);
		DebugFileName.Append(KDebugFileName);

		SaveDialog=new (ELeave) CEikFileSaveAsDialog(&DebugFileName, NULL, R_SELECT_DEBUG_TITLE);
		if (!SaveDialog->ExecuteLD(R_EIK_DIALOG_FILE_SAVEAS)) return;
		if (!DebugFileName.Compare(InFileName)) {
			STATIC_CAST(CEikonEnv*,iCoeEnv)->InfoWinL(R_ERROR_TITLE, R_IN_DEBUG_SIMILAR);
			return;
		}
		if (!DebugFileName.Compare(OutFileName)) {
			STATIC_CAST(CEikonEnv*,iCoeEnv)->InfoWinL(R_ERROR_TITLE, R_OUT_DEBUG_SIMILAR);
			return;
		}
	}
	StartConvertFileL(InFileName, OutFileName, DebugFileName);
}

_LIT(KTxtConvDirAndName, "TxtConv\\temp.txt");
void CTextConverterAppUi::StartConvertFileL(TDesC &aInFileName, TDesC &aOutFileName, TDesC &aDebugFileName)
//owns aInFile and aOutFile
{
	TFileName TempFileName = iEikonEnv->Process()->TempFilePath();
	TempFileName.Append(KTxtConvDirAndName);
	RFile TempFile;
	
	User::LeaveIfError(TempFile.Replace(iEikonEnv->FsSession(), TempFileName, EFileRead | EFileWrite));
	TempFile.Close();

	//create dialog:
	CProgressDialog *ProgressDialog = new (ELeave) CProgressDialog(); //self-destructing active

	//create new active object:
	CTextConverterDoc *doc = static_cast <CTextConverterDoc *> (Document());

	CConverterActive *testActive = new CConverterActive(ProgressDialog, doc->RulesetOptions(), doc->ReplaceRules()); //also add to AS, self-destructing active
	testActive->ConstructL(aInFileName, aOutFileName, TempFileName, aDebugFileName);
	ProgressDialog->DoExecuteLD(testActive, aDebugFileName.Length());
}


TKeyResponse CTextConverterAppView::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
{
	TKeyCode Code = (TKeyCode)(aKeyEvent.iCode);
	if (Code == EKeyBackspace || Code == EKeyDelete) {
		
		iAppUi->HandleCommandL(ECmdDeleteRule);
		return EKeyWasConsumed;
	}

	TKeyResponse response = iListBox->OfferKeyEventL(aKeyEvent, aType);
	if (response == EKeyWasConsumed) 
		return EKeyWasConsumed;
	else 
		return CCoeControl::OfferKeyEventL(aKeyEvent, aType);
}	

