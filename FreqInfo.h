// FreqInfo.h: interface for the CFreqInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREQINFO_H__E2572177_71A3_4119_81E8_9D0BD56B9F9C__INCLUDED_)
#define AFX_FREQINFO_H__E2572177_71A3_4119_81E8_9D0BD56B9F9C__INCLUDED_

#include <e32def.h>

class CFreqInfo
{
public:
	TInt16 iPrevChar, iNextChar;
	TInt iFreq;
	CFreqInfo(TUint8 aPrevChar, TUint8 aNextChar) :
		iPrevChar(aPrevChar),
		iNextChar(aNextChar),
		iFreq(1) {}
	CFreqInfo() {CFreqInfo(0, 0);}
};

#endif // !defined(AFX_FREQINFO_H__E2572177_71A3_4119_81E8_9D0BD56B9F9C__INCLUDED_)
