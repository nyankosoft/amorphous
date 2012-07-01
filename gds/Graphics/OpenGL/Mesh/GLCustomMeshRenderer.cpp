#include "GLCustomMeshRenderer.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"


using namespace std;


CGLCustomMeshRenderer CGLCustomMeshRenderer::ms_Instance;


void CGLCustomMeshRenderer::RenderMesh( CCustomMesh& mesh )
{
	PROFILE_FUNCTION();

	uchar *pV = mesh.GetVertexBufferPtr();
	if( !pV )
		return;

	uchar *pI = mesh.GetIndexBufferPtr();
	if( !pI )
		return;

	uint num_indices = mesh.GetNumIndices();
	uint num_triangles = num_indices / 3;

	const uint num_verts = mesh.GetNumVertices();
	const int vertex_size = mesh.GetVertexSize();

	const U32 vert_flags = mesh.GetVertexFormatFlags();

	glEnableClientState(GL_VERTEX_ARRAY);

	uchar *pPos = pV + mesh.GetVertexElementOffset( VEE::POSITION );
	glVertexPointer( 3, GL_FLOAT, vertex_size, pPos );

	if( vert_flags & VFF::NORMAL )
	{
		glEnableClientState(GL_NORMAL_ARRAY);

		uchar *pNormal = pV + mesh.GetVertexElementOffset( VEE::NORMAL );
		glNormalPointer( GL_FLOAT, vertex_size, pNormal );
	}

	if( vert_flags & VFF::DIFFUSE_COLOR )
	{
		glEnableClientState(GL_COLOR_ARRAY);

		uchar *pDiffuseColor = pV + mesh.GetVertexElementOffset( VEE::DIFFUSE_COLOR );
		glColorPointer( 4, GL_FLOAT, vertex_size, pDiffuseColor );
	}

	const int num_mats = mesh.GetNumMaterials();
	for( int i=0; i<num_mats; i++ )
	{
		const CMeshMaterial& mat = mesh.GetMaterial(i);
		for( size_t j=0; j<mat.Texture.size(); j++ )
		{
			const GLuint tex_id = mat.Texture[j].GetGLTextureID();
			glBindTexture( GL_TEXTURE_2D, tex_id );
		}
	}

//	if(mTexId)
	if( vert_flags & VFF::TEXCOORD2_0 )
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		uchar *pTex = pV + mesh.GetVertexElementOffset( VEE::TEXCOORD2_0 );
		glTexCoordPointer( 2, GL_FLOAT, vertex_size, pTex );

		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, mTexId);
		glColor4f(1.0f, 1.0f, 1.0f,1.0f);
	}

	const uint index_size = mesh.GetIndexSize();
	uint index_type = GL_UNSIGNED_BYTE;
	switch( index_size )
	{
	case 1: index_type = GL_UNSIGNED_BYTE;  break;
	case 2: index_type = GL_UNSIGNED_SHORT; break;
	case 4: index_type = GL_UNSIGNED_INT;   break;
	default:
		break;
	}

//	if( use_draw_range_elements )
//	{
//		glDrawRangeElements( GL_TRIANGLES, 0, num_verts-1, num_triangles, index_type, pI );
//	}
//	else
	{
		PROFILE_SCOPE( "glDrawElements( GL_TRIANGLES, num_indices, index_type, pI )" );
		glDrawElements( GL_TRIANGLES, num_indices, index_type, pI );
	}

	if( vert_flags & VFF::TEXCOORD2_0 )
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		glDisable(GL_TEXTURE_2D);
	}

	if( vert_flags & VFF::DIFFUSE_COLOR )
		glDisableClientState(GL_COLOR_ARRAY);
	if( vert_flags & VFF::NORMAL )
		glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}


void CGLCustomMeshRenderer::RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr )
{
//	LOG_PRINT_ERROR( " Not implemented." );

	shader_mgr.Begin();

	RenderMesh( mesh );
}
