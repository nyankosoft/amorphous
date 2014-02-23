
#include "GM_DialogDesc.hpp"


namespace amorphous
{

using namespace std;


void CGM_DialogDesc::SetDefault()
{
	CGM_ControlDescBase::SetDefault();

    bCaption = false;

	Rect.SetPositionLTRB( 0, 0, 256, 256 );

    iCaptionHeight = 0;

	bNonUserEvents = false;

    pEventHandlerFn = NULL;

	bRootDialog = false;

	StyleFlag = 0;

	for( int i=0; i<GM_NUM_MAX_ADJACENT_DIALOGS; i++ )
		apNextDialog[i] = 0;

	strTitle = "";
}


} // namespace amorphous
