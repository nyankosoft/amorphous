#include "PrimitiveShapeRenderer.hpp"
#include "Shader/FixedFunctionPipelineManager.hpp"
#include "Shader/ShaderManager.hpp"
#include "TextureUtilities.hpp"
#include "MeshGenerators.hpp"
#include "3DRect.hpp"
#include "PrimitiveRenderer.hpp"

using namespace std;


/// Used when a rectangle in 3D space is rendered via custom mesh
/// This function do not set vertex positions of the rect mesh,
/// since they are calculated when Draw3DRect() is called.
static void InitRectMesh( CCustomMesh& mesh, const SFloatRGBAColor& vertex_diffuse_color )
{
	U32 vertex_format_flags
		= VFF::POSITION
		| VFF::NORMAL
		| VFF::DIFFUSE_COLOR
		| VFF::TEXCOORD2_0;

	mesh.InitVertexBuffer( 4, vertex_format_flags );

	vector<Vector3> normals;
	normals.resize( 4, Vector3(0,0,1) );
	mesh.SetNormals( normals );

	vector<SFloatRGBAColor> diffuse_colors;
	diffuse_colors.resize( 4, vertex_diffuse_color );
	mesh.SetDiffuseColors( diffuse_colors );

	vector<unsigned int> indices;
	indices.resize( 6 );
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;

	mesh.SetIndices<unsigned int>( indices );
}



CCustomMesh CPrimitiveShapeRenderer::ms_BoxMesh;

CCustomMesh CPrimitiveShapeRenderer::ms_RectMesh;


CPrimitiveShapeRenderer::CPrimitiveShapeRenderer()
{
//	CShaderResourceDesc desc;
//	desc = non_programmable;

//	m_Shader.Load( desc );
}

void CPrimitiveShapeRenderer::RenderSphere( const Sphere& sphere, const SFloatRGBAColor& color )
{
}


void CPrimitiveShapeRenderer::RenderBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
	CShaderManager& shader_mgr = m_Shader.GetShaderManager() ? *(m_Shader.GetShaderManager()) : FixedFunctionPipelineManager();

	Vector3 s( vEdgeLengths );
	shader_mgr.SetWorldTransform( ToMatrix44(world_pose) * Matrix44Scaling(s.x,s.y,s.z) );

	if( ms_BoxMesh.GetNumVertices() == 0 )
	{
		CBoxMeshGenerator generator;
		generator.Generate( Vector3(1,1,1), CMeshGenerator::DEFAULT_VERTEX_FLAGS, color );
		C3DMeshModelArchive mesh_archive( generator.GetMeshArchive() );
		bool loaded = ms_BoxMesh.LoadFromArchive( mesh_archive );
	}

	ms_BoxMesh.Render( shader_mgr );
}


void CPrimitiveShapeRenderer::RenderCapsule( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


void CPrimitiveShapeRenderer::RenderCylinder( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


void CPrimitiveShapeRenderer::RenderAxisAlignedPlane( uint axis, const Vector3& vCenter, float span_0, float span_1, const SFloatRGBAColor& color, CTextureHandle& texture, const TEXCOORD2& top_left, const TEXCOORD2& bottom_right )
{
	if( 6 <= axis )
		return;

	uint uaxis = axis % 3;

	uint span_index_0 = (uaxis+1) % 3;
	uint span_index_1 = (uaxis+2) % 3;

	if( 3 <= axis )
		std::swap( span_index_0, span_index_1 );

	Vector3 normal( Vector3(0,0,0) );
	normal[uaxis] = (axis < 3) ? 1.0f : -1.0f;

	Vector3 corners[4] =
	{
		vCenter,
		vCenter,
		vCenter,
		vCenter
	};
	corners[0][span_index_0] += span_0 * 0.5f;
	corners[0][span_index_1] += span_1 * 0.5f;
	corners[1][span_index_0] -= span_0 * 0.5f;
	corners[1][span_index_1] += span_1 * 0.5f;
	corners[2][span_index_0] -= span_0 * 0.5f;
	corners[2][span_index_1] -= span_1 * 0.5f;
	corners[3][span_index_0] += span_0 * 0.5f;
	corners[3][span_index_1] -= span_1 * 0.5f;

	CTextureHandle default_texture;

	CTextureHandle texture_to_set;
	if( texture.IsLoaded() )
		texture_to_set = texture;
	else
	{
		if( !default_texture.IsLoaded() )
			default_texture = CreateSingleColorTexture( SFloatRGBAColor::White(), 1, 1 );
		texture_to_set = default_texture;
	}

	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();
	shader_mgr.SetTexture( 0, texture_to_set );

	C3DRect rect;
	rect.SetPositions( corners );
	rect.SetColor( color );
	rect.SetNormal( normal );
	rect.SetTextureUV( top_left, bottom_right );

	rect.Draw();
}


void CPrimitiveShapeRenderer::RenderFloorPlane( const Vector3& vCenter, float width, float depth, const SFloatRGBAColor& color, CTextureHandle& texture, const TEXCOORD2& top_left, const TEXCOORD2& bottom_right )
{
	RenderAxisAlignedPlane( 1, vCenter, width, depth, color, texture, top_left, bottom_right );
}


void CPrimitiveShapeRenderer::RenderWireframeBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& wireframe_color )
{
	CShaderManager& shader_mgr = m_Shader.GetShaderManager() ? *(m_Shader.GetShaderManager()) : FixedFunctionPipelineManager();

	Vector3 r( vEdgeLengths * 0.5f );
	shader_mgr.SetWorldTransform( ToMatrix44(world_pose) * Matrix44Scaling(r.x,r.y,r.z) );

//	Vector3 radii = vEdgeLengths * 0.5f;
	Vector3 vertices[8] =
	{
		Vector3( 1, 1, 1),// * radii,
		Vector3( 1, 1,-1),// * radii,
		Vector3(-1, 1,-1),// * radii,
		Vector3(-1, 1, 1),// * radii,
		Vector3( 1,-1, 1),// * radii,
		Vector3( 1,-1,-1),// * radii,
		Vector3(-1,-1,-1),// * radii,
		Vector3(-1,-1, 1),// * radii,
	};
/*
	uint indices[24] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		0, 1, 1, 2, 2, 3, 3, 0,
		0, 1, 1, 2, 2, 3, 3, 0
	};

	GetPrimitiveRenderer().DrawIndexedLines( vertices, 8, indices, 24, wireframe_color );
*/
	Vector3 *v = vertices;
	const Vector3 points[24] =
	{
		v[0], v[1], v[1], v[2], v[2], v[3], v[3], v[0],
		v[0], v[4], v[1], v[5], v[2], v[6], v[3], v[7],
		v[4], v[5], v[5], v[6], v[6], v[7], v[7], v[4],
	};

	const int num_edges = 12;
	for( int i=0; i<num_edges; i++ )
	{
		GetPrimitiveRenderer().DrawLine( points[i*2], points[i*2+1], wireframe_color, wireframe_color );
	}
}


void CPrimitiveShapeRenderer::RenderPlane(
	const Matrix34& pose,
	float width,
	float height,
	const SFloatRGBAColor& color,
	CTextureHandle& tex,
	const TEXCOORD2& top_left,
	const TEXCOORD2& bottom_right
	)
{
	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager& shader_mgr = pShaderMgr ? (*pShaderMgr) : FixedFunctionPipelineManager();

	shader_mgr.SetWorldTransform( pose );

	float hr = width  * 0.5f; // horizontal raidus
	float vr = height * 0.5f; // vertical raidus

	if( ms_RectMesh.GetNumVertices() == 0 )
		InitRectMesh( ms_RectMesh, color );

	vector<Vector3> positions;
	vector<TEXCOORD2> tex_coords;
	positions.resize( 4 );
	tex_coords.resize( 4 );

	positions[0] = Vector3(-hr, vr,0);
	positions[1] = Vector3( hr, vr,0);
	positions[2] = Vector3( hr,-vr,0);
	positions[3] = Vector3(-hr,-vr,0);

	tex_coords[0] = TEXCOORD2( top_left.u,     top_left.v );
	tex_coords[1] = TEXCOORD2( bottom_right.u, top_left.v );
	tex_coords[2] = TEXCOORD2( bottom_right.u, bottom_right.v );
	tex_coords[3] = TEXCOORD2( top_left.u,     bottom_right.v );

	ms_RectMesh.SetPositions( positions );
	ms_RectMesh.Set2DTexCoords( tex_coords, 0 );

//	if( ms_RectMesh.GetNumMaterials() == 0 )
//		return Result::UNKNOWN_ERROR;

	// Temporarily override the texture
	ms_RectMesh.Materials().resize( 1 );
	ms_RectMesh.Material(0).Texture.resize( 1 );
	ms_RectMesh.Material(0).Texture[0] = tex;

	ms_RectMesh.Render( shader_mgr );

	ms_RectMesh.Material(0).Texture[0] = CTextureHandle();
}
