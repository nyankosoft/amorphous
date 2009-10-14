#include "MotionPrimitiveViewer.h"

#include "boost/foreach.hpp"

#include "Input.hpp"
#include "3DMath/misc.hpp"
#include "GUI.hpp"
#include "MotionSynthesis.hpp"
#include "Support/memory_helpers.hpp"

using namespace std;
using namespace boost;


static const float g_fIndicatorHeight = 0.05f;


// Graphics.SetWorldTransform( Matrix34Identitiy() );
// Graphics.SetViewTransform( Matrix34Identitiy() );
// Graphics.SetLighting( true );
// Graphics.SetLight( index, light );

// Graphics.SetDepthBufferMode( DepthBuffer::Z )
// Graphics.SetDepthBufferMode( DepthBuffer::W )
// Graphics.SetDepthBufferMode( DepthBuffer::Disabled )

class Culling
{
	enum Mode
	{
		CCW,
		CW,
		None,
		NumModes
	};
};

// Graphics.SetCulling( Culling::Mode mode )

// Graphics.SetVertexFog( color, min_dist, max_dist )
// Graphics.SetPixelFog( color, min_dist, max_dist )

// Graphics.SetAlphaBlend( true );
// bool alpha_blend_enabled = Graphics.GetAlphaBlend();


using namespace std;
using namespace msynth;


class CMotionListBoxEventHandler : public CGM_ListBoxEventHandler
{
	CMotionPrimitiveViewer *m_pViewer;

public:

	CMotionListBoxEventHandler( CMotionPrimitiveViewer* pViewer ) : m_pViewer(pViewer) {}

//	virtual ~CGM_ListBoxEventHandler() {}

	virtual void OnItemSelected( CGM_ListBoxItem& item, int item_index ) { m_pViewer->OnItemSelected( item, item_index ); }

	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item ) {}
};


CMotionPrimitiveViewer::CMotionPrimitiveViewer()
:
m_pMotionPrimitiveListBox(NULL),
m_fCurrentPlayTime(0)
{
	m_pUnitCube = boost::shared_ptr<CUnitCube>( new CUnitCube() );
	m_pUnitCube->Init();
}


CMotionPrimitiveViewer::~CMotionPrimitiveViewer()
{
}


void CMotionPrimitiveViewer::Init()
{
	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager() );

	int dw = 240;
	int dh = grof(dw);
	CGM_Dialog *pDialog = m_pDialogManager->AddRootDialog( 
		ROOT_DIALOG,
		RectLTWH( 10, 10, dw, dh ),
		"motions"
		);

	if( !pDialog )
		return;

	m_pMotionPrimitiveListBox = pDialog->AddListBox(
		1,
		RectLTWH( 5, 5, dw - 10, dh - 10 ),
		"",
		LBX_MOTION_PRIMITIVES,
		16
		);

	// set event handler for the listbox
	CGM_ListBoxEventHandlerSharedPtr pEventHandler
		= CGM_ListBoxEventHandlerSharedPtr( new CMotionListBoxEventHandler( this ) );

	m_pMotionPrimitiveListBox->SetEventHandler( pEventHandler );

	// load font
	CGM_ControlRendererManagerSharedPtr pRendererMgr
		= m_pDialogManager->GetControlRendererManagerSharedPtr();

	shared_ptr<CGraphicsElementManager> pGraphicsElementMgr
		= pRendererMgr->GetGraphicsElementManager();

	pGraphicsElementMgr->LoadFont( 0, "Arial", CFontBase::FONTTYPE_NORMAL, 8, 16 );

	m_pInputHandler = CInputHandlerSharedPtr( new CGM_DialogInputHandler( m_pDialogManager ) );
	InputHub().SetInputHandler( 0, m_pInputHandler.get() );

	// set up guide geometry
	float h = g_fIndicatorHeight;
	m_DirectionGuide.AddLineSegment( Vector3(-100.0f, h,   0.0f), Vector3( 100.0f, h,  0.0f), 0xFFC0C0C0 );
	m_DirectionGuide.AddLineSegment( Vector3(0.0f,    h,-100.0f), Vector3( 0.0f,   h,100.0f), 0xFFF0F0F0 );
}


void CMotionPrimitiveViewer::Update( float dt )
{
	if( m_pDialogManager )
		m_pDialogManager->Update( dt );

	if( !m_pCurrentMotion )
		return;

	m_SkeletonRenderer.SetSkeleton( m_pCurrentMotion->GetSkeleton() );

	CKeyframe keyframe;
	m_pCurrentMotion->GetInterpolatedKeyframe( keyframe, m_fCurrentPlayTime );

	m_SkeletonRenderer.UpdateBonePoses(keyframe);

	m_fCurrentPlayTime += dt;
}


void CMotionPrimitiveViewer::RenderFloor()
{
//	m_pUnitCube->SetUniformColor( 0.7f, 0.7f, 0.7f, 0.5f );
	m_pUnitCube->SetUniformColor( 0.6f, 0.6f, 0.6f, 1.0f );

	Matrix34 pose = Matrix34Identity();
	D3DXMATRIX matScaling, matWorld;

	int x,z;
	for( z=-4; z<=4; z++ )
	{
		for( x=-4; x<=4; x++ )
		{
			// set world transform
			pose.vPosition = Vector3( x * 1.0f, 0.0f, z * 1.0f );
			pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
			D3DXMatrixScaling( &matScaling, 0.99f, 0.01f, 0.99f );
			matWorld = matScaling * matWorld;
			DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

			m_pUnitCube->Draw();
		}
	}

	m_DirectionGuide.Draw();
}


void CMotionPrimitiveViewer::RenderPoles()
{
//	m_pUnitCube->SetUniformColor( 0.7f, 0.7f, 0.7f, 0.5f );
	m_pUnitCube->SetUniformColor( 0.7f, 0.7f, 0.7f, 0.3f );

	Matrix34 pose = Matrix34Identity();
	D3DXMATRIX matScaling, matWorld;

	const float pole_height = 2.0f;

	int x,z;
	for( z=-1; z<=1; z++ )
	{
		for( x=-1; x<=1; x++ )
		{
			// set world transform
			pose.vPosition = Vector3( x * 2.0f, pole_height * 0.5f, z * 2.0f );
			pose.GetRowMajorMatrix44( (Scalar *)&matWorld );
			D3DXMatrixScaling( &matScaling, 0.02f, pole_height, 0.02f );
			matWorld = matScaling * matWorld;
			DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

			m_pUnitCube->Draw();
		}
	}
}


void CMotionPrimitiveViewer::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );

	RenderFloor();

	m_MotionTrace.Draw();

	m_KeyframeCoords.Draw();

	m_SkeletonRenderer.Render();

	RenderPoles();

	// render UI

	pd3dDevice->SetRenderState( D3DRS_ZENABLE,  FALSE );

	m_pDialogManager->Render();
}


void CMotionPrimitiveViewer::UpdateLinestrip()
{
	std::vector<Vector3> vecPoint;
	std::vector<CKeyframe>& vecKeyframe = m_pCurrentMotion->GetKeyframeBuffer();
	if( vecKeyframe.size() == 0 )
		return;

	vecPoint.reserve( vecKeyframe.size() );
	BOOST_FOREACH( const CKeyframe& keyframe, vecKeyframe )
	{
		Vector3 vRootPos = keyframe.GetRootPose().vPosition;
		vRootPos.y = g_fIndicatorHeight;

		vecPoint.push_back( vRootPos );
	}

	m_MotionTrace.SetPoints( vecPoint, 0xFFF0FF10 );

	// visualize local coordinates of start and end keyframe
	m_KeyframeCoords.Clear();
	vector<Matrix34> vecRootPose;
	vecRootPose.push_back( vecKeyframe.front().GetRootPose() );
	vecRootPose.push_back( vecKeyframe.back().GetRootPose() );
	vecRootPose[0].vPosition.y = g_fIndicatorHeight;
	vecRootPose[1].vPosition.y = g_fIndicatorHeight;
	GetCoordAxesDisplay( m_KeyframeCoords, vecRootPose, 0.2f );
}


void CMotionPrimitiveViewer::OnItemSelected( const CGM_ListBoxItem& item, int item_index )
{
	size_t i = 0;
	const size_t num = m_vecpMotionPrimitive.size();
	bool valid_motion_found = false;
	for( i=0; i<num; i++ )
	{
		if( item.GetText() == m_vecpMotionPrimitive[i]->GetName() )
		{
			valid_motion_found = true;
			break;
		}
	}

	if( !valid_motion_found )
		return;

	// update motion primitive

	m_pCurrentMotion = m_vecpMotionPrimitive[i];

	// reset play time
	m_fCurrentPlayTime = 0;

	// update linestrip
	UpdateLinestrip();
}


void CMotionPrimitiveViewer::LoadMotionPrimitivesFromDatabase( const std::string& filename, const std::string& motion_table_name )
{
	msynth::CMotionDatabase db;
	bool success = db.LoadFromFile( filename );

	if( !success )
	{
		return;
	}

	msynth::CHumanoidMotionTable tbl;

	db.GetHumanoidMotionTable( motion_table_name, tbl );
	
	BOOST_FOREACH( const msynth::CHumanoidMotionEntry& entry, tbl.m_vecEntry )
	{
		BOOST_FOREACH( const std::string& motion_name, entry.m_vecMotionPrimitiveName )
		{
			CMotionPrimitiveSharedPtr pMotion = db.GetMotionPrimitive( motion_name );

			m_vecpMotionPrimitive.push_back( pMotion );
		}
	}

	if( !m_pDialogManager )
		Init();

	if( m_pMotionPrimitiveListBox )
	{
		m_pMotionPrimitiveListBox->RemoveAllItems();

		BOOST_FOREACH( const CMotionPrimitiveSharedPtr pMotion, m_vecpMotionPrimitive )
		{
			m_pMotionPrimitiveListBox->AddItem( pMotion->GetName() );
		}
	}

	m_pDialogManager->OpenRootDialog( ROOT_DIALOG );
}
