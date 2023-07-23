// TcrCompressor.cpp: implementation of the CTcrCompressor class.
//
//////////////////////////////////////////////////////////////////////

#include "TcrCompressor.h"

CTcrCompressor::CTcrCompressor(CFileReader *aInStream, CFileWriter	*aOutStream):
	iInStream(aInStream),
	iOutStream(aOutStream) {}

	

void CTcrCompressor::ReadInputFileL() {
	pBuffer = HBufC::NewL(iInStream->FileSize());
	TUint8 Char;
	for (TInt i = 0; i < iInStream->FileSize(); i++) {
		iInStream->ReadChar(Char);
		iBuffer.Append(Char);
	}
	

	
}

void CTcrCompressor::WriteOutputFileL() {
	//Writing header:
	iOutStream->WriteCharL('!');
	iOutStream->WriteCharL('!');
	iOutStream->WriteCharL('8');
	iOutStream->WriteCharL('-');
	iOutStream->WriteCharL('B');
	iOutStream->WriteCharL('i');
	iOutStream->WriteCharL('t');
	iOutStream->WriteCharL('!');
	iOutStream->WriteCharL('!');
	
	//Writing dictionary:
	for (TInt i = 0; i < iDictionary.Count(); i++) {
		iOutStream->WriteCharL(TUint8(iDictionary[i]->Length()));
		for (TInt j = 0; j < iDictionary[i]->Length(); j++)
			iOutStream->WriteCharL(TUint8((*iDictionary[i])[j]));
	}

	//Writing text:
	TInt CurPos = 0;
	while (CurPos < iBuffer.Length()) {
		iOutStream->WriteCharL(iBuffer[CurPos]);
		CurPos += iDictionary[iBuffer[CurPos]]->Length();
	}
}

void CTcrCompressor::InitDictionary() {
	for (TInt i = 0; i <= 255; i++) {
		iDictionary[i] = new TBuf <256>;
		iDictionary[i]->Append(TUint8(i));
	}
}

TBool CTcrCompressor::Merge() {
	TFixedArray <TInt, 256> CharsAfter;
	for (TInt i = 0; i < 256; i++)
		CharsAfter[i] = -1; //Symbol not used
	
	TUint8 NextChar, PrevChar = iBuffer[0];
	TInt iCurPos = iDictionary[PrevChar]->Length();

	while (iCurPos < iBuffer.Length()) {
		NextChar = iBuffer[iCurPos];
		if (CharsAfter[PrevChar] == -1)
			CharsAfter[PrevChar] = NextChar;
		else if (CharsAfter[PrevChar] != -2 && CharsAfter[PrevChar] != NextChar)
			CharsAfter[PrevChar] = -2;
		
		PrevChar = NextChar; //More than 1 NextChar
		iCurPos += iDictionary[NextChar]->Length();
	}

	TBool Changed = EFalse;
	for (i = 0; i < 256; i++)
		if (CharsAfter[i] >= 0 && iDictionary[i]->Length() + iDictionary[CharsAfter[i]]->Length() <= 255) {
			Changed = ETrue;
			iDictionary[i]->Append(*iDictionary[CharsAfter[i]]);
			TInt ti = iDictionary[i]->Length();
			TBuf <256> Test;
			Test.Append(*iDictionary[i]);
			ti = 0;
		}
	return Changed;
}

TBool CTcrCompressor::DeleteUnused() {
	TUint8 NextChar;
	TInt iCurPos = 0;
	TFixedArray <TBool, 256> Used;
	for (TInt i = 0; i < 256; i++)
		Used[i] = EFalse; //Symbol not used

	while (iCurPos < iBuffer.Length()) {
		NextChar = iBuffer[iCurPos];
		Used[NextChar] = ETrue;
		iCurPos += iDictionary[NextChar]->Length();
	}

	TBool Changed = EFalse;
	for (i = 0; i < 256; i++)
		if (!Used[i] && iDictionary[i]->Length()) {
			iDictionary[i]->Zero();
			Changed = ETrue;
		}
	return Changed;
}

TBool CTcrCompressor::AddNew() {
	//Findind frequencies:
	CArrayFixFlat <CFreqInfo> *Freq = new (ELeave) CArrayFixFlat<CFreqInfo>(100);	
	CleanupStack::PushL(Freq);

	TUint8 NextChar, PrevChar = iBuffer[0];
	TUint16 PrevPrevChar = 256;
	TInt iCurPos = iDictionary[PrevChar]->Length();

	while (iCurPos < iBuffer.Length()) {
		NextChar = iBuffer[iCurPos];
		if (PrevPrevChar == 256 || PrevPrevChar != NextChar || PrevChar != NextChar)
			CheckAndAddFreq(*Freq, PrevChar, NextChar);

		PrevPrevChar = PrevChar;
		PrevChar = NextChar; //More than 1 NextChar
		iCurPos += iDictionary[NextChar]->Length();
	}
	
	//adding dict items:
	TBool Changed = EFalse;
	for (;;) {
		//finding not used dict lines:
		TUint DictIndex = 0;
		while (DictIndex < iDictionary.Count() && iDictionary[DictIndex]->Length()) DictIndex++;
		if (DictIndex == iDictionary.Count()) break;

		//finding max freq:
		TInt MaxI = -1, MaxFreq = 0;
		for (TInt i = 0; i < Freq->Count(); i++) 
			if ((*Freq)[i].iFreq > MaxFreq) {
				MaxI = i;
				MaxFreq = (*Freq)[i].iFreq;
			}
		if (MaxFreq <= 1) break;
		(*Freq)[MaxI].iFreq = 0;
	
		Changed = ETrue;
		//adding dict item:
		TBuf <256> Test;
		Test.Append(*iDictionary[(*Freq)[MaxI].iPrevChar]);
		Test.Append(*iDictionary[(*Freq)[MaxI].iNextChar]);
		TInt ti = Test.Length();
		iDictionary[DictIndex]->Append(*iDictionary[(*Freq)[MaxI].iPrevChar]);
		iDictionary[DictIndex]->Append(*iDictionary[(*Freq)[MaxI].iNextChar]);
		
		//changing text:
		PrevChar = iBuffer[0];
		iCurPos = iDictionary[PrevChar]->Length();
		TInt OldPos = 0;

		while (iCurPos < iBuffer.Length()) {
			NextChar = iBuffer[iCurPos];
			if (PrevChar == (*Freq)[MaxI].iPrevChar && NextChar == (*Freq)[MaxI].iNextChar) 
				iBuffer[OldPos] = DictIndex;
		
			PrevChar = NextChar; //More than 1 NextChar
			OldPos = iCurPos;
			iCurPos += iDictionary[NextChar]->Length();
		}
	}
	CleanupStack::PopAndDestroy();
	return Changed;
}

void CTcrCompressor::CompressL() {
	ReadInputFileL();
	InitDictionary();
	TBool Continue = ETrue;
	for (TInt i = 0; Continue; i++) {
		Continue = Merge();
		Continue = DeleteUnused() || Continue;
		Continue = AddNew() || Continue;
		if (i == 2) {
			/*WriteOutputFileL();	
			iOutStream->FlushL();*/
			i = 1000;
		}
	}
	WriteOutputFileL();	
}

TInt CTcrCompressor::CheckAndAddFreq(CFreqArray &arr, TUint8 PrevChar, TUint8 NextChar)
{
	if (iDictionary[PrevChar]->Length() + iDictionary[NextChar]->Length() > 255) return EFalse;
	for (TInt i = 0; i < arr.Count(); i++) {
		if (PrevChar == arr[i].iPrevChar && NextChar == arr[i].iNextChar) {
			arr[i].iFreq++;
			return ETrue;
		}
	}
	arr.AppendL(CFreqInfo(PrevChar, NextChar));
	return EFalse;
}
