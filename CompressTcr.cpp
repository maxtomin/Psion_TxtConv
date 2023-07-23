// CompressTcr.cpp: implementation of the CCompressTcr class.
//
//////////////////////////////////////////////////////////////////////

#include "CompressTcr.h"
#include "e32math.h"
#include "ConverterActive.h"
#define HEADER_LENGTH 9
#define EMPTY_PASS_COUNT 100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompressTcr::CCompressTcr(RFile *aOutFile) : CTextConversion()
{
	iState = tcsInitialMerge;
	iPassCount = EMPTY_PASS_COUNT; 
	iOutFile = aOutFile;
}

CCompressTcr::~CCompressTcr()
{
	delete pBuffer;
}

_LIT(KTcrHeader, "!!8-Bit!!");
TBool CCompressTcr::DoStepL(TInt &aFilePos)
{
	//we transfer control to the main CActive too rarely
	//so we should insert some empty RunLs in order to 
	//allow the interface process all necessary messages:
	if (iPassCount--) return EFalse; 
	iPassCount = EMPTY_PASS_COUNT;

	switch (iState) {
	case tcsInitialMerge:
		Merge();
		iState = tcsFindFreeCode;
		break;
	case tcsFindFreeCode:
		iC = FindFreeCode();
		if (iC >= 0)
			iState = tcsFindBestCreate;
		else
			iState = tcsMerge;
		break;
	case tcsFindBestCreate:
		iI = FindBestCreate();
		if (iI)
			iState = tcsCreate;
		else
			iState = tcsMerge;
		break;
	case tcsCreate:
		Create((TUint8)iC, (TUint8)((iI >> 8) & 0xFF), (TUint8)(iI & 0xFF));
		iState = tcsFindFreeCode;
		break;
	case tcsMerge:
		if (!Merge()) 
			iState = tcsFinalize;
		else
			iState = tcsFindFreeCode;
		break;
	case tcsFinalize: 
		{
			GetVirtualFilePosL();
			
			//only now, because iInFile and iOutFile can be similar:
			iFileWriter.AttachL(*iOutFile);
			iFileWriter.WriteDesL(KTcrHeader);

			TInt i = 0;
			TUint8* c;
			for (; i < 256; ++i) {
				iFileWriter.WriteCharL((TUint8)dictLength[i]);
				c = dict[i];
				while (*c) 
					iFileWriter.WriteCharL(*c++);
			}

			while (*text)
					iFileWriter.WriteCharL(*text++);
			return ETrue;
		}
	}
	aFilePos = GetVirtualFilePosL();
//	CConverterActive::AskForRetry(_L("Finish GetVirtualFilePosL"));
	return EFalse;
}

void CCompressTcr::ConstructL(RFile *aSource, RFile *aDest)
{
	TUint8 *s, Char;
	TInt seen[256];
	TInt c;

	//DO NOT CALL IT:
	//CTextConversion::ConstructL(aSource, aDest);
	iFileReader.AttachL(*aSource);
	aDest->SetSize(0); //free disk unused space
	aDest->Flush();

	//Max Tomin: initialize text pointer:
	textLength = origLength = iFileReader.FileSize();
	pBuffer = HBufC::NewL(iFileReader.FileSize() + 1);
	TPtr iBuffer(pBuffer->Des());
	while (iFileReader.ReadChar(Char))
		iBuffer.Append(Char);
	iBuffer.Append(0);
	text = (TUint8 *)iBuffer.Ptr();

	if (aSource == iOutFile || aDest == iOutFile) {
		//aSource - temp, aDest - out or vice versa
		//anyway aSource is not input file - we can clear it
		aSource->SetSize(0);
		aSource->Flush();
	}

	// Initialise the dictionary.  Each code that appears in the text is
	// set up as one character: itself.  Any unused ones are left empty.

	memset(seen, 0, sizeof(seen));
	s = text;
	while (*s)
		seen[*s++] = TRUE;

	dictTotalLength = 256;
	for (c = 0; c < 256; ++c)
		if (seen[c])
		{	dict[c][0] = (unsigned char)c;
			dict[c][1] = '\0';
			dictLength[c] = 1;
			dictTotalLength++;
		}
		else
		{	dict[c][0] = '\0';
			dictLength[c] = 0;
		}

	dictLength[256] = 0;	// Fence value

	// Do an initial merge().  This will do in one go any
	// always-followed-by type replacements that could take several
	// create() stages.  Plus it'll free up some more codes.

	freq_valid = FALSE;
}

TInt CCompressTcr::GetVirtualFilePosL()
{
	

	TReal x = HEADER_LENGTH + dictTotalLength + textLength; 
	
	x /= origLength;	//x = 0,5 .. 1
	x = (1 - x) * 2;	//x = 0 .. 1
	x *= x;				//x = 0 .. 1
	x *= origLength;	//x = 0 .. origLength

	TInt32 res;
	Math::Int(res, x);

	if (res >= origLength) res = origLength - 1;
	else if (res < 0) res = 0;
	return res;
}

TInt CCompressTcr::FindFreeCode()
{
	register TInt i = 1;		// Ignore code 0

	// Assume dictLength[] has extra fence [256] = 0.

	while (dictLength[i])
		++i;

	return (i >= 256) ? -1 : i;

}

TInt CCompressTcr::FindBestCreate() {
	TUint8 *s = text, c1 = *s++, c2;
	register TInt bestSaving, saving, bestc1, bestc2, i1, i2;

	if (!freq_valid)
	{	memset(freq, 0, sizeof(freq));
		while ((c2 = *s++) != 0)
		{	if (++freq[c1][c2] == 255)
			{	if (dictLength[c1] + dictLength[c2] < 256)
					return ((TUint16) c1 << 8) | c2;
				else
					--freq[c1][c2];
			}
			c1 = c2;
		}
		freq_valid = ETrue;
	}

	// None have overflowed, so we gotta search the freqs ourselves

	bestSaving = bestc1 = bestc2 = 0;
	for (i1 = 1; i1 < 256; ++i1)
		if (dictLength[i1])
			for (i2 = 1; i2 < 256; ++i2)
			{	saving = freq[i1][i2] - dictLength[i1] - dictLength[i2];
				// Note - a sequence of repeated characters will cause
				// overestimation!  No easy way around this, but it's not a
				// major problem...
				if (saving > bestSaving
				&& dictLength[i1] + dictLength[i2] < 256)
				{	bestc1 = i1;
					bestc2 = i2;
					bestSaving = saving;
				}
			}

	if (bestc1 == 0)
		return 0;

	/*if (ASSERTIONS && (!dictLength[bestc1] || !dictLength[bestc2]))
	{	
		printf("\nAssertion failure: empty replace code!  %i (%i), %i (%i)!\nPress RETURN...\n",
			bestc1, dictLength[bestc1], bestc2, dictLength[bestc2]);
		getchar();
	}*/

	return ((TUint16) bestc1 << 8) | bestc2;
}
 
void CCompressTcr::Create(TUint8 to, TUint8 from1, TUint8 from2)
{
	register TUint8 *s = text, *d = text, c = *s, prev;
	TBool hadFrom1 = EFalse;

/*	if (ASSERTIONS && dictLength[to])
	{	printf("\nAssertion failure: code %i is already used by `%s' (%i)\nPress RETURN...\n",
			to, dict[to], dictLength[to]);
		getchar();
	}

	if (ASSERTIONS && from1 == to)
	{	printf("\nAssertion failure: from1 = to = %i\nPress RETURN...\n", to);
		getchar();
	}*/

	// Substitute all occurrences in the text

	while (c)
		if (c != from1)
		{	*d++ = c;
			c = *++s;
		}
		else if ((c = *++s) != from2)
		{	*d++ = from1;
			hadFrom1 = ETrue;
		}
		else
		{	c = *++s;
			if (d > text)
			{	prev = d[-1];
				--freq[prev][from1];
				++freq[prev][to];
			}
			*d++ = to;
			--freq[from1][from2];
			if (c)
			{	--freq[from2][c];
				++freq[to][c];
			}
		}

	*d = '\0';
	textLength = d - text;

	// Now adjust the dictionary

	strcpy((TUint8 *) dict[to], (TUint8 *) dict[from1]);
	strcat((TUint8 *) dict[to], (TUint8 *) dict[from2]);
	dictLength[to] = dictLength[from1] + dictLength[from2];
	dictTotalLength += dictLength[to];

	// Are those codes now free?

	if (!hadFrom1)
	{	dict[from1][0] = '\0';
		dictTotalLength -= dictLength[from1];
		dictLength[from1] = 0;
	}

	if (!strchr(text, from2))
	{	dict[from2][0] = '\0';
		dictTotalLength -= dictLength[from2];
		dictLength[from2] = 0;
	} 
}

TBool CCompressTcr::Merge()
{
	register TUint8 *s = text, *d, c1 = *s++, c2;
//	static TInt follows[256], sfreq[256], skip[256];
	TBool seen[256];
	TUint8 prev;
	TInt i, j, sk;
	TBool found = EFalse;

	memset(follows, 0, sizeof(follows));
	memset(sfreq, 0, sizeof(sfreq));
	memset(seen, 0, sizeof(seen));

	// Scan for following characters
	// Set follows[c] to 0 if no characters ever follow c;
	// c2 if only c2 ever follows it, and -1 if more than one
	// character does.

	while ((c2 = *s++) != 0)
	{	if (follows[c1] < 0)
			;
		else if (follows[c1] == c2)
			++sfreq[c1];
		else if (follows[c1] == 0)
		{	follows[c1] = c2;
			sfreq[c1] = 1;
		}
		else
			follows[c1] = -1;
		c1 = c2;
	}
	follows[c1] = -1;	// Can't merge past EOF

	// Find sequences to merge.  Adjust the dictionary.
	// Set up a `skip' array for fast merging of the text.

	for (i = 1; i < 256; ++i)
		skip[i] = 1;

	for (i = 1; i < 256; ++i)
	{	j = follows[i];
		while (j > 0 && dictLength[i] + dictLength[j] < 256)
		{	if (sfreq[i] < dictLength[j])	// Would increase size!
				break;
			found = ETrue;
			strcat(dict[i], dict[j]);
			dictLength[i] += dictLength[j];
			dictTotalLength += dictLength[j];
			skip[i] += skip[j];
			if (j < i)
				break;
			j = follows[j];
		}
	}

	if (!found)
		return EFalse;

	// Now perform all replacements - and note which codes are still used

	s = d = text;
	if (!freq_valid)
		while ((*d++ = *s) != 0)
		{	seen[*s] = ETrue;
			s += skip[*s];
		}
	else
		while ((*d++ = *s) != 0)
		{	seen[*s] = ETrue;
			if ((sk = skip[*s]) == 1)
				++s;
			else
			{	prev = *s;
				while (sk--)
				{	--freq[*s][s[1]];
					s++;
				}
				if (*s)
					++freq[prev][*s];
			}
		}

	textLength = d - text - 1;

	// Clear any unused codes

	for (i = 1; i < 256; ++i)
		if (!seen[i])
		{	dict[i][0] = '\0';
			dictTotalLength -= dictLength[i];
			dictLength[i] = 0;
		}

	return ETrue; 
}

void CCompressTcr::memset(TAny *ptr, TUint8 value, TInt len)
{
	while (len--)
		*(((TUint8 *&)ptr)++) = value;
}

void CCompressTcr::strcpy(TUint8 *dest, TUint8 *source)
{
	while (*source)
		*dest++ = *source++;
	*dest = 0;
}

void CCompressTcr::strcat(TUint8 *dest, TUint8 *source)
{
	while (*dest) ++dest;
	strcpy(dest, source);
}

TUint8 * CCompressTcr::strchr(TUint8 *str, TUint8 token)
{
	while (*str && (*str != token)) ++str;
	if (*str)
		return str;
	else
		return NULL;
}
