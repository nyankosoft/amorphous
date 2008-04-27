
#include "GameItem.h"
//#include "3DCommon/D3DXStillObject.h"

#include <string.h>

#include "Support/memory_helpers.h"


CGameItem::~CGameItem()
{
//	SafeDelete( m_p3DModel );
}


bool CGameItem::Load3DModel()
{
	return false;
}



//===========================================================================
// CGI_Binocular
//===========================================================================

void CGI_Binocular::OnSelected()
{
}

bool CGI_Binocular::HandleInput( int input_code, int input_type, float fParam )
{
	return false;
}

void CGI_Binocular::Update( float dt )
{
}

void CGI_Binocular::RenderStatus( int index, CFontBase *pFont )
{
}


//===========================================================================
// CGI_NightVision
//===========================================================================

void CGI_NightVision::OnSelected()
{
}


void CGI_NightVision::Update( float dt )
{
}

void CGI_NightVision::RenderStatus( int index, CFontBase *pFont )
{
}


//===========================================================================
// CGI_CamouflageDevice
//===========================================================================

void CGI_CamouflageDevice::OnSelected()
{
}


//===========================================================================
// CGI_Suppressor
//===========================================================================

void CGI_Suppressor::OnSelected()
{
}


//===========================================================================
// CGI_Key
//===========================================================================

void CGI_Key::OnSelected()
{
}