#ifndef  __GM_FLOWTEXTCAPTION_H__
#define  __GM_FLOWTEXTCAPTION_H__


#include "GM_CaptionRenderRoutine.hpp"

#include <string>
using namespace std;


class CGM_DialogManager;


class CGM_FlowTextCaption : public CGM_CaptionRenderRoutine
{

    CGM_DialogManager *m_pDialogManager;

	float m_fElapsedTime;

public:

	CGM_FlowTextCaption( CGM_DialogManager *pDialogManager ) { m_pDialogManager = pDialogManager; m_fElapsedTime = 0; }

	~CGM_FlowTextCaption() {}

	void CaptionControlChanged( CGM_Control *pNewControl );

	void Render( float fElapsedTime,
		         const string& strCaption,
				 const CGM_CaptionParameters& rCaptionParam );

};



#endif		/*  __GM_FLOWTEXTCAPTION_H__  */