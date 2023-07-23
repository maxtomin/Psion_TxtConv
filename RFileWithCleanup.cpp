// RFileWithCleanup.cpp: implementation of the RFileWithCleanup class.
//
//////////////////////////////////////////////////////////////////////

#include "RFileWithCleanup.h"

void RFileWithCleanup::Cleanup(TAny *aPtr)
	{
	  // Invoke the Close member on the RItem at aPtr
	((RFileWithCleanup *)aPtr)->Close();
	}

RFileWithCleanup::operator TCleanupItem()
	{
	return TCleanupItem(Cleanup,this);
	}