#include "PrimitiveShapeRenderer.hpp"
#include "Shader/FixedFunctionPipelineManager.hpp"
#include "Shader/ShaderManager.hpp"
#include "TextureHandle.hpp"
#include "MeshGenerators.hpp"
#include "3DRect.hpp"

using namespace std;


CTextureHandle CreateSingleColorTexture( uint size_x = 1, uint size_y = 1, const SFloatRGBAColor& color = SFloatRGBAColor::White() )
{
	CTextureResourceDesc desc;
	desc.Width     = size_x;
	desc.Height    = size_y;
	desc.MipLevels = 0;
	desc.Format    = TextureFormat::A8R8G8B8;
	desc.pLoader.reset( new CSignleColorTextureFilling( color ) );

	CTextureHandle tex;
	bool loaded = tex.Load( desc );

	return tex;
}



CCustomMesh CPrimitiveShapeRenderer::ms_BoxMesh;


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

	ms_BoxMesh.Render();
}


void CPrimitiveShapeRenderer::RenderCapsule( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


void CPrimitiveShapeRenderer::RenderCylinder( float radius, float height, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
}


void CPrimitiveShapeRenderer::RenderAxisAlignedPlane( uint axis, const Vector3& vCenter, float span_0, float span_1, const SFloatRGBAColor& color )
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

	CTextureHandle s_Texture;
	if( !s_Texture.IsLoaded() )
		s_Texture = CreateSingleColorTexture( 1, 1, SFloatRGBAColor::White() );

	FixedFunctionPipelineManager().SetTexture( 0, s_Texture );

	C3DRect rect;
	rect.SetPositions( corners );
	rect.SetColor( color );
	rect.SetNormal( normal );

	rect.Draw();
}
