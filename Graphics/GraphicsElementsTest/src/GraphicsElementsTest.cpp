#include "GraphicsElementsTest.hpp"
#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics.hpp>
#include <gds/Graphics/AsyncResourceLoader.hpp>
#include <gds/Support/Timer.hpp>
#include <gds/Support/Profile.hpp>
#include <gds/Support/Macro.h>
#include <gds/GUI.hpp>

using namespace std;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CGraphicsElementsTest();
}


extern const std::string GetAppTitle()
{
	return string("GraphicsElementsTest");
}


CGraphicsElementsTest::CGraphicsElementsTest()
{
	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	m_pGraphicsElementManager
		= shared_ptr<CGraphicsElementManager>( new CGraphicsElementManager() );
}


CGraphicsElementsTest::~CGraphicsElementsTest()
{
}


void CGraphicsElementsTest::CreateSampleUI()
{
}


int CGraphicsElementsTest::Init()
{
	m_pFont = shared_ptr<CFontBase>( new CFont( "ÇlÇr ÉSÉVÉbÉN", 6, 12 ) );
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );

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

	int left = 0, top = 0, w = 0, h = 0;

	shared_ptr<CGraphicsElementManager> pMgr = m_pGraphicsElementManager;

	int i = 0;
	w = 80;
	m_apRect[i++] = pMgr->CreateRect( RectAtLeftTop( w, grof(w), 50,       50 ), FRGBA::Red(),   FRGBA::Aqua(), 5 );
	m_apRect[i++] = pMgr->CreateRect( RectAtLeftTop( w, grof(w), 50 + 200, 50 ), FRGBA::Green(), FRGBA::Magenta(), 5 );
	m_apRect[i++] = pMgr->CreateRect( RectAtLeftTop( w, grof(w), 50 + 400, 50 ), FRGBA::Blue(),  FRGBA::Yellow(), 5 );

	// rotate around the center of the rect
	Vector2 vCenter = m_apRect[0]->GetAABB().GetCenterPosition();

	Matrix23 rect_pose
		= Matrix23( vCenter, Matrix22Identity() ) // last transform
		* Matrix23( Vector2(0,0), Matrix22Rotation(deg_to_rad(30)) )
		* Matrix23( -vCenter, Matrix22Identity() ); // first transform

	m_apRect[0]->SetLocalTransform( rect_pose );

//	m_apRect[0]->SetLocalRotationCenterPos( m_apRect[0]->GetAABB().GetCenterPosition() );
//	m_apRect[0]->SetRotationAngle( 30 );


	// triangles

	const SFloatRGBAColor white = FRGBA::White();

	Vector2 avVert[] = { Vector2(20,170), Vector2(80,120), Vector2(140,170) };
//	m_apTriangle[0] = pMgr->CreateTriangle( avVert, white );

	left = 20; top = 200;
	w = 60; h = 50;
	m_apTriangle[0] = pMgr->CreateTriangle( C2DTriangle::DIR_UP,    RectLTWH( left + (w+20) * 0, top, w, h ), FRGBA::Red(),    white,  3 );
	m_apTriangle[1] = pMgr->CreateTriangle( C2DTriangle::DIR_RIGHT, RectLTWH( left + (w+20) * 1, top, w, h ), FRGBA::Green(),  white,  3 );
	m_apTriangle[2] = pMgr->CreateTriangle( C2DTriangle::DIR_DOWN,  RectLTWH( left + (w+20) * 2, top, w, h ), FRGBA::Blue(),   white,  3 );
	m_apTriangle[3] = pMgr->CreateTriangle( C2DTriangle::DIR_LEFT,  RectLTWH( left + (w+20) * 3, top, w, h ), FRGBA::Yellow(), white,  3 );
}


void CGraphicsElementsTest::TestRotations()
{
}


void CGraphicsElementsTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );


	if( !GraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
		// async loading is not enabled
		// - The primary thread (this thread) loads the resources from the disk/memory.
		AsyncResourceLoader().ProcessResourceLoadRequest();
	}
}


void CGraphicsElementsTest::RenderGraphicsElements()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
}


void CGraphicsElementsTest::Render()
{
	PROFILE_FUNCTION();

	m_pGraphicsElementManager->Render();

	if( m_pSampleUI )
		m_pSampleUI->Render();
}


void CGraphicsElementsTest::HandleInput( const SInputData& input )
{
	if( m_pUIInputHandler )
	{
//		CInputHandler::ProcessInput() does not take const SInputData&
		SInputData input_copy = input;
		m_pUIInputHandler->ProcessInput( input_copy );

		if( m_pUIInputHandler->PrevInputProcessed() )
			return;
	}

	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
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
		break;
	}
}


void CGraphicsElementsTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CGraphicsElementsTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
