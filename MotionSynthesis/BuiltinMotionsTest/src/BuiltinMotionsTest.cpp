#include "BuiltinMotionsTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/Mesh/SkeletalMesh.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "gds/Graphics/TextureGenerators/GridTextureGenerator.hpp"
#include "gds/Graphics/PrimitiveShapeRenderer.hpp"
#include "gds/Graphics/HemisphericLight.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/GenericShaderDesc.hpp"
#include "gds/Graphics/Shader/GenericShaderGenerator.hpp"
#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/ShadowMaps.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp"
#include "gds/MotionSynthesis/SkeletonRenderer.hpp"
#include "gds/MotionSynthesis/BuiltinMotions.hpp"
#include "gds/MotionSynthesis/SkeletalMeshTransform.hpp"

using std::string;
using std::vector;
using namespace boost;
using namespace msynth;


class CBuiltinMotionTestShadowMapSceneRenderer : public ShadowMapSceneRenderer
{
	BuiltinMotionsTest *m_pTarget;

public:

	CBuiltinMotionTestShadowMapSceneRenderer( BuiltinMotionsTest *pTarget )
		:
	m_pTarget(pTarget)
	{}

	/// render objects that cast shadows
	void RenderSceneToShadowMap( Camera& camera )
	{
//		m_pTarget->RenderShadowCasters();
	}

	/// render objects which are cast shadows by others
	void RenderShadowReceivers( Camera& camera )
	{
//		m_pTarget->RenderShadowReceivers();
	}
};


extern CPlatformDependentCameraController g_CameraController;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new BuiltinMotionsTest();
}


extern const std::string GetAppTitle()
{
	return string("Built-in Motions Test");
}



BuiltinMotionsTest::BuiltinMotionsTest()
:
m_CurrentKeyframeIndex(0),
m_CurrentModelIndex(0),
m_fCurrentTime(0),
m_IsPlayingMotion(false)
{
}


BuiltinMotionsTest::~BuiltinMotionsTest()
{
}


int BuiltinMotionsTest::Init()
{
	m_CharacterModelPathnames.reserve( 16 );
	m_CharacterModelPathnames.push_back( "./models/male_skinny_young.bak.msh" );
	m_CharacterModelPathnames.push_back( "./models/male-height1.94--boxes.msh" );

	LoadCharacterModel();

	InitBuiltinMotions();

	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/BuiltinMotionsTest.fx" );

	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = MeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	shared_ptr<GridTextureGenerator> pGridTexGenerator( new GridTextureGenerator );
	pGridTexGenerator->m_BaseColor = SFloatRGBAColor::White();
	pGridTexGenerator->m_LineColor = SFloatRGBAColor( 0.5f, 1.0f, 0.5f, 1.0f );
	TextureResourceDesc grid_tex_desc;
	grid_tex_desc.Width   = 1024;
	grid_tex_desc.Height  = 1024;
	grid_tex_desc.Format  = TextureFormat::A8R8G8B8;
	grid_tex_desc.pLoader = pGridTexGenerator;
	m_FloorTexture.Load( grid_tex_desc );

	ShaderResourceDesc character_shader_desc;
	GenericShaderDesc desc;
	desc.Specular = SpecularSource::DECAL_TEX_ALPHA;
//	desc.Specular = SpecularSource::NONE;
	desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	desc.ShaderLightingType = ShaderLightingType::PER_VERTEX;
	desc.VertexBlendType = CVertexBlendType::QUATERNION_AND_VECTOR3;
	character_shader_desc.pShaderGenerator.reset( new GenericShaderGenerator( desc ) );
	shader_loaded = m_SkeletalCharacterShader.Load( character_shader_desc );

	return 0;
}


void BuiltinMotionsTest::Update( float dt )
{
	if( m_IsPlayingMotion )
	{
		m_fCurrentTime += dt;

		shared_ptr<CMotionPrimitive> pMotion = GetCurrentMotion();
		const std::vector<CKeyframe>& keyframes = pMotion->GetKeyframeBuffer();
		if( !keyframes.empty() )
		{
			float last_frame_time = (float)keyframes.back().GetTime();
			if( pMotion->IsLoopedMotion() )
			{
				if( last_frame_time <= m_fCurrentTime )
					m_fCurrentTime -= last_frame_time;
			}
			else
			{
				clamp( m_fCurrentTime, 0.0f, last_frame_time );
			}
		}
	}
}


bool BuiltinMotionsTest::GetKeyframeToRender( CKeyframe& dest )
{
	if( m_pMotions.empty() )
		return false;

	boost::shared_ptr<CMotionPrimitive> pMotion = m_pMotions[0];
	if( !pMotion )
		return false;

	if( pMotion->GetKeyframeBuffer().empty() )
		return false;

	if( m_IsPlayingMotion )
	{
		const std::vector<CKeyframe>& keyframes = pMotion->GetKeyframeBuffer();
		if( keyframes.empty() )
			return false;

		pMotion->GetInterpolatedKeyframe( dest, m_fCurrentTime );
	}
	else
	{
		unsigned int keyframe_index
			= get_clamped( m_CurrentKeyframeIndex, (unsigned int)0, (unsigned int)pMotion->GetKeyframeBuffer().size() - 1 );

		dest = pMotion->GetKeyframeBuffer()[keyframe_index];
	}

	return true;
}


void BuiltinMotionsTest::Render()
{
	// A draw test of a 2D primitive
	C2DRect rect( Vector2( 50, 50 ), Vector2( 80, 80 ), 0xFFFF0000 );
//	rect.Draw();

	GraphicsDevice().Enable(  RenderStateType::DEPTH_TEST );

	Matrix44 matWorld = Matrix44Identity();
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	if( !pShaderMgr )
//		return;

	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	PrimitiveShapeRenderer primitive_renderer;
	primitive_renderer.RenderFloorPlane( Vector3(0,0,0), 16.0f, 16.0f, SFloatRGBAColor::White(), m_FloorTexture );

//	RenderAsSkybox( m_SkyboxMesh, g_CameraController.GetPosition() );

	shader_mgr.SetWorldTransform( matWorld );

	shader_mgr.SetTechnique( m_MeshTechnique );

	shared_ptr<BasicMesh> pTerrainMesh = m_TerrainMesh.GetMesh();
	if( pTerrainMesh )
		pTerrainMesh->Render( shader_mgr );

	shader_mgr.SetTechnique( m_DefaultTechnique );

	rect.Draw();

	// Need to turn off the face culling turned on in RenderAsSkybox()
	GraphicsDevice().Disable(  RenderStateType::FACE_CULLING );

	GraphicsDevice().Disable(  RenderStateType::LIGHTING );

	SetLight();

	GetKeyframeToRender( m_KeyframeToRender );
	const CKeyframe& keyframe = m_KeyframeToRender;

	CSkeletonRenderer renderer;
	renderer.SetDisplayBoneNames( false );
	renderer.SetSkeleton( m_pSkeleton );
	renderer.UpdateBonePoses( keyframe );
	renderer.Render();

	// The skeletal mesh of the character

	shared_ptr<BasicMesh> pMesh = m_SkeletalCharacter.GetMesh();
	shared_ptr<SkeletalMesh> pSkeletalMesh
		= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( pMesh );
	if( pMesh )
	{
		GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

		if( pSkeletalMesh && m_pSkeleton )
		{
			UpdateMeshBoneTransforms( keyframe, *m_pSkeleton, *pSkeletalMesh );
			pSkeletalMesh->CalculateBlendTransformsFromCachedLocalTransforms();
		}

		ShaderManager *pShader = m_SkeletalCharacterShader.GetShaderManager();
		ShaderManager& skeletal_mesh_shader = pShader ? *pShader : FixedFunctionPipelineManager();
		skeletal_mesh_shader.SetWorldTransform( Matrix44Identity() );
		vector<Transform> blend_transforms;
		pSkeletalMesh->GetBlendTransforms( blend_transforms );
		skeletal_mesh_shader.SetVertexBlendTransforms( blend_transforms );
		pMesh->Render( skeletal_mesh_shader );
	}

//	if( m_pFont )
//		m_pFont->DrawText( "Rendering some text for test...", 150, 150 );
}


void BuiltinMotionsTest::InitBuiltinMotions()
{
//	vector< shared_ptr<msynth::CMotionPrimitive> > m_pMotions;

	m_pSkeleton.reset( new msynth::CSkeleton );

	bool create_skeleton_from_skeletal_mesh = true;
	if( create_skeleton_from_skeletal_mesh )
	{
		shared_ptr<SkeletalMesh> pSkeletalMesh = m_SkeletalCharacter.GetSkeletalMesh();
		if( pSkeletalMesh )
		{
			m_pSkeleton = CreateSkeletonFromMeshSkeleton( *pSkeletalMesh );

			m_pSkeleton->DumpToTextFile( "./.debug/skeletal_mesh_skeleton.txt" );
		}
	}
	else
	{
		// Load fom a binary archive file?
		const char *skeleton_file_pathname = "misc/skeleton.bin";
		bool loaded = m_pSkeleton->LoadFromFile( skeleton_file_pathname );
		if( !loaded )
		{
			LOG_PRINT_ERROR( "Failed to load the skeleton: " + string(skeleton_file_pathname) );
		}

		m_pSkeleton->DumpToTextFile( "./.debug/skeleton.bin.txt" );
	}

	if( !m_pSkeleton )
		return;

	// Force the horizontal position of the root bone to the origin
	Vector3 root_offset = m_pSkeleton->RootBone().GetOffset();
	root_offset.x = 0;
	m_pSkeleton->RootBone().SetOffset( root_offset );

	const msynth::CSkeleton& skeleton = *m_pSkeleton;

	m_pMotions.resize( 0 );
	m_pMotions.reserve( 16 );
	m_pMotions.push_back( CreateWalkMotion( skeleton ) );
//	m_pMotions.push_back( CreateRunMotion( skeleton ) );
//	m_pMotions.push_back( CreateVerticalJumpMotion( skeleton ) );
}


shared_ptr<CMotionPrimitive> BuiltinMotionsTest::GetCurrentMotion()
{
	if( m_pMotions.empty() )
		return shared_ptr<CMotionPrimitive>();

	boost::shared_ptr<CMotionPrimitive> pMotion = m_pMotions[0];

	return pMotion;
}


void BuiltinMotionsTest::LoadCharacterModel()
{
	if( m_CharacterModelPathnames.empty()
	 || m_CharacterModelPathnames.size() <= (size_t)m_CurrentModelIndex )
	{
		return;
	}

	MeshResourceDesc character_mesh_desc;
	character_mesh_desc.ResourcePath = m_CharacterModelPathnames[m_CurrentModelIndex];
	character_mesh_desc.MeshType = MeshType::SKELETAL;
	m_SkeletalCharacter.Load( character_mesh_desc );

	shared_ptr<SkeletalMesh> pSkeletalMesh = m_SkeletalCharacter.GetSkeletalMesh();
	if( pSkeletalMesh )
	{
		CSkeleton skeleton;
//		CreateSkeletonFromSkeletalMesh( *pSkeletalMesh, skeleton );
		CreateSkeletonFromMeshSkeleton( *pSkeletalMesh, skeleton );
		const string& model_pathname = m_CharacterModelPathnames[m_CurrentModelIndex];
		skeleton.DumpToTextFile( ".debug/created_from_skeletal_mesh'" + lfs::get_leaf(model_pathname) + "'.txt" );
	}
}

void BuiltinMotionsTest::ResetTimeIfPlayedToEnd()
{
	shared_ptr<CMotionPrimitive> pCurrentMotion = GetCurrentMotion();
	if( !pCurrentMotion )
		return;

	const std::vector<CKeyframe>& keyframes = pCurrentMotion->GetKeyframeBuffer();
	if( keyframes.empty() )
		return;

	float length = keyframes.back().GetTime();
	if( fabs(length - m_fCurrentTime) < 0.001 )
		m_fCurrentTime = 0.0f;
}


void BuiltinMotionsTest::HandleInput( const SInputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			SaveTexturesAsImageFiles();
		}
		break;

	case 'N':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_CurrentModelIndex = (m_CurrentModelIndex + 1) % (int)m_CharacterModelPathnames.size();
			LoadCharacterModel();
		}
		break;

	case 'R':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			InitBuiltinMotions();
		}
		break;

	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_IsPlayingMotion = false;

			shared_ptr<CMotionPrimitive> pCurrentMotion = GetCurrentMotion();
			if( !pCurrentMotion )
				break;

			m_CurrentKeyframeIndex += 1;
			if( (int)pCurrentMotion->GetKeyframeBuffer().size() <= m_CurrentKeyframeIndex )
				m_CurrentKeyframeIndex = 0;

//			unsigned int m_CurrentMotionIndex;
//			if( (unsigned int)m_pMotions.size() <= m_CurrentMotionIndex )
//				m_CurrentKeyframeIndex = 0;
		}
		break;

	case GIC_LEFT:
		break;

	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_IsPlayingMotion = !m_IsPlayingMotion;
			if( m_IsPlayingMotion )
				ResetTimeIfPlayedToEnd();
		}
		break;

	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}


void BuiltinMotionsTest::SetLight()
{
	HemisphericDirectionalLight hs_dir_light;
	hs_dir_light.Attribute.UpperDiffuseColor = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f);
	hs_dir_light.Attribute.LowerDiffuseColor = SFloatRGBAColor(0.1f,0.1f,0.1f,1.0f);
	hs_dir_light.vDirection = Vec3GetNormalized( Vector3(1.0f,-3.0f,1.5f) );

	ShaderManager *pShaderMgr = m_SkeletalCharacterShader.GetShaderManager();
	if( pShaderMgr )
	{
		shared_ptr<ShaderLightManager> pShaderLightMgr = pShaderMgr->GetShaderLightManager();
		if( pShaderLightMgr )
		{
			pShaderLightMgr->ClearLights();
			pShaderLightMgr->SetHemisphericDirectionalLight( hs_dir_light );
			pShaderLightMgr->CommitChanges();
		}
	}
}
