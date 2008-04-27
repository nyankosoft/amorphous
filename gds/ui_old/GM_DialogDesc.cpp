
#include "GM_DialogDesc.h"

void CGM_DialogDesc::SetDefault()
{
    bCaption = false;

    fPosX  = fPosY   = 0.0f;
	fWidth = fHeight = 0.1f;

    iCaptionHeight = 0;

	pBackgroundRectElement = NULL;

	bNonUserEvents = false;

    pEventHandler = NULL;

	bRootDialog = false;

}

