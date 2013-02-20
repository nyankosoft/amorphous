#include "PlanarReflectionTest.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/Mesh/BasicMesh.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderManagerHub.hpp"
#include "gds/Graphics/SkyboxMisc.hpp"
#include "gds/Graphics/TextureRenderTarget.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/CameraController_Win32.hpp"
#include "gds/Support/Timer.hpp"

using std::string;
using std::vector;
using namespace boost;


template<typename ColorType>
class TLVertex
{
public:
	TLVertex()
		:
	Vector3(0,0,0)
	{}

	TLVertex( const Vector3& _position, const ColorType& _color ) : position(_position), color(_color)
	{}

	~TLVertex(){}

	Vector3 position;
	ColorType color;
};


#include "gds/Graphics/Direct3D/Direct3D9.hpp"

namespace amorphous {
extern D3DPRIMITIVETYPE ToD3DPrimitiveType( PrimitiveType::Name pt ); // in gds/Graphics/Direct3D/2DPrimitive/2DPrimitiveRenderer_D3D.cpp
}

void DrawPrimitives( PrimitiveType::Name primitive_type, const TLVertex<U32> *vertices, uint num_vertices )//, int u32_color_componets_order == always ARGB )
{
	if( !vertices || num_vertices == 0 )
		return;

	// Support primitives types other than triangle fan and triangle strips
	uint num_primitives = num_vertices - 2;

	D3DPRIMITIVETYPE d3d_pt = amorphous::ToD3DPrimitiveType( primitive_type );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;

	DWORD fvf = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE);

	hr = pd3dDev->SetVertexShader( NULL );
	hr = pd3dDev->SetPixelShader( NULL );

	hr = pd3dDev->SetFVF( fvf );

	hr = pd3dDev->DrawPrimitiveUP( d3d_pt, num_primitives, vertices, sizeof(TLVertex<U32>) );
}


void DrawPrimitives( PrimitiveType::Name primitive_type, const std::vector< TLVertex<U32> >& vertices )
{
	if( vertices.empty() )
		return;

	DrawPrimitives( primitive_type, &vertices[0], (uint)vertices.size() );
}



Vector3 GetMirroredPosition( const Plane& plane, const Vector3& pos )
{
	float d = plane.GetDistanceFromPoint( pos );
	return pos - plane.normal * d * 2.0f;
}


void RenderPlane( const Matrix34& plane_pose, float x, float y )
{
//	C3DRect rect;
}



//extern void InitCg();

CPlanarReflectionTest::CPlanarReflectionTest()
{
//	InitCg();
}


CPlanarReflectionTest::~CPlanarReflectionTest()
{
}


int CPlanarReflectionTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();

//	m_SkyboxTechnique.SetTechniqueName( "SkyBox" );
	m_MeshTechnique.SetTechniqueName( "Default" );
	m_DefaultTechnique.SetTechniqueName( "NullShader" );

	// initialize shader
//	string shader_path = "./shaders/PlanarReflectionTest.fx";
	string shader_path = "./shaders/PerPixelSingleHSDirectionalLight.fx";
	bool shader_loaded = m_Shader.Load( shader_path );

	string pr_shader_path = "./shaders/PerPixelSingleHSDirectionalLight_PR.fx";
	LoadParamFromFile( "params.txt", "planar_reflection_shader", pr_shader_path );
	shader_loaded = m_PlanarReflectionShader.Load( pr_shader_path );
	
	// load skybox mesh
	m_SkyboxMesh = CreateSkyboxMesh( "./textures/skygrad_slim_01.jpg" );

	// load the terrain mesh
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = "./models/terrain06.msh";
	mesh_desc.MeshType     = MeshType::BASIC;
	m_TerrainMesh.Load( mesh_desc );

	m_ReflectionSourceMeshes.resize( 1 );
	m_ReflectionSourceMeshes[0].Load( "./models/wall_and_ceiling.msh" );

	m_ReflectiveSurfaceMeshes.resize( 1 );
	m_ReflectiveSurfaceMeshes[0].Load( "./models/floor.msh" );

//	m_TestTexture.Load( "./textures/flare02.dds" );

	m_pTextureRenderTarget = TextureRenderTarget::Create();
	m_pTextureRenderTarget->InitScreenSizeRenderTarget();

	m_PerturbationTexture.Load( "./textures/watersurf_nmap.jpg" );

	return 0;
}


void CPlanarReflectionTest::Update( float dt )
{
}


void CPlanarReflectionTest::RenderReflectionSourceMeshes( const Matrix34& camera_pose, CullingMode::Name culling_mode )
{
	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

	GraphicsDevice().SetCullingMode( culling_mode );

	RenderAsSkybox( m_SkyboxMesh, camera_pose );

	GraphicsDevice().SetCullingMode( culling_mode );

	Matrix44 matWorld = Matrix44Identity();
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetWorldTransform( matWorld );

	Result::Name res = shader_mgr.SetTechnique( m_MeshTechnique );

	for( size_t i=0; i<m_ReflectionSourceMeshes.size(); i++ )
	{
		shared_ptr<BasicMesh> pMesh = m_ReflectionSourceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}
}


void CPlanarReflectionTest::RenderReflectionSurface()
{
	ShaderManager *pShaderMgr = m_PlanarReflectionShader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetTexture( 2, m_PerturbationTexture );

	ShaderTechniqueHandle tech;
	tech.SetTechniqueName( "Default" );
	shader_mgr.SetTechnique( tech );

	shader_mgr.SetTexture( 1, m_pTextureRenderTarget->GetRenderTargetTexture() );

	// shift UV of perturbation texture to make the reflection look like water surface with waves
	vector<float> uv_shift;
	uv_shift.resize( 2 );
	uv_shift[0] = (float)GlobalTimer().GetTime() * 0.05f;
	uv_shift[1] = 0;

	shader_mgr.SetParam( "g_vPerturbationTextureUVShift", uv_shift );

	for( size_t i=0; i<m_ReflectiveSurfaceMeshes.size(); i++ )
	{
		shared_ptr<BasicMesh> pMesh = m_ReflectiveSurfaceMeshes[i].GetMesh();
		
		if( !pMesh )
			continue;
		
		pMesh->Render( shader_mgr );
	}
}

/*
void RenderReflectionClipPlane() //const Plane& reflection_plane )
{
	TLVertex<U32> verts[4] =
	{
		TLVertex<U32>( Vector3(-100, 0, 100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3( 100, 0, 100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3( 100, 0,-100), 0xFFFF0000 ),//0x00000000 ),
		TLVertex<U32>( Vector3(-100, 0,-100), 0xFFFF0000 ),//0x00000000 )
	};

	GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

	DrawPrimitives( PrimitiveType::TRIANGLE_FAN, verts, 4 );
}
*/


void SetClipPlaneViaD3DXFunctions()// const Plane& reflection_plane )
{
	D3DXPLANE clipPlane;
	D3DXVECTOR3 point( 0 , 0, 0 );
	D3DXVECTOR3 normal( 0, -1, 0 ); // negative of the normal vector.
	// create and normalize the plane
	D3DXPlaneFromPointNormal(&clipPlane,&point,&normal);
	D3DXPlaneNormalize(&clipPlane,&clipPlane);

	// To transform a plane from world space to view space there is a methode D3DXPlaneTransform
	// but the peculiar thing about this method is that it takes the inverse transpose of the viewprojection matrix

	Matrix44 proj_view = g_Camera.GetProjectionMatrix() * g_Camera.GetCameraMatrix();
	D3DXMATRIXA16 matrix, view_proj;
	proj_view.GetRowMajorMatrix44( (float *)&view_proj );

	D3DXMatrixInverse(&matrix, NULL, &view_proj); // second parameter is an out parameter for the determinant
	D3DXMatrixTranspose(&matrix, &matrix);

	D3DXPLANE viewSpacePlane;
	D3DXPlaneTransform(&viewSpacePlane, &clipPlane, &matrix);

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;
	hr = pd3dDev->SetClipPlane( 0, (float *)&viewSpacePlane );
	hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
}


void SetClipPlane( const Plane& reflection_plane )
{
	Plane src_plane( reflection_plane );
	src_plane.Flip();

	Matrix44 proj_view = g_Camera.GetProjectionMatrix() * g_Camera.GetCameraMatrix();
	Matrix44 inv_proj_view = proj_view.GetInverse();
	Matrix44 inv_transpose_proj_view = Matrix44Transpose( inv_proj_view );

	Matrix44 res = proj_view * inv_proj_view;

	Plane reflection_plane_in_clipping_space;
	Vector4 src_plane_normal4( src_plane.normal.x, src_plane.normal.y, src_plane.normal.z, 1.0f );
	Vector4 reflection_plane_in_clipping_space_normal4 = inv_transpose_proj_view * src_plane_normal4;
	reflection_plane_in_clipping_space.normal = inv_transpose_proj_view * src_plane.normal;
	Vector3 pos_on_plane = src_plane.normal * src_plane.dist;
	Vector3 pos_on_clipping_space_plane = inv_transpose_proj_view * pos_on_plane;
	reflection_plane_in_clipping_space.dist = Vec3Dot( pos_on_clipping_space_plane, reflection_plane_in_clipping_space.normal );

	float plane_coefficients[] =
	{
		reflection_plane_in_clipping_space.normal.x,
		reflection_plane_in_clipping_space.normal.y,
		reflection_plane_in_clipping_space.normal.z,
		reflection_plane_in_clipping_space.dist * (-1.0f)
	};

	// For fixed function pipeline
/*	float plane_coefficients[] =
	{
		reflection_plane.normal.x,
		reflection_plane.normal.y,
		reflection_plane.normal.z,
		reflection_plane.dist * (-1.0f)
	};*/

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;
	hr = pd3dDev->SetClipPlane( 0, plane_coefficients );
	hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
}


void CPlanarReflectionTest::Render()
{
	// Render mirrored scene to the texture render target

	m_pTextureRenderTarget->SetRenderTarget();

	Plane reflection_plane( Vector3(0,1,0), 0 );
	Matrix44 mirror = Matrix44Mirror( reflection_plane );

	Matrix44 view = GetCurrentCamera().GetCameraMatrix();
	GetShaderManagerHub().PushViewAndProjectionMatrices( view * mirror, GetCurrentCamera().GetProjectionMatrix() );

/*	ShaderManager& ffp_mgr = FixedFunctionPipelineManager();
	Matrix44 view = ffp_mgr.GetViewTransform();
	ffp_mgr.SetViewTransform( view * mirror );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( pShaderMgr )
	{
		view = pShaderMgr->GetViewTransform();
		pShaderMgr->SetViewTransform( view * mirror );
	}*/

	GraphicsDevice().SetClipPlane( 0, reflection_plane );
	GraphicsDevice().EnableClipPlane( 0 );
	GraphicsDevice().UpdateViewProjectionTransformsForClipPlane(
		0,
		GetCurrentCamera().GetCameraMatrix(),
		GetCurrentCamera().GetProjectionMatrix()
		);

//	GraphicsDevice().SetCullingMode( CullingMode::CLOCKWISE );

//	RenderReflectionClipPlane( /*reflection_plane*/ );
/*	SetClipPlane( reflection_plane );
	SetClipPlaneViaD3DXFunctions();*/

//	RenderReflectionSourceMeshes( GetCurrentCamera().GetPose(), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetMirroredPose( Plane(Vector3(0,1,0),0), GetCurrentCamera().GetPose() ), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetMirroredPosition( Plane(Vector3(0,1,0),0), GetCurrentCamera().GetPosition() ), CullingMode::CLOCKWISE );
//	RenderReflectionSourceMeshes( GetCameraPoseFromCameraMatrix( mirror ), CullingMode::CLOCKWISE );
	Matrix34 camera_pose = GetCurrentCamera().GetPose();
	RenderReflectionSourceMeshes( Matrix34( GetMirroredPosition(Plane(Vector3(0,1,0),0),camera_pose.vPosition), camera_pose.matOrient ), CullingMode::CLOCKWISE );

//	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
//	HRESULT hr = pd3dDev->SetRenderState( D3DRS_CLIPPLANEENABLE, 0 );

	GraphicsDevice().DisableClipPlane( 0 );

	GetShaderManagerHub().PopViewAndProjectionMatrices();

	m_pTextureRenderTarget->ResetRenderTarget();

	// Render the scene that has planar reflection
//	GraphicsDevice().SetCullingMode( CullingMode::COUNTERCLOCKWISE );
	RenderReflectionSourceMeshes( GetCurrentCamera().GetPose(), CullingMode::COUNTERCLOCKWISE );

	// Render surface that does planar reflection
	RenderReflectionSurface();

	// lens flares

//	pShaderMgr->SetTechnique( m_DefaultTechnique );

//	rect.Draw();
}


void CPlanarReflectionTest::UpdateViewTransform( const Matrix44& matView )
{
}


void CPlanarReflectionTest::UpdateProjectionTransform( const Matrix44& matProj )
{
}
