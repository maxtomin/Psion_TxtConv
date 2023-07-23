// CopyToOutConvertion.h: interface for the CCopyToOutConvertion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COPYTOOUTCONVERTION_H__64CDC1B4_ABE6_4F25_B98C_3141F828C49D__INCLUDED_)
#define AFX_COPYTOOUTCONVERTION_H__64CDC1B4_ABE6_4F25_B98C_3141F828C49D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TextConversion.h"

class CCopyToOutConvertion : public CTextConversion  
{
public:
	TUint8 Char;
	TBool DoStepL(TInt  &aFilePos);
	CCopyToOutConvertion();
	virtual ~CCopyToOutConvertion();

};

#endif // !defined(AFX_COPYTOOUTCONVERTION_H__64CDC1B4_ABE6_4F25_B98C_3141F828C49D__INCLUDED_)
