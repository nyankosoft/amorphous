#include "GameItem.hpp"
#include "XML/XMLNodeReader.hpp"
#include "Support/memory_helpers.hpp"
#include <string.h>


using namespace std;
using namespace boost;


void ItemDesc::LoadFromXMLNode( CXMLNodeReader& reader )
{
	int lang_index = 0;
	CXMLNodeReader text_node_reader = reader.GetChild( "Text" );
	text_node_reader.GetChildElementTextContent( "Japanese", text[Lang::Japanese] );
	text_node_reader.GetChildElementTextContent( "English",  text[Lang::English] );
}


//=======================================================================
// CGameItem
//=======================================================================

CGameItem::CGameItem()
:
m_iCurrentQuantity(0),
m_iMaxQuantity(1),
m_Price(1),
m_TypeFlag(0)
{
	// create a mesh container in the root node
	// - used as a default mesh container
	shared_ptr<CMeshObjectContainer> pMeshContainer
		= shared_ptr<CMeshObjectContainer>( new CMeshObjectContainer() );

	m_MeshContainerRootNode.AddMeshContainer( pMeshContainer, Matrix34Identity() );
}


CGameItem::~CGameItem()
{
}


void CGameItem::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_strName;
	ar & m_MeshContainerRootNode;
	ar & m_iMaxQuantity;
	ar & m_iCurrentQuantity;
	ar & m_Price;
	ar & m_TypeFlag;

	ar & m_Desc;
}


bool CGameItem::LoadMeshObject()
{
//	return m_MeshObjectContainer.m_MeshObjectHandle.Load( m_MeshObjectContainer.m_MeshDesc );
	return m_MeshContainerRootNode.LoadMeshesFromDesc();
}


void CGameItem::Render()
{
	m_MeshContainerRootNode.Render( Matrix34Identity() );
}


void CGameItem::LoadFromXMLNode( CXMLNodeReader& reader )
{
	reader.GetChildElementTextContent( "Name",        m_strName );
	reader.GetChildElementTextContent( "Price",       m_Price );
	reader.GetChildElementTextContent( "MaxQuantity", m_iMaxQuantity );

//	m_MeshContainerRootNode.LoadFromXMLNode( reader.GetChild( "Model/MeshNode" ) );
	m_MeshContainerRootNode.LoadFromXMLNode( (reader.GetChild( "Model" )).GetChild( "MeshNode" ) );
	m_Desc.LoadFromXMLNode( reader.GetChild( "Desc" ) );
}



//===========================================================================
// CGI_Binocular
//===========================================================================

#include "3DMath/MathMisc.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/Camera.hpp"
#include "GameCommon/CriticalDamping.hpp"
#include "Stage/PlayerInfo.hpp"
#include "Stage/Stage.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Input/InputHandler.hpp"


CGI_Binocular::CGI_Binocular()
:
m_fCurrentZoom(1.0f),
m_fTargetZoom(1.0f),
m_fMaxZoom(2.0f),
m_fZoomSpeed(0.0f),
m_fMaxZoomSpeed(5.0f),
m_fFocusDelay(0.0f)
{
	m_TypeFlag = TYPE_UTILITY;
}


void CGI_Binocular::OnSelected()
{
	SinglePlayerInfo().AddActiveItem( this );
	SinglePlayerInfo().SetItemFocus( this );
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
			SinglePlayerInfo().ReleaseItemFocus();
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
 	CCamera *pCamera = SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetCamera();
	if( !pCamera )
		return;

	m_fCurrentZoom = SmoothCD( m_fCurrentZoom, m_fTargetZoom, m_fZoomSpeed, 0.2f, dt );

	// change the field of view of the camera to do the fake zoom effect
	pCamera->SetFOV( 3.141592f / 3.0f / m_fCurrentZoom );

	if( 0.25f < m_fZoomSpeed )
	{
		m_fFocusDelay += ( m_fZoomSpeed - 0.25f ) * dt * 1.5f;
	}
	else
	{
		m_fFocusDelay -= 4.0f * dt;
	}

	Limit( m_fFocusDelay, 0.0f, 2.0f );

	CStageSharedPtr pStage = m_pStage.lock();

	if( pStage.get() )
	{
		CScreenEffectManager* pEffectManager = pStage->GetScreenEffectManager();
		pEffectManager->SetBlurEffect( m_fFocusDelay );
	}

	if( SinglePlayerInfo().GetFocusedItem() != this )
	{
		// player has deselected the binocular
		if( fabsf(m_fCurrentZoom - 1.0f) < 0.001f )
		{
			// release from the active item list
			m_fCurrentZoom = 1.0f;
			SinglePlayerInfo().ReleaseActiveItem( this );
		}
	}
}


void CGI_Binocular::GetStatus( std::string& dest_buffer )
{
	char acStatus[32];
	sprintf( acStatus, "ZOOM: X%.2f", m_fCurrentZoom );
	dest_buffer = acStatus;
}


//void CGI_Binocular::OnSelected() {}
//bool CGI_Binocular::HandleInput( int input_code, int input_type, float fParam ) { return false; }
//void CGI_Binocular::Update( float dt ) {}
//void CGI_Binocular::RenderStatus( int index, CFontBase *pFont ) {}



//===========================================================================
// CGI_NightVision
//===========================================================================

#include "Task/GameTask_Stage.hpp"
#include "Stage/Stage.hpp"
#include "Stage/ScreenEffectManager.hpp"


void CGI_NightVision::OnSelected()
{
	if( !m_bEnabled )
	{
		// turn on the night vision
		m_bEnabled = true;

		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage.get() )
			pStage->GetScreenEffectManager()->RaiseEffectFlag( ScreenEffect::PseudoNightVision ); 

		SinglePlayerInfo().AddActiveItem( this );
		m_bActive = true;
	}
	else
	{
		// turn off the night vision
		m_bEnabled = false;

		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage.get() )
			pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PseudoNightVision ); 

//		SinglePlayerInfo().ReleaseActiveItem( this );
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
			SinglePlayerInfo().ReleaseActiveItem( this );
			m_bActive = false;
			return;
		}
	}

}

void CGI_NightVision::GetStatus( std::string& dest_buffer )
{
	char acStatus[32];
	sprintf( acStatus, "Night Vision: %d/%d", (int)m_fBatteryLeft, (int)m_fMaxBatteryLife );
	dest_buffer = acStatus;
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
