#ifndef  __GM_CAPTIONRENDERROUTINE_H__
#define  __GM_CAPTIONRENDERROUTINE_H__


#include "GM_CaptionParameters.h"

#include <string>
using namespace std;


class CGM_Control;


class CGM_CaptionRenderRoutine
{
public:

	CGM_CaptionRenderRoutine() {}

	~CGM_CaptionRenderRoutine() {}

	virtual void CaptionControlChanged( CGM_Control *pNewControl ) {}

	virtual void Render( float fElapsedTime,
		                 const string& strCaption,
						 const CGM_CaptionParameters& rCaptionParam ) = 0;

};



#endif		/*  __GM_CAPTIONRENDERROUTINE_H__  */