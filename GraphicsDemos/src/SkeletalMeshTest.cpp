#include "SkeletalMeshTest.hpp"
//#include "VehicleController.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/SkyboxMisc.hpp"
//#include "amorphous/GameCommon/CriticalDamping.hpp"
#include "amorphous/Input/InputDevice.hpp"
#include "amorphous/Input/InputDeviceGroup.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::vector;
using std::string;
using boost::shared_ptr;


//===========================================================================
// CSkeletalMeshTest
//===========================================================================

CSkeletalMeshTest::CSkeletalMeshTest()
{
//	m_fDetailLevel = 1.0f;
//	m_vLightDir = Vector3(0,0,0);

//	m_MeshFilepath = "???.msh";
}


CSkeletalMeshTest::~CSkeletalMeshTest()
{
}


Result::Name CSkeletalMeshTest::LoadShader()
{
	GenericShaderDesc gsd;
	gsd.Specular = SpecularSource::DECAL_TEX_ALPHA;
	gsd.VertexBlendType = VertexBlendType::QUATERNION_AND_VECTOR3;

	ShaderResourceDesc sd;
	sd.pShaderGenerator.reset( new GenericShaderGenerator( gsd ) );

	bool loaded = m_Shader.Load( sd );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CSkeletalMeshTest::InitLight( ShaderManager& shader_mgr )
{
	shared_ptr<ShaderLightManager> pShaderLightMgr = shader_mgr.GetShaderLightManager();

	if( !pShaderLightMgr )
		return Result::UNKNOWN_ERROR;

	Vector3 vLightDir = Vector3(-0.5f, -1.0f, 0.8f);
	HemisphericDirectionalLight light;
	light.vDirection = vLightDir;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );

	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	return Result::SUCCESS;
}


int CSkeletalMeshTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

	Result::Name res = LoadShader();
//
//	if( !shader_loaded )
//		return -1;

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
		InitLight( *pShaderMgr );

	string mesh_pathname = "models/skeletal_mesh.msh";//m_MeshFilepath;

	LoadParamFromFile( "params.txt", "mesh", mesh_pathname );

	if( 0 < mesh_pathname.length() )
	{
		MeshResourceDesc mrd;
		mrd.ResourcePath = mesh_pathname;
//		mrd.MeshType = CMeshType::SKELETAL;

//		bool loaded = m_Mesh.Load( mesh_pathname );
		bool loaded = m_Mesh.Load( mrd );
		if( !loaded )
			return -1;
	}
	else
		return -1;

	shared_ptr<SkeletalMesh> pSMesh
		= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( m_Mesh.GetMesh() );

	if( !pSMesh )
		return -1;

//	if( 1 < pSMesh->GetNumBones() )
//	{
//		m_pVehicleController.reset( new CVehicleController );
//		m_pVehicleController->Init( *pSMesh );
//	}

	return 0;
}


void CSkeletalMeshTest::RenderMesh()
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	if( !pShaderMgr )
//		return;

	ShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	shader_mgr.SetParam( "g_vEyePos", GetCurrentCamera().GetPosition() );

	// alpha-blending settings 
	GraphicsDevice().SetRenderState( RenderStateType::ALPHA_BLEND, false );

	ShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );

	Result::Name res = shader_mgr.SetTechnique( tech );

	boost::shared_ptr<BasicMesh> pBasicMesh = m_Mesh.GetMesh();

	if( !pBasicMesh )
		return;

	shared_ptr<SkeletalMesh> pSMesh
		= boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>( pBasicMesh );

	if( !pSMesh )
	{
		pBasicMesh->Render();
		return;
	}

	pSMesh->Render( shader_mgr );
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void CSkeletalMeshTest::Render()
{
//	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
//	if( pShaderMgr )
//	{
//		pShaderMgr->SetViewTransform( g_CameraController.GetCameraMatrix() );
//		RenderMesh();
//		pShaderMgr->SetWorldTransform( Matrix44Identity() );
//	}

	RenderMesh();

//	CLineSegmentRenderer::Draw( -m_vLightDir * 5.0f, -m_vLightDir * 3.0f, 0xFFF0F0F0 );

	// rendering

	if( m_pFont )
	{
//		char acStr[256];

//		sprintf( acStr, "LOD (0.0 - 1.0): %02.1f", m_fDetailLevel );
//		m_pFont->DrawText( acStr, Vector2(20,36), 0xFFFFFFFF );

//		if( m_pVehicleController )
//		{
//			sprintf( acStr, "speed: %02.1f", m_pVehicleController->GetSpeed() );
//			m_pFont->DrawText( acStr, Vector2(20,52), 0xFFFFFFFF );
//		}
	}
}


void CSkeletalMeshTest::Update( float dt )
{
}


Result::Name CSkeletalMeshTest::UpdateVertexBlendTransformsFromLocalTransforms( SkeletalMesh& skeletal_mesh, const std::vector<Transform>& local_transforms )
{
	std::vector<Transform> vertex_blend_transforms;

	skeletal_mesh.CalculateBlendTransforms( local_transforms, vertex_blend_transforms );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return Result::INVALID_ARGS;

	pShaderMgr->SetVertexBlendTransforms( vertex_blend_transforms );

	return Result::SUCCESS;
}


Result::Name CSkeletalMeshTest::SetUniformRotations( float angle, unsigned int axis )
{
	std::vector<Transform> local_transforms;

	boost::shared_ptr<BasicMesh> pMesh = m_Mesh.GetMesh();
	if( !pMesh )
		return Result::INVALID_ARGS;

	boost::shared_ptr<SkeletalMesh> pSMesh = boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pMesh);
	if( !pSMesh )
		return Result::INVALID_ARGS;

	local_transforms.resize( pSMesh->GetNumBones() );

	Matrix33 rotation( Matrix33Identity() );
	switch(axis)
	{
	case 0: rotation = Matrix33RotationX(angle); break;
	case 1: rotation = Matrix33RotationY(angle); break;
	case 2: rotation = Matrix33RotationZ(angle); break;
	default:
		rotation = Matrix33Identity();
	}

	Transform transform;
	transform.qRotation.FromRotationMatrix( rotation );
	for( size_t i=0; i<local_transforms.size(); i++ )
		local_transforms[i] = transform;

	return UpdateVertexBlendTransformsFromLocalTransforms( *pSMesh, local_transforms );
}


void CSkeletalMeshTest::RenderScene()
{
}


int GetRotationAxis()
{
	CInputState::Name st0 = InputDeviceHub().GetInputDeviceGroup(0)->GetInputState( 'Z' );
	CInputState::Name st1 = InputDeviceHub().GetInputDeviceGroup(0)->GetInputState( 'X' );
	CInputState::Name st2 = InputDeviceHub().GetInputDeviceGroup(0)->GetInputState( 'C' );
	
	if( st0 == CInputState::PRESSED )
		return 2;
	else if( st1 == CInputState::PRESSED )
		return 0;
	else if( st2 == CInputState::PRESSED )
		return 1;
	else
		return -1;
}


void CSkeletalMeshTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'Z':
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	case GIC_MOUSE_AXIS_X:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			int axis = GetRotationAxis();
			if( 0 <= axis && axis < 3 )
			{
//				SetUniformRotations( input.fParam1, (unsigned int)axis );
			}
		}
		break;
	default:
		break;
	}
}
