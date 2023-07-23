// RFileWithCleanup.h: interface for the RFileWithCleanup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFILEWITHCLEANUP_H__34501A17_F62B_431F_9BE5_2DA70FC60F62__INCLUDED_)
#define AFX_RFILEWITHCLEANUP_H__34501A17_F62B_431F_9BE5_2DA70FC60F62__INCLUDED_

#include <f32file.h>

class RFileWithCleanup : public RFile  
{
private:
	static void Cleanup(TAny *aPtr);
public:
	operator TCleanupItem();
};

#endif // !defined(AFX_RFILEWITHCLEANUP_H__34501A17_F62B_431F_9BE5_2DA70FC60F62__INCLUDED_)
