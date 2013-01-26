#include "Graphics/DoubleConeScrollEffect.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/Meshgenerators/MeshGenerator.hpp"
#include "Graphics/TextureStage.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"
#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"


namespace amorphous
{

using std::vector;
using namespace boost;


class CDoubleConeMeshGenerator : public MeshGenerator
{
public:

//	std::string m_ScrollTextureFilepath;

public:

	CDoubleConeMeshGenerator() {}

	~CDoubleConeMeshGenerator() {}

	Result::Name Generate();
};



Result::Name CDoubleConeMeshGenerator::Generate()
{
	const int num_sides = 12;

	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh );
	CGeneral3DMesh& mesh = *pMesh;

	// vertex format
	// - No need for normals
	mesh.SetVertexFormatFlags( 
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	// vertices

	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = mesh.GetVertexBuffer();
	vector<CGeneral3DVertex>& vert_buffer = *pVertexBuffer;
	vert_buffer.resize( (num_sides+1) * 3 );

	float y = 0.5f;
	for( int i=0; i<num_sides+1; i++ )
		vert_buffer[i            ].m_vPosition = Vector3( 0.0f, y, 0.0f );
	for( int i=0; i<num_sides+1; i++ )
		vert_buffer[i+num_sides+1].m_vPosition = Vector3( 0.0f,-y, 0.0f );

	const int center_vert_offset = (num_sides+1) * 2;

	float fDeltaHeading = 2.0f * (float)PI / (float)num_sides;
	for( int i=0; i<num_sides+1; i++ )
	{
		vert_buffer[center_vert_offset + i].m_vPosition
		= Matrix33RotationY(fDeltaHeading * (float)i)
		* Vector3( 0.0f, 0.0f, 0.5f );
	}

	// diffuse colors
	// - top and bottom: transprarent
	// - center: opaque
	const SFloatRGBAColor diffuse_colors[] = { SFloatRGBAColor(1,1,1,0), SFloatRGBAColor(1,1,1,0), SFloatRGBAColor(1,1,1,1) };
	for( int i=0; i<3; i++ )
	{
		for( int j=0; j<num_sides+1; j++ )
		{
			vert_buffer[i * (num_sides+1) + j].m_DiffuseColor = diffuse_colors[i];
		}
	}

	// texture coordinates
	float fScaling = 4.0f;
	float fTexShiftV = 1.0f / (float)num_sides;
	float tex_v[] = { 0.0f, 1.0f, 0.5f };
	for( int i=0; i<3; i++ )
	{
		for( int j=0; j<num_sides+1; j++ )
		{
			CGeneral3DVertex& vert = vert_buffer[ i * (num_sides+1) + j ];
			vert.m_TextureCoord.resize( 1 );

			float u = fTexShiftV * (float)j;
			float v = tex_v[i];
			vert.m_TextureCoord[0] // test: = TEXCOORD2( 0.5f, 0.5f );
			= TEXCOORD2(u,v) * fScaling;
		}
	}

	// polygons
	vector<CIndexedPolygon>& polygon_buffer = mesh.GetPolygonBuffer();
	int num_polygons = num_sides * 2;
	polygon_buffer.reserve( num_polygons );
	for( int i=0; i<2; i++ )
	{
		const int vert_offset = i * (num_sides + 1);
		int i1 = (i==0) ? 1 : 0;
		int i0 = (i==0) ? 0 : 1;
		for( int j=0; j<num_sides; j++ )
		{
			polygon_buffer.push_back( CIndexedPolygon() );
			CIndexedPolygon& poly = polygon_buffer.back();
			poly.m_index.resize( 4 );
			poly.m_index[0] = vert_offset + j + i1;
			poly.m_index[1] = vert_offset + j + i0;
			poly.m_index[2] = center_vert_offset + j + i0;
			poly.m_index[3] = center_vert_offset + j + i1;
		}
	}

	// create mesh archive

	mesh.GetMaterialBuffer().resize( 1 );

	// Create mesh archive from the mesh
	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModelArchive( pMesh );
	m_MeshArchive = mesh_builder.GetArchive();

 	SetMiscMeshAttributes();

	return Result::SUCCESS;

/*	m_RequestedVertexFormatFlags
		= CMMA_VertexSet::VF_POSITION
		| CMMA_VertexSet::VF_DIFFUSE_COLOR
		| CMMA_VertexSet::VF_2D_TEXCOORD0;

	CMMA_VertexSet& vert_set = m_MeshArchive.GetVertexSet();

	// vertices

	const int num_vertices = num_sides + 2;
	vert_set.SetVertexFormat( m_RequestedVertexFormatFlags );
//	vert_set.Resize( num_sides + 2 );

	vert_set.vecPosition.resize( num_vertices );
	vert_set.vecDiffuseColor.resize( num_vertices );
	vert_set.vecTex.resize( 1 );
	vert_set.vecTex[0].resize( num_vertices );

	vert_set.vecPosition[0] = Vector3( 0.0f, 0.5f, 0.0f );
	vert_set.vecPosition[1] = Vector3( 0.0f,-0.5f, 0.0f );

	float fDeltaHeading = 2.0f * (float)PI / (float)num_sides;
	for( int i=0; i<num_sides; i++ )
	{
		vert_set.vecPosition[i+2]
		= Matrix33RotationY(fDeltaHeading * (float)i)
		* Vector3( 0.0f, 0.0f, 0.5f );
	}

	// texture coordinates
	for( int i=0; i<num_vertices; i++ )
	{
		vert_set.vecTex[0][i].u = ;
	}

	for( int 

	// triangles

	const int num_triangles = num_sides * 2;
	vector<uint>& indices = m_MeshArchive.GetVertexIndex();
	indices.reserve( num_triangles * 3 );
	for( int i=0; i<2; i++ )
	{
		int cone_top_index = i;
		for( int j=0; j<num_sides; j++ )
		{
			indices.push_back( cone_top_index );
			indices.push_back( i + 2 );
			indices.push_back( (i+1)%num_sides + 2 );
		}
	}

	vector<CMMA_Material>& mats = m_MeshArchive.GetMaterial();
	mats.resize( 1 );
	mats[0].vecTexture.resize( 1 );
//	mats[0].vecTexture[0].strFilename = m_ScrollTextureFilepath;
	mats[0].vecTexture[0].strFilename = m_TexturePath;

	return Result::SUCCESS;*/
}


DoubleConeScrollEffect::DoubleConeScrollEffect()
:
m_CameraPose( Matrix34Identity() ),
m_vCameraVelocity( Vector3(0,0,0) ),
m_fTexShiftV(0.0f)
{
}


/*
void DoubleConeScrollEffect::SetTextureFilepath( const std::string& tex_filepath ){}
*/
void DoubleConeScrollEffect::Update( float dt )
{
	m_fTexShiftV -= ( dt * 1.0f );

//	m_qTilt.Update( dt ); // How can I use critical damping for quaternion?
	m_TiltAngle.Update( dt );
}


void DoubleConeScrollEffect::Init()
{
	// Quaternion - how can I use critical damping with it?
//	m_qTilt.target.FromRotationMatrix( Matrix33Identity() );
//	m_qTilt.current.FromRotationMatrix( Matrix33Identity() );
//	m_qTilt.vel.FromRotationMatrix( Matrix33Identity() );
//	m_qTilt.smooth_time = 1.0f;

	m_TiltAngle.target = 0.0f;
	m_TiltAngle.current = 0.0f;
	m_TiltAngle.vel = 0.1f;
	m_TiltAngle.smooth_time = 2.0f;

	// create double cone mesh
	// - create with unit radius and height, scale at runtime

	MeshResourceDesc mesh_desc;
	shared_ptr<CDoubleConeMeshGenerator> pMeshGenerator( new CDoubleConeMeshGenerator );
	pMeshGenerator->SetPolygonDirection( MeshPolygonDirection::INWARD );
	pMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	pMeshGenerator->SetTexturePath( m_TextureFilepath );
	mesh_desc.pMeshGenerator = pMeshGenerator;
	bool res = m_DoubleConeMesh.Load( mesh_desc );
	if( !res )
		return;

	// load texture
	// - Done by the mesh
}


void DoubleConeScrollEffect::Render()
{
	GraphicsDevice().Disable( RenderStateType::LIGHTING );
//	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );

	GraphicsDevice().Disable( RenderStateType::FACE_CULLING );
//	GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );
	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

	CTextureStage ts;
	ts.ColorArg0 = TexStageArg::DIFFUSE;
	ts.ColorArg1 = TexStageArg::TEXTURE;
	ts.ColorOp   = TexStageOp::SELECT_ARG1;
	ts.AlphaArg0 = TexStageArg::DIFFUSE;
	ts.AlphaArg1 = TexStageArg::TEXTURE;
	ts.AlphaOp   = TexStageOp::SELECT_ARG1;
	GraphicsDevice().SetTextureStageParams( 0, ts );
	ts.ColorOp = TexStageOp::DISABLE;
	ts.AlphaOp = TexStageOp::DISABLE;
	GraphicsDevice().SetTextureStageParams( 1, ts );

	shared_ptr<BasicMesh> pMesh = m_DoubleConeMesh.GetMesh();
	if( !pMesh )
		return;

	// set up world transform

	Matrix33 matTilt( Matrix33Identity() );
	float fCamSpeed = Vec3Length( m_vCameraVelocity );
	Vector3 vInvDropDir = Vector3(0,1,0); /// The inverse of the direction of precipitation
	if( 0.001f < fCamSpeed )
	{
		const Vector3 vCamVelDir = m_vCameraVelocity / fCamSpeed;
		const Vector3 vRotAxis = Vec3Cross( vInvDropDir, vCamVelDir );
		float fTiltAngle = fCamSpeed * 0.1f;
		m_TiltAngle.target = fTiltAngle;
		matTilt = Matrix33RotationAxis( m_TiltAngle.current, vRotAxis );
//		matTilt = Matrix33RotationAxis( fTiltAngle, vRotAxis ); // no smoothing
//		m_qTilt.target.FromRotationMatrix( matTilt ); // How can I use critical damping for quaternion?
	}

	Matrix33 matScaling( Matrix33Identity() );
	matScaling(0,0) =  3.0f;
	matScaling(1,1) = 10.0f;
	matScaling(2,2) =  3.0f;

//	matTilt = m_qTilt.current.ToRotationMatrix();
	const Matrix34 world_transform(
		m_CameraPose.vPosition, // translation;
		matTilt * matScaling
		);

	CShaderManager& shader_mgr = FixedFunctionPipelineManager();
	shader_mgr.SetWorldTransform( world_transform );
//	shader_mgr.SetWorldTransform( Matrix34( Vector3(0,0,0), matScaling ) ); // Use this to see the geometry from outside
//	shader_mgr.SetWorldTransform( Matrix34Identity() );

	Matrix44 tex_translation( Matrix44Identity() );
	tex_translation( 1, 2 ) = m_fTexShiftV;
	GraphicsDevice().SetTextureCoordTrasnform( 0, tex_translation );

	CTextureTransformParams params;
	params.NumElements = 2;
	GraphicsDevice().SetTextureTrasnformParams( 0, params );

	pMesh->Render();

	GraphicsDevice().SetTextureCoordTrasnform( 0, Matrix44Identity() );
}


} // namespace amorphous
