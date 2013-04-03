#include "GraphicsElementsTest.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Input.hpp"

using std::string;
using namespace boost;


CGraphicsElementsTest::CGraphicsElementsTest()
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_pGraphicsElementManager.reset( new GraphicsElementManager() );
}


CGraphicsElementsTest::~CGraphicsElementsTest()
{
}


void CGraphicsElementsTest::CreateSampleUI()
{
}


int CGraphicsElementsTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

/*
	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	CreateGraphicsElements();

	TestRotations();

	return 0;
}


void CGraphicsElementsTest::CreateGraphicsElements()
{
	typedef SFloatRGBAColor FRGBA;
	typedef GraphicsComponent gc;

	int left = 0, top = 0, w = 0, h = 0;

	shared_ptr<GraphicsElementManager> pMgr = m_pGraphicsElementManager;

	if( !pMgr )
		return;

	int i = 0;
	w = 80;
	int border_widths[] = { 1, 3, 5, 10, 15, 20 };
	int index = 0;
	m_pRects.resize( 3 * numof(border_widths) );
	for( int i=0; i<numof(border_widths); i++ )
	{
		int y = 50 + (grof(w) + 20) * i;
		int border_width = border_widths[i];
		m_pRects[index++] = pMgr->CreateRect( gc::RectAtLeftTop( w, grof(w), 50,       y ), FRGBA::Red(),   FRGBA::Aqua(),    border_width );
		m_pRects[index++] = pMgr->CreateRect( gc::RectAtLeftTop( w, grof(w), 50 + 200, y ), FRGBA::Green(), FRGBA::Magenta(), border_width );
		m_pRects[index++] = pMgr->CreateRect( gc::RectAtLeftTop( w, grof(w), 50 + 400, y ), FRGBA::Blue(),  FRGBA::Yellow(),  border_width );
	}

	index = 0;
	m_pRoundRects.resize( 3 * numof(border_widths) );
	for( int i=0; i<numof(border_widths); i++ )
	{
		int y = 50 + (grof(w) + 20) * i;
		int border_width = border_widths[i];
//		float corner_radius = 10;
		m_pRoundRects[index++] = pMgr->CreateRoundRect( gc::RectAtRightTop( w, grof(w), 50,       y ), FRGBA::Red(),   FRGBA::Aqua(),     5, border_width );
		m_pRoundRects[index++] = pMgr->CreateRoundRect( gc::RectAtRightTop( w, grof(w), 50 + 200, y ), FRGBA::Green(), FRGBA::Magenta(), 10, border_width );
		m_pRoundRects[index++] = pMgr->CreateRoundRect( gc::RectAtRightTop( w, grof(w), 50 + 400, y ), FRGBA::Blue(),  FRGBA::Yellow(),  15, border_width );
	}

	// rotate around the center of the rect
	Vector2 vCenter = m_pRects[0]->GetAABB().GetCenterPosition();

	Matrix23 rect_pose
		= Matrix23( vCenter, Matrix22Identity() ) // last transform
		* Matrix23( Vector2(0,0), Matrix22Rotation(deg_to_rad(30)) )
		* Matrix23( -vCenter, Matrix22Identity() ); // first transform

	m_pRects[0]->SetLocalTransform( rect_pose );

//	m_pRects[0]->SetLocalRotationCenterPos( m_pRects[0]->GetAABB().GetCenterPosition() );
//	m_pRects[0]->SetRotationAngle( 30 );


	// triangles

	const SFloatRGBAColor white = FRGBA::White();

	Vector2 avVert[] = { Vector2(20,170), Vector2(80,120), Vector2(140,170) };
//	m_apTriangle[0] = pMgr->CreateTriangle( avVert, white );

	left = 20; top = 1000;
	w = 60; h = 50;
	m_apTriangle[0] = pMgr->CreateTriangle( C2DTriangle::DIR_UP,    RectLTWH( left + (w+20) * 0, top, w, h ), FRGBA::Red(),    white,  3 );
	m_apTriangle[1] = pMgr->CreateTriangle( C2DTriangle::DIR_RIGHT, RectLTWH( left + (w+20) * 1, top, w, h ), FRGBA::Green(),  white,  3 );
	m_apTriangle[2] = pMgr->CreateTriangle( C2DTriangle::DIR_DOWN,  RectLTWH( left + (w+20) * 2, top, w, h ), FRGBA::Blue(),   white,  3 );
	m_apTriangle[3] = pMgr->CreateTriangle( C2DTriangle::DIR_LEFT,  RectLTWH( left + (w+20) * 3, top, w, h ), FRGBA::Yellow(), white,  3 );
}


void CGraphicsElementsTest::ReleaseGraphicsElements()
{
	if( !m_pGraphicsElementManager )
		return;

	for( size_t i=0; i<m_pRects.size(); i++ )
	{
		m_pGraphicsElementManager->RemoveElement( m_pRects[i] );
		m_pRects[i].reset();
	}
	m_pRects.resize( 0 );

	for( size_t i=0; i<m_pRoundRects.size(); i++ )
	{
		m_pGraphicsElementManager->RemoveElement( m_pRoundRects[i] );
		m_pRoundRects[i].reset();
	}
	m_pRoundRects.resize( 0 );

	const int num_trianlges = numof(m_apTriangle);
	for( int i=0; i<num_trianlges; i++ )
	{
		m_pGraphicsElementManager->RemoveElement( m_apTriangle[i] );
		m_apTriangle[i].reset();
	}
}


void CGraphicsElementsTest::ReleaseAllGraphicsElements()
{
	if( !m_pGraphicsElementManager )
		return;

	m_pGraphicsElementManager->RemoveAllElements();
}


void CGraphicsElementsTest::TestRotations()
{
}


void CGraphicsElementsTest::Update( float dt )
{
//	if( m_pSampleUI )
//		m_pSampleUI->Update( dt );
}


void CGraphicsElementsTest::RenderGraphicsElements()
{
}


void CGraphicsElementsTest::Render()
{
	PROFILE_FUNCTION();

	if( m_pGraphicsElementManager )
		m_pGraphicsElementManager->Render();

//	if( m_pSampleUI )
//		m_pSampleUI->Render();
}


void CGraphicsElementsTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'R':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			ReleaseGraphicsElements();
		}
		break;

	case 'C':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			CreateGraphicsElements();
		}
		break;

	case 'X':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			ReleaseAllGraphicsElements();
		}
		break;

	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}


void CGraphicsElementsTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CGraphicsElementsTest::LoadGraphicsResources( const GraphicsParameters& rParam )
{
//	CreateSampleUI();
}
