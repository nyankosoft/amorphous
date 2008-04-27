
#include "GameItem.h"
#include "3DCommon/Font.h"
#include <string.h>

#include "Support/memory_helpers.h"

using namespace std;


CGameItem::~CGameItem()
{
}


bool CGameItem::LoadMeshObject()
{
	return m_MeshObjectContainer.m_MeshObjectHandle.Load();
}




//===========================================================================
// CGI_Binocular
//===========================================================================

#include "3DMath/MathMisc.h"
#include "3DCommon/Camera.h"
#include "GameCommon/CriticalDamping.h"
#include "Stage/PlayerInfo.h"
//#include "Stage/BE_PlayerShip.h"
#include "Stage/Stage.h"
#include "Stage/ScreenEffectManager.h"
#include "GameInput/3DActionCode.h"
#include "GameInput/InputHandler.h"


void CGI_Binocular::OnSelected()
{
	PLAYERINFO.AddActiveItem( this );
	PLAYERINFO.SetItemFocus( this );
}


bool CGI_Binocular::HandleInput( int input_code, int input_type, float fParam )
{
	switch( input_code )
	{
//	case GIC_WHEEL_UP:
	case ACTION_QMENU_UP:
		m_fTargetZoom += 0.2f;
		if( m_fMaxZoom < m_fTargetZoom )
			m_fTargetZoom = m_fMaxZoom;
		return true;

//	case GIC_WHEEL_DOWN:
	case ACTION_QMENU_DOWN:
		m_fTargetZoom -= 0.2f;
		if( m_fTargetZoom < 1.0f )
			m_fTargetZoom = 1.0f;
		return true;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			// reset zoom and release the binocular
			m_fTargetZoom = 1.0f;
			PLAYERINFO.ReleaseItemFocus();
			return true;
		}
		break;

//	case GIC_MOUSE_M:
//		break;
	}

	return false;
}


void CGI_Binocular::Update( float dt )
{
 	CCamera *pCamera = PLAYERINFO.GetCurrentPlayerBaseEntity()->GetCamera();
	if( !pCamera )
		return;

	m_fCurrentZoom = SmoothCD( m_fCurrentZoom, m_fTargetZoom, m_fZoomSpeed, 0.2f, dt );

	// change the field of view of the camera to do the fake zoom effect
	pCamera->SetFOV( 3.141592f / 3.0f / m_fCurrentZoom );

	static float s_fFocusDelay = 0.0f;
	if( 0.25f < m_fZoomSpeed )
	{
		s_fFocusDelay += ( m_fZoomSpeed - 0.25f ) * dt * 1.5f;
	}
	else
	{
		s_fFocusDelay -= 4.0f * dt;
	}

	Limit( s_fFocusDelay, 0.0f, 2.0f );

	CStageSharedPtr pStage = m_pStage.lock();

	if( pStage.get() )
	{
		CScreenEffectManager* pEffectManager = pStage->GetScreenEffectManager();
		pEffectManager->SetBlurEffect( s_fFocusDelay );
	}

	if( PLAYERINFO.GetFocusedItem() != this )
	{
		// player has deselected the binocular
		if( fabsf(m_fCurrentZoom - 1.0f) < 0.001f )
		{
			// release from the active item list
			m_fCurrentZoom = 1.0f;
			PLAYERINFO.ReleaseActiveItem( this );
		}
	}
}


void CGI_Binocular::RenderStatus( int index, CFontBase *pFont )
{
	D3DXVECTOR2 vPos = D3DXVECTOR2( 480.0f, 500.0f + index * 16 );

	char acStatus[32];
	sprintf( acStatus, "ZOOM: X%.2f", m_fCurrentZoom );
	pFont->DrawText( acStatus, vPos, 0xFFFFFFFF );
}


//void CGI_Binocular::OnSelected() {}
//bool CGI_Binocular::HandleInput( int input_code, int input_type, float fParam ) { return false; }
//void CGI_Binocular::Update( float dt ) {}
//void CGI_Binocular::RenderStatus( int index, CFontBase *pFont ) {}



//===========================================================================
// CGI_NightVision
//===========================================================================

#include "Task/GameTask_Stage.h"
#include "Stage/Stage.h"
#include "Stage/ScreenEffectManager.h"


void CGI_NightVision::OnSelected()
{
	if( !m_bEnabled )
	{
		// turn on the night vision
		m_bEnabled = true;

		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage.get() )
			pStage->GetScreenEffectManager()->RaiseEffectFlag( ScreenEffect::PseudoNightVision ); 

		PLAYERINFO.AddActiveItem( this );
		m_bActive = true;
	}
	else
	{
		// turn off the night vision
		m_bEnabled = false;

		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage.get() )
			pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PseudoNightVision ); 

//		PLAYERINFO.ReleaseActiveItem( this );
	}
}


void CGI_NightVision::Update( float dt )
{
	if( m_bEnabled )
	{
		m_fBatteryLeft -= dt;
		if( m_fBatteryLeft <= 0 )
		{
			m_fBatteryLeft = 0;
			m_bEnabled = false;

            CStageSharedPtr pStage = m_pStage.lock();
			if( pStage.get() )
				pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PseudoNightVision ); 
		}
	}
	else
	{
		if( m_fBatteryLeft < m_fMaxBatteryLife )
		{
			m_fBatteryLeft += dt * m_fChargeSpeed;
		}

		if( m_fMaxBatteryLife <= m_fBatteryLeft )
		{
			m_fBatteryLeft = m_fMaxBatteryLife;
			PLAYERINFO.ReleaseActiveItem( this );
			m_bActive = false;
			return;
		}
	}

}

void CGI_NightVision::RenderStatus( int index, CFontBase *pFont )
{
	D3DXVECTOR2 vPos = D3DXVECTOR2( 480.0f, 500.0f + index * 16 );

	char acStatus[32];
	sprintf( acStatus, "Night Vision: %d/%d", (int)m_fBatteryLeft, (int)m_fMaxBatteryLife );
	pFont->DrawText( acStatus, vPos, 0xFFFFFFFF );
}


//void CGI_NightVision::OnSelected() {}
//void CGI_NightVision::Update( float dt ) {}
//void CGI_NightVision::RenderStatus( int index, CFontBase *pFont ) {}


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
