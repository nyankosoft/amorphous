#include "PrimitiveShapeRenderer.hpp"
#include "Shader/ShaderManager.hpp"
#include "Shader/CommonShaders.hpp"
#include "TextureUtilities.hpp"
#include "MeshGenerators/MeshGenerators.hpp"
#include "PrimitiveRenderer.hpp"


namespace amorphous
{

using namespace std;


static ShaderHandle sg_NoLightingShader;


/// Used when a rectangle in 3D space is rendered via custom mesh
/// This function do not set vertex positions of the rect mesh,
/// since they are calculated when Draw3DRect() is called.
static void InitRectMesh( CustomMesh& mesh, const SFloatRGBAColor& vertex_diffuse_color )
{
	U32 vertex_format_flags
		= VFF::POSITION
		| VFF::NORMAL
		| VFF::DIFFUSE_COLOR
		| VFF::TEXCOORD2_0;

	mesh.InitVertexBuffer( 4, vertex_format_flags );

	mesh.SetNormals( Vector3(0,0,1) );

	mesh.SetDiffuseColors( vertex_diffuse_color );

	vector<unsigned int> indices;
	indices.resize( 6 );
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 2;

	mesh.SetIndices( indices );

	vector<CMMA_TriangleSet> triangle_sets;
	triangle_sets.resize( 1 );
	triangle_sets[0].m_iStartIndex             = 0;
	triangle_sets[0].m_iMinIndex               = 0;
	triangle_sets[0].m_iNumVertexBlocksToCover = 4;
	triangle_sets[0].m_iNumTriangles           = 2;
	triangle_sets[0].m_AABB                    = AABB3( Vector3(0,0,0), Vector3(0,0,0) );

	mesh.SetTriangleSets( triangle_sets );
}



CustomMesh PrimitiveShapeRenderer::ms_BoxMesh;

CustomMesh PrimitiveShapeRenderer::ms_RectMesh;


PrimitiveShapeRenderer::PrimitiveShapeRenderer()
{
//	ShaderResourceDesc desc;
//	desc = non_programmable;

//	m_Shader.Load( desc );
}

void PrimitiveShapeRenderer::RenderSphere( const Sphere& sphere, const SFloatRGBAColor& color )
{
}


void PrimitiveShapeRenderer::RenderBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
	ShaderManager *pShaderMgr = GetShaderManagerForPrimitiveShape();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	Vector3 s( vEdgeLengths );
	shader_mgr.SetWorldTransform( ToMatrix44(world_pose) * Matrix44Scaling(s.x,s.y,s.z) );

	if( ms_BoxMesh.GetNumVertices() == 0 )
	{
		BoxMeshGenerator generator;
		generator.Generate( Vector3(1,1,1), MeshGenerator::DEFAULT_VERTEX_FLAGS, color );
		C3DMeshModelArchive mesh_archive( generator.GetMeshArchive() );
		bool loaded = ms_BoxMesh.LoadFromArchive( mesh_archive );
	}
	else
		ms_BoxMesh.SetDiffuseColors( color );

	ms_BoxMesh.Render( shader_mgr );
}


void PrimitiveShapeRenderer::RenderCapsule( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


void PrimitiveShapeRenderer::RenderCylinder( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


ShaderManager *PrimitiveShapeRenderer::GetShaderManagerForPrimitiveShape()
{
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	if( pShaderMgr )
		return pShaderMgr;

	if( !sg_NoLightingShader.IsLoaded() )
		sg_NoLightingShader = GetNoLightingShader();

	return sg_NoLightingShader.GetShaderManager();
}


Result::Name PrimitiveShapeRenderer::RenderPlane(
	const Vector3 *positions,
	const Vector3& normal,
	const SFloatRGBAColor& color,
	TextureHandle& texture,
	const TEXCOORD2& top_left,
	const TEXCOORD2& bottom_right,
	ShaderManager& shader_mgr
	)
{
	if( ms_RectMesh.GetNumVertices() == 0 )
		InitRectMesh( ms_RectMesh, color );
	else
		ms_RectMesh.SetDiffuseColors( color );

	// Set the four vertices of the rectangle.
	const unsigned int num_vertices_to_set = 4;
	ms_RectMesh.SetPositions( positions, num_vertices_to_set );
	ms_RectMesh.UpdateAABBs();

	ms_RectMesh.SetNormals( normal );

	TEXCOORD2 tex_coords[4] =
	{
		TEXCOORD2( top_left.u,     top_left.v ),
		TEXCOORD2( bottom_right.u, top_left.v ),
		TEXCOORD2( bottom_right.u, bottom_right.v ),
		TEXCOORD2( top_left.u,     bottom_right.v )
	};

	ms_RectMesh.Set2DTexCoords( tex_coords, num_vertices_to_set, 0 );

//	if( ms_RectMesh.GetNumMaterials() == 0 )
//		return Result::UNKNOWN_ERROR;

	// Temporarily override the texture
	ms_RectMesh.Materials().resize( 1 );
	ms_RectMesh.Material(0).Texture.resize( 1 );
	ms_RectMesh.Material(0).Texture[0] = texture;

	ms_RectMesh.Render( shader_mgr );

	ms_RectMesh.Material(0).Texture[0] = TextureHandle();

	return Result::SUCCESS;
}


void PrimitiveShapeRenderer::RenderPlane(
	const Matrix34& pose,
	float width,
	float height,
	const SFloatRGBAColor& color,
	TextureHandle& texture,
	const TEXCOORD2& top_left,
	const TEXCOORD2& bottom_right
	)
{
	ShaderManager *pShaderMgr = GetShaderManagerForPrimitiveShape();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetWorldTransform( pose );

	float hr = width  * 0.5f; // horizontal raidus
	float vr = height * 0.5f; // vertical raidus

	const Vector3 positions[4] =
	{
		Vector3(-hr, vr,0),
		Vector3( hr, vr,0),
		Vector3( hr,-vr,0),
		Vector3(-hr,-vr,0)
	};

	const Vector3 normal = pose.matOrient.GetColumn(1);

	RenderPlane( positions, normal, color, texture, top_left, bottom_right, shader_mgr );
}


void PrimitiveShapeRenderer::RenderAxisAlignedPlane(
	uint axis,
	const Vector3& vCenter,
	float span_0,
	float span_1,
	const SFloatRGBAColor& color,
	TextureHandle& texture,
	const TEXCOORD2& top_left,
	const TEXCOORD2& bottom_right
	)
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

	corners[0][span_index_0] -= span_0 * 0.5f;
	corners[0][span_index_1] += span_1 * 0.5f;
	corners[1][span_index_0] += span_0 * 0.5f;
	corners[1][span_index_1] += span_1 * 0.5f;
	corners[2][span_index_0] += span_0 * 0.5f;
	corners[2][span_index_1] -= span_1 * 0.5f;
	corners[3][span_index_0] -= span_0 * 0.5f;
	corners[3][span_index_1] -= span_1 * 0.5f;

	TextureHandle default_texture;

	TextureHandle texture_to_set;
	if( texture.IsLoaded() )
		texture_to_set = texture;
	else
	{
		if( !default_texture.IsLoaded() )
			default_texture = CreateSingleColorTexture( SFloatRGBAColor::White(), 1, 1 );
		texture_to_set = default_texture;
	}

	ShaderManager *pShaderMgr = GetShaderManagerForPrimitiveShape();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

	shader_mgr.SetWorldTransform( Matrix44Identity() );

	Result::Name res = shader_mgr.SetTexture( 0, texture_to_set );

	// Commented out; lighting does not work in the OpenGL mode
	// when the texture is set to the second stage. 
//	res = shader_mgr.SetTexture( 1, texture_to_set );

	RenderPlane( corners, normal, color, texture_to_set, top_left, bottom_right, shader_mgr );
}


void PrimitiveShapeRenderer::RenderFloorPlane( const Vector3& vCenter, float width, float depth, const SFloatRGBAColor& color, TextureHandle& texture, const TEXCOORD2& top_left, const TEXCOORD2& bottom_right )
{
	RenderAxisAlignedPlane( 1, vCenter, width, depth, color, texture, top_left, bottom_right );
}


void PrimitiveShapeRenderer::RenderWireframeBox( const Vector3& vEdgeLengths, const Matrix34& world_pose, const SFloatRGBAColor& wireframe_color )
{
	ShaderManager *pShaderMgr = GetShaderManagerForPrimitiveShape();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

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


} // namespace amorphous
