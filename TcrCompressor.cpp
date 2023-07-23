// TcrCompressor.cpp: implementation of the CTcrCompressor class.
//
//////////////////////////////////////////////////////////////////////

#include "TcrCompressor.h"
//#include "ebuptcr.c"

CTcrCompressor::CTcrCompressor(CFileReader *aInStream, CFileWriter	*aOutStream):
	iInStream(aInStream),
	iOutStream(aOutStream) {}

void CTcrCompressor::CompressL() {
	
	//return;
/*	long size = iInStream->FileSize();
	pBuffer = (char *) malloc (size);
	tagCommonData *pData = (tagCommonData *)malloc(sizeof(tagCommonData));
	
	for (TInt i = 0; i < size; i++) 
		iInStream->ReadChar((unsigned char &)pBuffer[i]);

	tcr_compress(pData, pBuffer, &size);

	//Writing header "!!8-Bit!!":
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
	TUint8 DictLength, *DictItem;
	for (i = 0; i < 256; ++i) {
		DictItem = (unsigned char*)get_dictionary_item(pData, i, (char *)&DictLength);
		iOutStream->WriteCharL(DictLength);
		for (TUint16 j = 0; j < DictLength; ++j)
			iOutStream->WriteCharL(DictItem[j]);
	}


	//Writing text:
	for (i = 0; i < size; i++) 
		iOutStream->WriteCharL(pBuffer[i]);
	free(pData);
	free(pBuffer);*/
}

