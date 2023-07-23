// CompressTcr.h: interface for the CCompressTcr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPRESSTCR_H__AF283985_8144_4175_9214_9CDCF7F82BC6__INCLUDED_)
#define AFX_COMPRESSTCR_H__AF283985_8144_4175_9214_9CDCF7F82BC6__INCLUDED_

#include "TextConversion.h"

#define TBuf256 TBuf <256>

class CCompressTcr : public CTextConversion  
{
private:
	RFile *iOutFile;
	TUint8 * strchr(TUint8 *str, TUint8 token);
	void strcat(TUint8 *dest, TUint8 *source);
	void strcpy(TUint8 *dest, TUint8 *source);
	void memset(TAny *ptr, TUint8 value, TInt len);
	TBool Merge();
	void Create(TUint8 aTo, TUint8 aFrom1, TUint8 aFrom2);
	TInt FindBestCreate();
	TInt FindFreeCode();
	TInt GetVirtualFilePosL();


	TInt iC, iI;
	HBufC *pBuffer; //own
	TUint8 *text;
	TInt textLength, origLength;			// And its length

	TUint8 dict[256][256];	// The 256 dictionary strings, zero-terminated
	TInt dictLength[257];		// Their lengths (for speed; always = strlen(cData->dict[])) - and a fence
	TInt dictTotalLength;		// The sum + 256, i.e. the size of the dictionary within the TCR file

	TUint8 freq[256][256];		// cData->freq[a][b] is # occurrences of a followed by b in the text
	TInt freq_valid;				// Whether the cData->freq[][] table is up-to-date
	TInt iPassCount;

	
	//static for "merge" function
	TInt follows[256], sfreq[256], skip[256], seen[256];
	enum TTcrConvertState {tcsInitialMerge, tcsFindFreeCode, tcsFindBestCreate, tcsCreate, tcsMerge, tcsFinalize} iState;
public:
	void ConstructL(RFile *aSource, RFile *aDest);
	virtual TBool DoStepL(TInt &aFilePos);
	CCompressTcr(RFile *aOutFile);
	virtual ~CCompressTcr();

};

#endif // !defined(AFX_COMPRESSTCR_H__AF283985_8144_4175_9214_9CDCF7F82BC6__INCLUDED_)
