#include "MotionPrimitiveViewer.h"

#include <map>
#include <boost/foreach.hpp>
#include "amorphous/Input.hpp"
#include "amorphous/3DMath/misc.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/MeshUtilities.hpp"
#include "amorphous/GUI.hpp"
#include "amorphous/MotionSynthesis.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/FileOpenDialog_Win32.hpp"

using namespace boost;


static const float g_fIndicatorHeight = 0.05f;

//extern CCameraController g_CameraController;

// Graphics.SetDepthBufferMode( DepthBuffer::Z )
// Graphics.SetDepthBufferMode( DepthBuffer::W )
// Graphics.SetDepthBufferMode( DepthBuffer::Disabled )


// Graphics.SetVertexFog( color, min_dist, max_dist )
// Graphics.SetPixelFog( color, min_dist, max_dist )



using std::string;
using std::vector;
using namespace msynth;

/*
class CTransformNodeToMeshBoneMap
{
	std::vector< std::pair<int,int> > m_NodeToMeshBoneMatIndexMaps;

public:

	void Create_r( const CBone& bone, CMM_Bone& mesh_bone, SkeletalMesh& mesh )
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

	void Create( CSkeleton& src_skeleton, SkeletalMesh& mesh )
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


static CLineList sg_RotationIndicator;



void DisplayLocalRotations_r( const CBone& bone, const CTransformNode& node, Matrix34& parent_transform )
{
//	Vector3 vWorldPos = vParentPos + bone.GetOrient() * bone.GetOffset();

	Matrix34 world_transform;
	bone.CalculateWorldTransform( world_transform, parent_transform, node );

	Vector3 vParentPos = parent_transform.vPosition;
	Vector3 vWorldPos  = world_transform.vPosition;
	sg_RotationIndicator.AddLineSegment( vParentPos, vWorldPos, SFloatRGBAColor::White().GetARGB32() );
	Quaternion q( node.GetLocalRotationQuaternion() );
	Vector3 vRotationIndicator( q.x, q.y, q.z );
	vRotationIndicator *= q.w;
	sg_RotationIndicator.AddLineSegment( vWorldPos, vWorldPos + vRotationIndicator, SFloatRGBAColor::Magenta().GetARGB32() );

	const int num_children = take_min( bone.GetNumChildren(), node.GetNumChildren() );
	for( int i=0; i<num_children; i++ )
	{
		DisplayLocalRotations_r( bone.GetChild(i), node.GetChildNode(i), world_transform );
	}
}


void UpdateLocalRotationIndicators( const CSkeleton& skeleton, const CKeyframe& keyframe )
{
	Vector3 vRootPos( 2, 0, 0 );
	Matrix34 root_pose( vRootPos, Matrix33Identity() );
	sg_RotationIndicator.Clear();
	DisplayLocalRotations_r( skeleton.GetRootBone(), keyframe.GetRootNode(), root_pose );
}


void DisplayLocalRotationIndicators()
{
	sg_RotationIndicator.Draw();
}


SkeletalMeshMotionViewer::SkeletalMeshMotionViewer()
:
m_UseQuaternionForBoneTransformation(false)
{
	int use_quaternion = LoadParamFromFile<int>( "config", "UseQuaternionForVertexBlending" );
	m_UseQuaternionForBoneTransformation = (use_quaternion == 1);
}


void SkeletalMeshMotionViewer::Init()
{
	string mesh_filepath( "models/male_skinny_young.msh" );
	LoadParamFromFile<string>( "params.txt", "Model", mesh_filepath );
	LoadSkeletalMesh( mesh_filepath );

	ShaderResourceDesc shader_desc;
	int use_embedded_shader = 1;
//	LoadParamFromFile<string>( "config", "UseEmbeddedShader", mesh_filepath );
	if( use_embedded_shader )
	{
		GenericShaderDesc desc;
		desc.Specular = SpecularSource::DECAL_TEX_ALPHA;
		desc.VertexBlendType = CVertexBlendType::QUATERNION_AND_VECTOR3;
//		desc.ShaderLightingType = ShaderLightingType::PER_PIXEL;
//		desc.Specular = CSpecularSource::NONE;

		shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(desc) );
	}
	else
	{
		string shader_filepath;
		if( m_UseQuaternionForBoneTransformation )
			shader_filepath = "shaders/QVertexBlend.fx";
		else
			shader_filepath = "shaders/VertexBlend.fx";

		shader_desc.ResourcePath = shader_filepath;
	}

	bool loaded = m_Shader.Load( shader_desc );
//	bool loaded = m_Shader.Load( "shaders/VertexBlend.fx:VertBlend_PVL_1HSDL" );
	if( !loaded )
	{
		return;
	}

	m_Technique.SetTechniqueName( "VertBlend_PVL_1HSDL" );

	shared_ptr<ShaderLightManager> pLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager();
	if( pLightMgr )
	{
		HemisphericDirectionalLight hsdl;
		hsdl.Attribute.UpperDiffuseColor = SFloatRGBAColor( 0.6f, 0.6f, 0.6f, 1.0f );
		hsdl.Attribute.LowerDiffuseColor = SFloatRGBAColor( 0.3f, 0.3f, 0.3f, 1.0f );
		hsdl.fIntensity = 1.0f;
		hsdl.vDirection = Vec3GetNormalized( Vector3(-1,-1,-1) );
		pLightMgr->SetHemisphericDirectionalLight( hsdl );
		pLightMgr->CommitChanges();
	}
}


void SkeletalMeshMotionViewer::LoadSkeletalMesh( const std::string& mesh_path )
{
	MeshResourceDesc desc;
	desc.ResourcePath = mesh_path;
	desc.MeshType = MeshType::SKELETAL;

	bool loaded = m_SkeletalMesh.Load( desc );
	if( !loaded )
	{
		return;
	}
}


shared_ptr<SkeletalMesh> GetSkeletalMesh( MeshHandle& mesh_handle )
{
	shared_ptr<BasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh )
		return shared_ptr<SkeletalMesh>();

	shared_ptr<SkeletalMesh> pSkeletalMesh
		= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( pMesh );

	return pSkeletalMesh;
}


void SkeletalMeshMotionViewer::UpdateVertexBlendTransforms( ShaderManager& shader_mgr, SkeletalMesh& skeletal_mesh )
{
	if( !shader_mgr.GetEffect() )
		return;

//	skeletal_mesh.SetLocalTransformsFromCache();
	skeletal_mesh.CalculateBlendTransformsFromCachedLocalTransforms();

	vector<Transform> vert_blend_transforms;
	skeletal_mesh.GetBlendTransforms( vert_blend_transforms );
	shader_mgr.SetVertexBlendTransforms( vert_blend_transforms );
}


void SkeletalMeshMotionViewer::UpdateVertexBlendMatrices( ShaderManager& shader_mgr, SkeletalMesh& skeletal_mesh )
{
	if( !shader_mgr.GetEffect() )
		return;

	HRESULT hr = S_OK;

//	skeletal_mesh.SetLocalTransformsFromCache();
	skeletal_mesh.CalculateBlendTransformsFromCachedLocalTransforms();

	vector<Transform> blend_transforms;
	skeletal_mesh.GetBlendTransforms( blend_transforms );
	shader_mgr.SetVertexBlendTransforms( blend_transforms );
/*
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
	}*/
}


void SkeletalMeshMotionViewer::Render()
{
	shared_ptr<SkeletalMesh> pSkeletalMesh = GetSkeletalMesh( m_SkeletalMesh );
	if( !pSkeletalMesh )
		return;

//	GraphicsDevice().SetWorldTransform( Matrix34Identity() );
	FixedFunctionPipelineManager().SetWorldTransform( Matrix34Identity() );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )//&& pShaderMgr->GetEffect() )
	{
//		HRESULT hr = pShaderMgr->GetEffect()->SetFloatArray( "g_vEyePos", (float *)m_ViewerPose.vPosition, 3 );
		pShaderMgr->SetParam( "g_vEyePos", m_ViewerPose.vPosition );

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

	DisplayLocalRotationIndicators();
}

/*
extern int g_htrans_rev;

/// NOTE: param 'bone' is used to calculate num_child_bones
void SkeletalMeshMotionViewer::Update_r( const msynth::CBone& bone,
                                          const msynth::CTransformNode& node,
										  boost::shared_ptr<SkeletalMesh>& pMesh )//,
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
//	else if( g_htrans_rev == 3 )
//	{
//		Matrix34 local_transform;
//		local_transform.vPosition = node.GetLocalTranslation();
//		local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix() * bone.GetOrient();
//		pMesh->SetLocalTransformToCache( index, local_transform );
//	}

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
}
*/

void SkeletalMeshMotionViewer::Update( const msynth::CKeyframe& keyframe )
{
	shared_ptr<SkeletalMesh> pSkeletalMesh = GetSkeletalMesh( m_SkeletalMesh );
	if( !pSkeletalMesh )
		return;

	shared_ptr<CSkeleton> pSkeleton = m_pSkeleton.lock();
	if( !pSkeleton )
		return;

	UpdateMeshBoneTransforms( keyframe, *pSkeleton, *pSkeletalMesh );

	UpdateLocalRotationIndicators( *pSkeleton, keyframe );
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
m_DisplaySkeletalMesh(true),
m_pPlaytimeText(NULL),
m_Playing(false)
{
	BoxMeshGenerator box_mesh_generator;
	box_mesh_generator.Generate( Vector3(1,1,1) );
	C3DMeshModelArchive archive = box_mesh_generator.GetMeshArchive();
	m_UnitCube.LoadFromArchive( archive );

	m_MeshViewer.Init();
}


CMotionPrimitiveViewer::~CMotionPrimitiveViewer()
{
	m_pGUIInputHandler->RemoveChild( m_pInputHandler.get() );
//	or InputHub().RemoveInputHandler( m_pInputHandler.get() );
	GetInputHub().RemoveInputHandler( m_pGUIInputHandler.get() );
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

	shared_ptr<GraphicsElementManager> pGraphicsElementMgr
		= pRendererMgr->GetGraphicsElementManager();

//	pGraphicsElementMgr->LoadFont( 0, "Arial", CFontBase::FONTTYPE_NORMAL, 8, 16 );
	pGraphicsElementMgr->LoadFont( 0, "BuiltinFont::BitstreamVeraSansMono-Bold-256", FontBase::FONTTYPE_TEXTURE, 8, 16, 0, 0 );

	m_pPlaytimeText
		= pDialog->AddStatic( STC_PLAYTIME, GraphicsComponent::RectAtLeftBottom( 300, 40, 20, 20 ), "0.0" );

	m_pGUIInputHandler = shared_ptr<InputHandler>( new CGM_DialogInputHandler( m_pDialogManager ) );
	GetInputHub().PushInputHandler( 0, m_pGUIInputHandler.get() );

	// set up guide geometry
	float h = g_fIndicatorHeight;
	m_DirectionGuide.AddLineSegment( Vector3(-100.0f, h,   0.0f), Vector3( 100.0f, h,  0.0f), 0xFFC0C0C0 );
	m_DirectionGuide.AddLineSegment( Vector3(0.0f,    h,-100.0f), Vector3( 0.0f,   h,100.0f), 0xFFF0F0F0 );


	// input handler for display options
	m_pInputHandler.reset( new CInputDataDelegate<CMotionPrimitiveViewer>( this ) );
	m_pGUIInputHandler->AddChild( m_pInputHandler.get() );
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
	if( m_Playing )
	{
		m_fPlaySpeedFactor = 1.0f;
	}
	else
	{
		shared_ptr<InputDeviceGroup> pInputDeivceGroup = GetInputDeviceHub().GetInputDeviceGroup(0);
		if( pInputDeivceGroup )
		{
			CInputState::Name left_shift = pInputDeivceGroup->GetInputState( GIC_LSHIFT );
			CInputState::Name right      = pInputDeivceGroup->GetInputState( GIC_RIGHT );
			CInputState::Name left       = pInputDeivceGroup->GetInputState( GIC_LEFT );
			if( left_shift == CInputState::PRESSED && right == CInputState::PRESSED )
				m_fPlaySpeedFactor =  1.0f;
			if( left_shift == CInputState::PRESSED && left == CInputState::PRESSED )
				m_fPlaySpeedFactor = -1.0f;
/*			else
				m_fPlaySpeedFactor =  0.0f;*/
		}
	}

	UpdatePlayTime( m_fCurrentPlayTime + dt * m_fPlaySpeedFactor );
}


void CMotionPrimitiveViewer::RenderFloor()
{
//	m_UnitCube.SetDiffuseColors( SFloatRGBAColor( 0.7f, 0.7f, 0.7f, 0.5f ) );
	m_UnitCube.SetDiffuseColors( SFloatRGBAColor( 0.6f, 0.6f, 0.6f, 1.0f ) );

	Matrix34 pose = Matrix34Identity();

	int x,z;
	for( z=-4; z<=4; z++ )
	{
		for( x=-4; x<=4; x++ )
		{
			// set world transform
			pose.vPosition = Vector3( x * 1.0f, 0.0f, z * 1.0f );
			Matrix44 scaling = Matrix44Scaling( 0.99f, 0.01f, 0.99f );
			FixedFunctionPipelineManager().SetWorldTransform( ToMatrix44(pose) * scaling );

			m_UnitCube.Render();
		}
	}

	m_DirectionGuide.Draw();
}


void CMotionPrimitiveViewer::RenderPoles()
{
//	m_UnitCube.SetDiffuseColors( SFloatRGBAColor( 0.7f, 0.7f, 0.7f, 0.5f ) );
	m_UnitCube.SetDiffuseColors( SFloatRGBAColor( 0.7f, 0.7f, 0.7f, 0.3f ) );

	Matrix34 pose = Matrix34Identity();

	const float pole_height = 2.0f;

	int x,z;
	for( z=-1; z<=1; z++ )
	{
		for( x=-1; x<=1; x++ )
		{
			// set world transform
			pose.vPosition = Vector3( x * 2.0f, pole_height * 0.5f, z * 2.0f );
			Matrix44 scaling = Matrix44Scaling( 0.02f, pole_height, 0.02f );
			FixedFunctionPipelineManager().SetWorldTransform( ToMatrix44(pose) * scaling );

			m_UnitCube.Render();
		}
	}
}


void CMotionPrimitiveViewer::Render()
{
	GraphicsDevice().Disable( RenderStateType::LIGHTING );
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	if( m_DisplaySkeletalMesh )
		m_MeshViewer.Render();

	RenderFloor();

	m_MotionTrace.Draw();

	m_KeyframeCoords.Draw();

	m_SkeletonRenderer.Render();

	RenderPoles();

	// render UI

	GraphicsDevice().Disable( RenderStateType::DEPTH_TEST );

	if( m_pDialogManager )
		m_pDialogManager->Render();

	// help text
	if( m_pDialogManager
	 && m_pDialogManager->GetControlRendererManager()
	 )
	{
		// borrow a font from GUI manager
		shared_ptr<GraphicsElementManager> pElementMgr
			= m_pDialogManager->GetControlRendererManager()->GetGraphicsElementManager();

		FontBase *pFont = NULL;
		if( pElementMgr )
			pFont = pElementMgr->GetFont( 0 );

		if( pFont )
		{
			int w=0,h=0;
			pFont->GetFontSize( w, h );
			pFont->SetFontSize( 8, 16 );
			int sh = GraphicsComponent::GetScreenHeight();
			int y = sh - 20 * 2 - 10;
			pFont->DrawText( "V: Load a motion database", Vector2(300,(float)y+20*0), 0xFFFFFFFF );
			pFont->DrawText( "C: Load a skeletal mesh",   Vector2(300,(float)y+20*1), 0xFFFFFFFF );
			pFont->SetFontSize( w, h );
		}
	}
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


int CMotionPrimitiveViewer::LoadMotionPrimitivesFromDatabase( const std::string& mdb_filepath, const std::string& motion_table_name )
{
	msynth::CMotionDatabase db;
	bool success = db.LoadFromFile( mdb_filepath );

	if( !success )
	{
		return -1;
	}

	m_vecpMotionPrimitive.resize( 0 );

	msynth::CHumanoidMotionTable tbl;

	bool res = db.GetHumanoidMotionTable( motion_table_name, tbl );

	BOOST_FOREACH( const msynth::CHumanoidMotionEntry& entry, tbl.m_vecEntry )
	{
		BOOST_FOREACH( const std::string& motion_name, entry.m_vecMotionPrimitiveName )
		{
			shared_ptr<CMotionPrimitive> pMotion = db.GetMotionPrimitive( motion_name );

			if( pMotion )
				m_vecpMotionPrimitive.push_back( pMotion );
			else
			{
				LOG_PRINT_ERROR( " The motion '" + motion_name + "' was not found in the database." );
			}
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

	if( m_pDialogManager )
		m_pDialogManager->OpenRootDialog( ROOT_DIALOG );

	return 0;
}


void CMotionPrimitiveViewer::UpdatePlayTime( float new_playtime )
{
	clamp( new_playtime, 0.0f, 50.0f );
	m_fCurrentPlayTime = new_playtime;

	if( m_pPlaytimeText )
		m_pPlaytimeText->SetText( to_string(m_fCurrentPlayTime) );
}


void CMotionPrimitiveViewer::HandleInput( const InputData& input )
{
	shared_ptr<InputDeviceGroup> pInputDeivceGroup = GetInputDeviceHub().GetInputDeviceGroup(0);
	const CInputState::Name left_shift
		= pInputDeivceGroup ? pInputDeivceGroup->GetInputState( GIC_LSHIFT ) : CInputState::RELEASED;

	switch( input.iGICode )
	{
	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED
		 && !m_Playing
		 && left_shift == CInputState::RELEASED )
			UpdatePlayTime( m_fCurrentPlayTime + 1.0f / 60.0f );
		break;
	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED
		 && !m_Playing
		 && left_shift == CInputState::RELEASED )
			UpdatePlayTime( m_fCurrentPlayTime - 1.0f / 60.0f );
		break;
	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Playing = !m_Playing;
//			if( 0.0f < m_fPlaySpeedFactor )
//			m_fCurrentPlayTime += 1.0f / 60.0f;
		}
		break;
	case '0':
//		if( input.iType == ITYPE_KEY_PRESSED )
//			m_pViewer->ToggleDisplayHelpText();
		break;
	case '2':
//		if( input.iType == ITYPE_KEY_PRESSED )
//			m_pViewer->ToggleDisplaySkeleton();
		break;
	case '3':
		if( input.iType == ITYPE_KEY_PRESSED )
			ToggleDisplaySkeletalMesh();
		break;

	case 'C':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			string pathname;
			GetFilename( pathname );
			if( 0 < pathname.length() )
			{
				m_MeshViewer.LoadSkeletalMesh( pathname );
			}
		}

	case 'V':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			string pathname;
			GetFilename( pathname );
			if( 0 < pathname.length() )
			{
				int ret = LoadMotionPrimitivesFromDatabase( pathname );
			}
		}
	default:
		break;
	}
}
