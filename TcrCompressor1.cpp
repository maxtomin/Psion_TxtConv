#include "TcrCompressor.h"

void CTcrCompressor::ReadInputFile() {

}

void CTcrCompressor::WriteOutputFile() {

}

void CTcrCompressor::InitDictionary() {
	iDictionary.SetCount(256);
	for (TUint8 i = 0; i <= 255; i++) {
		iDictionary[i].Zero();
		iDictionary[i].Append(TUint8(i));
	}
}

TBool CTcrCompressor::Merge() {
	TIntArray <256> CharsAfter;
	for (TInt i = 0; i < 256; i++)
		CharsAfter[i] = -1; //Symbol not used
	
	TUint8 NextChar, PrevChar = iBuffer[0];
	iCurPos = iDictionary[PrevChar].Length();

	while (iCurPos < iBuffer.Length()) {
		NextChar = iBuffer[iCurPos];
		if (CharsAfter[PrevChar] == -1)
			CharsAfter[PrevChar] = NextChar
		else if (CharsAfter[PrevChar] != -2 && CharsAfter[PrevChar] != NextChar)
			CharsAfter[PrevChar] = -2;
		
		PrevChar = NextChar; //More than 1 NextChar
		iCurPos += iDictionary[NextChar].Length();
	}

	TBool Changed = EFalse;
	for (TInt i = 0; i < 256; i++)
		if (CharsAfter[i] >= 0) {
			Changed = ETrue;
			iDictionary[i].Append(iDictionary[CharsAfter[i]]);
		}
	return Changed;
}

TBool CTcrCompressor::DeleteUnused() {
	TUint8 NextChar, PrevChar = iBuffer[0];
	iCurPos = iDictionary[PrevChar].Length();
	TBoolArray <256> Used;
	for (TInt i = 0; i < 256; i++)
		Used[i] = EFalse; //Symbol not used

	while (iCurPos < iBuffer.Length()) {
		Used[iBuffer[iCurPos]] = ETrue;
		iCurPos += iDictionary[NextChar].Length();
	}
	TBool Changed = EFalse;
	for (TInt i = 0; i < 256; i++)
		if (!Used[i]) {
			iDictionary[i].Zero();
			Changed = ETrue;
		}
}

TBool CTcrCompressor::AddNew() {
	//Findind frequencies:
	TIntMatrix <256, 256> Freq;

	TUint8 NextChar, PrevChar = iBuffer[0];
	iCurPos = iDictionary[PrevChar].Length();

	while (iCurPos < iBuffer.Length()) {
		NextChar = iBuffer[iCurPos];
		Freq[PrevChar, NextChar]++;
		
		PrevChar = NextChar; //More than 1 NextChar
		iCurPos += iDictionary[NextChar].Length();
	}
	
	//adding dict items:
	TBool Changed = EFalse;
	for (;;) {
		//finding not used dict lines:
		TInt DictIndex = 0;
		while (DictIndex < iDictionary.Count() && iDictionary[DictIndex].Length()) i++;
		if (DictIndex == iDictionary.Count()) break;
		Changed = ETrue;

		//finding max freq:
		TInt MaxI, MaxJ, MaxFreq = 0;
		for (TInt i = 0; i < 256; i++) 
			for (TInt j = 0; j < 256; j++)
				if (Freq[i, j] > MaxFreq) {
					MaxI = i;
					MaxJ = j;
					MaxFreq = Freq[i, j];
				}
		if (!MaxFreq) break;
	
		//adding dict item:
		iDictionary[DictIndex].Append(iDictionary[MaxI]);
		iDictionary[DictIndex].Append(iDictionary[MaxJ]);
		
		//changing text:
		PrevChar = iBuffer[0];
		iCurPos = iDictionary[PrevChar].Length();
		OldPos = 0;

		while (iCurPos < iBuffer.Length()) {
			NextChar = iBuffer[iCurPos];
			if (PrevChar == MaxI && NextChar == MaxJ) 
				iBuffer[OldPos] = DictIndex;
		
			PrevChar = NextChar; //More than 1 NextChar
			OldPos = iCurPos;
			iCurPos += iDictionary[NextChar].Length();
		}
	}
}

void CTcrCompressor::CompressL() {
	ReadInputFile();
	InitDictionary();
	while (Merge() || DeleteUnused() || AddNew()) ;
	WriteOutputFile();	
}