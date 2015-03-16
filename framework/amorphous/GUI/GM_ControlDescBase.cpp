
#include "GM_ControlDescBase.hpp"


namespace amorphous
{


void CGM_ControlDescBase::SetDefault()
{
	ID = -1;

	Rect = SRect( 0, 0, 2, 2 );
}


void CGM_ControlDesc::SetDefault()
{
	CGM_ControlDescBase::SetDefault();

	bEnabled   = true;
	bVisible   = true;
	bIsDefault = false;
	bHasFocus  = false;

	coord_type = COORD_LOCAL;

//	UserIndex = 0;
//	pUserData = nullptr;
}


void CGM_StaticDesc::SetDefault()
{
	Rect.SetPositionLTRB(0,0,15,15);

	CGM_ControlDesc::SetDefault();

	strText = "";
}


void CGM_ButtonDesc::SetDefault()
{
	CGM_StaticDesc::SetDefault();

	bPressed = false;
}


void CGM_CheckBoxDesc::SetDefault()
{
	CGM_ButtonDesc::SetDefault();

	bChecked = false;
}


void CGM_RadioButtonDesc::SetDefault()
{
	CGM_CheckBoxDesc::SetDefault();

	iButtonGroup = 0; 
}


void CGM_SubDialogButtonDesc::SetDefault()
{
	CGM_ButtonDesc::SetDefault();

	pSubDialog = nullptr;
}


void CGM_SliderDesc::SetDefault()
{
	CGM_ControlDesc::SetDefault();

	iMin = iMax = 0;
	iInitialValue = 0; 
}


} // namespace amorphous
