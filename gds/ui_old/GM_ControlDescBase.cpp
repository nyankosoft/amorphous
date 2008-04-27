
#include "GM_ControlDescBase.h"

CGM_ControlDesc::CGM_ControlDesc()
{
	SetDefault();
}


CGM_ControlDesc::~CGM_ControlDesc()
{
}


void CGM_ControlDesc::SetDefault()
{
	ID = -1;

	fPosX = fPosY = 0.0f;
	fWidth = fHeight = 0.1f;

	bEnabled   = true;
	bVisible   = true;
	bIsDefault = false;
	bHasFocus  = false;

}


void CGM_StaticDesc::SetDefault()
{
	CGM_ControlDesc::SetDefault();

	strText = "";

	pDefaultFontElement = NULL;
	pDefaultRectElement = NULL;
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

	iButtonGroup = -1; 
}


void CGM_SubDialogButtonDesc::SetDefault()
{
	CGM_ButtonDesc::SetDefault();

	pSubDialog = NULL;
}


void CGM_SliderDesc::SetDefault()
{
	CGM_ControlDesc::SetDefault();

	iMin = iMax = 0;
	iInitialValue = 0; 
	pRectElement = NULL; 
	pButtonElement = NULL; 
}