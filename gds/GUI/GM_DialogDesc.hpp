#ifndef  __GM_DIALOGDESC_H__
#define  __GM_DIALOGDESC_H__


#include "fwd.hpp"
#include "GM_ControlDescBase.hpp"
#include "Graphics/Rect.hpp"


//========================================================================================
// CGM_DialogDesc
//========================================================================================

class CGM_DialogDesc : public CGM_ControlDescBase
{
	enum eParams
	{
		GM_NUM_MAX_ADJACENT_DIALOGS = 4,
	};

public:

	CGM_DialogDesc() { SetDefault(); }

	virtual ~CGM_DialogDesc() {}

	void SetDefault();

public:

    bool bCaption;

    int iCaptionHeight;

	bool bNonUserEvents;

    CGM_DialogEventHandlerSharedPtr pEventHandler;

    PCALLBACK_GM_GUIEVENT pEventHandlerFn;

	bool bRootDialog;

	unsigned int StyleFlag;

	CGM_Dialog *apNextDialog[GM_NUM_MAX_ADJACENT_DIALOGS];

	std::string strTitle;

	CGM_DialogSoundPlayerSharedPtr pSoundPlayer;
};


#endif		/*  __GM_DIALOGDESC_H__  */
