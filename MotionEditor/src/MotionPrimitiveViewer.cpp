#include "MotionPrimitiveViewer.h"

#include <map>
#include <boost/foreach.hpp>
#include <gds/Input.hpp>
#include <gds/3DMath/misc.hpp>
#include <gds/GUI.hpp>
#include <gds/MotionSynthesis.hpp>
//#include <gds/Support/CameraController.hpp>
#include <gds/Support/memory_helpers.hpp>
#include <gds/Support/ParamLoader.hpp>

using namespace std;
using namespace boost;


class CDebugInputHandler : public CInputHandler
{
	CMotionPrimitiveViewer *m_pViewer;
public:
	CDebugInputHandler( CMotionPrimitiveViewer *pViewer ) : m_pViewer(pViewer) {}
	void ProcessInput(SInputData& input)
	{
		switch( input.iGICode )
		{
		case '0':
//			if( input.iType == ITYPE_KEY_PRESSED )
//				m_pViewer->ToggleDisplayHelpText();
			break;
		case '2':
//			if( input.iType == ITYPE_KEY_PRESSED )
//				m_pViewer->ToggleDisplaySkeleton();
			break;
		case '3':
			if( input.iType == ITYPE_KEY_PRESSED )
				m_pViewer->ToggleDisplaySkeletalMesh();
			break;
		default:
			break;
		}
	}
};


extern int ConvertGICodeToWin32VKCode( int general_input_code );

inline bool IsKeyPressed( int general_input_code )
{
	if( !IsValidGeneralInputCode( general_input_code ) )
		return false;

	return ( ( GetAsyncKeyState( ConvertGICodeToWin32VKCode(general_input_code) ) & 0x8000 ) != 0 );
}


static const float g_fIndicatorHeight = 0.05f;

//extern CCameraController g_CameraController;

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

/*
class CTransformNodeToMeshBoneMap
{
	std::vector< std::pair<int,int> > m_NodeToMeshBoneMatIndexMaps;

public:

	void Create_r( const CBone& bone, CMM_Bone& mesh_bone, CSkeletalMesh& mesh )
	{
		for( int i=0; i<bone.GetNumChildren(); i++ )
		{
//			for( int j=0; j<mesh_bone.GetNumChildren(); j++ )
//			{
//				if( bone.GetChild(i).GetName() == mesh_bone.
//			}

			int mat_index = mesh.GetBoneMatrixIndexByName( bone.GetChild(i).GetName() );
			if( 0 <= mat_index )
			{
				m_NodeToMeshBoneMatIndexMaps.push_back( std::pair<int,int>( i, mat_index ) );
			}
		}
	}

	void Create( CSkeleton& src_skeleton, CSkeletalMesh& mesh )
	{
		const CBone& root_bone = src_skeleton.GetRootBone();
		CMM_Bone& mesh_bone = mesh.GetBone( root_bone.GetName() );

		if( mesh_bone != mesh.GetRootBone() )
			return;

		if( mesh_bone == CMM_Bone::NullBone() )
			return;

		Create_r( root_bone, mesh_bone, mesh );
	}
};
*/



CSkeletalMeshMotionViewer::CSkeletalMeshMotionViewer()
:
m_UseQuaternionForBoneTransformation(false)
{
	int use_quaternion = LoadParamFromFile<int>( "config", "UseQuaternionForVertexBlending" );
	m_UseQuaternionForBoneTransformation = (use_quaternion == 1);
}


void CSkeletalMeshMotionViewer::Init()
{
	LoadSkeletalMesh( "models/male_skinny_young.msh" );

	string shader_filepath;
	if( m_UseQuaternionForBoneTransformation )
		shader_filepath = "shaders/QVertexBlend.fx";
	else
		shader_filepath = "shaders/VertexBlend.fx";

	bool loaded = m_Shader.Load( shader_filepath );
//	bool loaded = m_Shader.Load( "shaders/VertexBlend.fx:VertBlend_PVL_1HSDL" );
	if( !loaded )
	{
		return;
	}

	m_Technique.SetTechniqueName( "VertBlend_PVL_1HSDL" );

	shared_ptr<CShaderLightManager> pLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager();
	if( pLightMgr )
	{
		CHemisphericDirectionalLight hsdl;
		hsdl.Attribute.UpperDiffuseColor = SFloatRGBAColor( 0.6f, 0.6f, 0.6f, 1.0f );
		hsdl.Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.3f, 0.3f, 0.3f, 1.0f );
		hsdl.fIntensity = 1.0f;
		hsdl.vDirection = Vec3GetNormalized( Vector3(-1,-1,-1) );
		pLightMgr->SetHemisphericDirectionalLight( hsdl );
		pLightMgr->CommitChanges();
	}
}


void CSkeletalMeshMotionViewer::LoadSkeletalMesh( const std::string& mesh_path )
{
	CMeshResourceDesc desc;
	desc.ResourcePath = mesh_path;
	desc.MeshType = CMeshType::SKELETAL;

	bool loaded = m_SkeletalMesh.Load( desc );
	if( !loaded )
	{
		return;
	}
}


shared_ptr<CSkeletalMesh> GetSkeletalMesh( CMeshObjectHandle& mesh_handle )
{
	shared_ptr<CBasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh )
		return shared_ptr<CSkeletalMesh>();

	shared_ptr<CSkeletalMesh> pSkeletalMesh
		= boost::dynamic_pointer_cast<CSkeletalMesh,CBasicMesh>( pMesh );

	return pSkeletalMesh;
}


void CSkeletalMeshMotionViewer::UpdateVertexBlendTransforms( CShaderManager& shader_mgr, CSkeletalMesh& skeletal_mesh )
{
	if( !shader_mgr.GetEffect() )
		return;

	vector<Transform> vert_blend_transforms;
	skeletal_mesh.GetBlendTransforms( vert_blend_transforms );
	shader_mgr.SetVertexBlendTransforms( vert_blend_transforms );
}


void CSkeletalMeshMotionViewer::UpdateVertexBlendMatrices( CShaderManager& shader_mgr, CSkeletalMesh& skeletal_mesh )
{
	if( !shader_mgr.GetEffect() )
		return;

	HRESULT hr = S_OK;

	skeletal_mesh.SetLocalTransformsFromCache();

	int num_max_matrices = 36;
	int num_bones = skeletal_mesh.GetNumBones();
	int num_matrices = take_min( num_bones, num_max_matrices );
	char acParam[32];
	D3DXMATRIX *paBlendMatrix = skeletal_mesh.GetBlendMatrices();
	for( int i=0; i<num_matrices; i++ )
	{
		sprintf( acParam, "g_aBlendMatrix[%d]", i );
		hr = shader_mgr.GetEffect()->SetMatrix( acParam, &paBlendMatrix[i] );

		if( FAILED(hr) )
			return;
	}
}


void CSkeletalMeshMotionViewer::Render()
{
	shared_ptr<CSkeletalMesh> pSkeletalMesh = GetSkeletalMesh( m_SkeletalMesh );
	if( !pSkeletalMesh )
		return;

//	GraphicsDevice().SetWorldTransform( Matrix34Identity() );
	FixedFunctionPipelineManager().SetWorldTransform( Matrix34Identity() );

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr && pShaderMgr->GetEffect() )
	{
		HRESULT hr = pShaderMgr->GetEffect()->SetFloatArray( "g_vEyePos", (float *)m_ViewerPose.vPosition, 3 );

		pShaderMgr->SetWorldTransform( Matrix34Identity() );
		pShaderMgr->SetTechnique( m_Technique );

		if( m_UseQuaternionForBoneTransformation )
			UpdateVertexBlendTransforms( *pShaderMgr, *pSkeletalMesh );
		else
			UpdateVertexBlendMatrices( *pShaderMgr, *pSkeletalMesh );

		pSkeletalMesh->Render( *pShaderMgr );
	}
	else
	{
		// How to do vertex blending in fixed function pipeline?
		pSkeletalMesh->Render();
	}
}


extern int g_htrans_rev;

/// NOTE: param 'bone' is used to calculate num_child_bones
void CSkeletalMeshMotionViewer::Update_r( const msynth::CBone& bone,
                                          const msynth::CTransformNode& node,
										  boost::shared_ptr<CSkeletalMesh>& pMesh )//,
//										  CMM_Bone& mesh_bone )
{
	// find the matrix index from the bone name (slow).
	int index = pMesh->GetBoneMatrixIndexByName( bone.GetName() );
	if( index == -1 )
		return;

	if( g_htrans_rev == 2 || g_htrans_rev == 3 )
	{
		Matrix34 local_transform;
		local_transform.vPosition = node.GetLocalTranslation();
		local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix();
		pMesh->SetLocalTransformToCache( index, local_transform );
	}
/*	else if( g_htrans_rev == 3 )
	{
		Matrix34 local_transform;
		local_transform.vPosition = node.GetLocalTranslation();
		local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix() * bone.GetOrient();
		pMesh->SetLocalTransformToCache( index, local_transform );
	}*/

	const int num_child_bones = bone.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
	for( int i=0; i<num_children; i++ )
	{
		Update_r(
			bone.GetChild(i),
			node.GetChildNode(i),
			pMesh );//,
//			mesh
	}

/*	for( int i=0; i<num_child_bones; i++ )
	{
		for( int j=0; j<num_child_nodes; j++ )
			bone.GetImmediateChild( node.Get );
	}*/
}


void CSkeletalMeshMotionViewer::Update( const msynth::CKeyframe& keyframe )
{
	shared_ptr<CSkeletalMesh> pSkeletalMesh = GetSkeletalMesh( m_SkeletalMesh );
	if( !pSkeletalMesh )
		return;

	shared_ptr<CSkeleton> pSkeleton = m_pSkeleton.lock();
	if( !pSkeleton )
		return;

	Update_r( pSkeleton->GetRootBone(), keyframe.GetRootNode(), pSkeletalMesh );
}


/*
class CRootDialogEventHandler : public CGM_EventHandlerBase
{
public:

	void HandleEvent( CGM_Event &event )
	{
		if( !event.pControl )
			return;

		switch( event.pControl->GetID() )
		{
		case ID_BUTTON1:
			if( event.Type == CGM_Event::BUTTON_CLICKED )
			{
			}
			break;
	}
};
*/

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
m_fCurrentPlayTime(0),
m_RenderMesh(true),
m_fPlaySpeedFactor(1.0f),
m_DisplaySkeletalMesh(true)
{
	m_pUnitCube = boost::shared_ptr<CUnitCube>( new CUnitCube() );
	m_pUnitCube->Init();

	m_MeshViewer.Init();
}


CMotionPrimitiveViewer::~CMotionPrimitiveViewer()
{
}


void CMotionPrimitiveViewer::Init()
{
	m_pDialogManager = CGM_DialogManagerSharedPtr( new CGM_DialogManager() );

	int dw = 400;
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

//	pGraphicsElementMgr->LoadFont( 0, "Arial", CFontBase::FONTTYPE_NORMAL, 8, 16 );
	pGraphicsElementMgr->LoadFont( 0, "BuiltinFont::BitstreamVeraSansMono_Bold_256", CFontBase::FONTTYPE_TEXTURE, 8, 16, 0, 0 );

	m_pInputHandler = shared_ptr<CInputHandler>( new CGM_DialogInputHandler( m_pDialogManager ) );
	InputHub().SetInputHandler( 0, m_pInputHandler.get() );

	// set up guide geometry
	float h = g_fIndicatorHeight;
	m_DirectionGuide.AddLineSegment( Vector3(-100.0f, h,   0.0f), Vector3( 100.0f, h,  0.0f), 0xFFC0C0C0 );
	m_DirectionGuide.AddLineSegment( Vector3(0.0f,    h,-100.0f), Vector3( 0.0f,   h,100.0f), 0xFFF0F0F0 );

	// input handler for display options
	m_pDebugInputHandler = shared_ptr<CInputHandler>( new CDebugInputHandler( this ) );
	InputHub().SetInputHandler( 1, m_pDebugInputHandler.get() );
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

	m_MeshViewer.SetSkeleton( m_pCurrentMotion->GetSkeleton() );
	m_MeshViewer.Update( keyframe );

	m_fPlaySpeedFactor = 0.0f;
	if( false )//mode == Play
	{
		m_fPlaySpeedFactor = 1.0f;
	}
	else
	{
		if( IsKeyPressed( GIC_RIGHT ) )
			m_fPlaySpeedFactor =  1.0f;
		else if( IsKeyPressed( GIC_LEFT ) )
			m_fPlaySpeedFactor = -1.0f;
		else
			m_fPlaySpeedFactor =  0.0f;
	}

	m_fCurrentPlayTime += dt * m_fPlaySpeedFactor;
	clamp( m_fCurrentPlayTime, 0.0f, 50.0f );
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

	if( m_DisplaySkeletalMesh )
		m_MeshViewer.Render();

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


int CMotionPrimitiveViewer::LoadMotionPrimitivesFromDatabase( const std::string& filename, const std::string& motion_table_name )
{
	msynth::CMotionDatabase db;
	bool success = db.LoadFromFile( filename );

	if( !success )
	{
		return -1;
	}

	msynth::CHumanoidMotionTable tbl;

	db.GetHumanoidMotionTable( motion_table_name, tbl );
	
	BOOST_FOREACH( const msynth::CHumanoidMotionEntry& entry, tbl.m_vecEntry )
	{
		BOOST_FOREACH( const std::string& motion_name, entry.m_vecMotionPrimitiveName )
		{
			shared_ptr<CMotionPrimitive> pMotion = db.GetMotionPrimitive( motion_name );

			m_vecpMotionPrimitive.push_back( pMotion );
		}
	}

	if( !m_pDialogManager )
		Init();

	if( m_pMotionPrimitiveListBox )
	{
		m_pMotionPrimitiveListBox->RemoveAllItems();

		BOOST_FOREACH( const shared_ptr<CMotionPrimitive> pMotion, m_vecpMotionPrimitive )
		{
			m_pMotionPrimitiveListBox->AddItem( pMotion->GetName() );
		}
	}

	m_pDialogManager->OpenRootDialog( ROOT_DIALOG );

	return 0;
}
