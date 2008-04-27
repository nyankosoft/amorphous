
#ifndef  __GM_DIALOGDESC_H__
#define  __GM_DIALOGDESC_H__


#include "GM_Dialog.h"


class CGM_TextureRectElement;


//========================================================================================
// CGM_DialogDesc
//========================================================================================

class CGM_DialogDesc
{

public:
	CGM_DialogDesc() { SetDefault(); }
//	~CGM_DialogDesc() {}

	void SetDefault();

    bool bCaption;

    float fPosX, fPosY;
	float fWidth, fHeight;

    int iCaptionHeight;

	CGM_TextureRectElement *pBackgroundRectElement;

	bool bNonUserEvents;

    PCALLBACK_GM_GUIEVENT pEventHandler;

	bool bRootDialog;

//	D3DCOLOR m_colorTopLeft;    D3DCOLOR m_colorTopRight;    D3DCOLOR m_colorBottomLeft;    D3DCOLOR m_colorBottomRight;


};


#endif		/*  __GM_DIALOGDESC_H__  */