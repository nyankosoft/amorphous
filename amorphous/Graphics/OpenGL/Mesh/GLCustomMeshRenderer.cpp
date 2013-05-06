#include "GLCustomMeshRenderer.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/OpenGL/GLTextureResourceVisitor.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"


namespace amorphous
{


using namespace std;


CGLCustomMeshRenderer CGLCustomMeshRenderer::ms_Instance;


void CGLCustomMeshRenderer::RenderMeshWithCurrentProgram( CustomMesh& mesh )
{
	// NOTE: it seems that the fixed function pipeline does not work
	// with glEnableVertexAttribArray() & glVertexAttribPointer().

	PROFILE_FUNCTION();

	uchar *pV = mesh.GetVertexBufferPtr();
	if( !pV )
		return;

	uchar *pI = mesh.GetIndexBufferPtr();
	if( !pI )
		return;

	const uint num_indices = mesh.GetNumIndices();
	const uint num_triangles = num_indices / 3;

	const uint num_verts = mesh.GetNumVertices();
	const int vertex_size = mesh.GetVertexSize();

	const U32 vert_flags = mesh.GetVertexFormatFlags();

	static const bool use_vertex_attrib_array_and_ptr = true;

	// Unbind GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER to source a standard memory location (RAM).
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	if( vert_flags & VFF::POSITION )
	{
		uchar *pPos = pV + mesh.GetVertexElementOffset( VEE::POSITION );

		if( use_vertex_attrib_array_and_ptr )
		{
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertex_size, pPos );
		}
		else
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer( 3, GL_FLOAT, vertex_size, pPos );
		}
	}
	else
	{
		LOG_PRINT_ERROR( " A vertex must at least have a position." );
		return;
	}


	if( vert_flags & VFF::NORMAL )
	{
		uchar *pNormal = pV + mesh.GetVertexElementOffset( VEE::NORMAL );

		if( use_vertex_attrib_array_and_ptr )
		{
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, vertex_size, pNormal );
		}
		else
		{
			glDisableClientState(GL_NORMAL_ARRAY);
//			glEnableClientState(GL_NORMAL_ARRAY);
//			glNormalPointer( GL_FLOAT, vertex_size, pNormal );
		}
	}

	if( vert_flags & VFF::DIFFUSE_COLOR )
	{
		uchar *pDiffuseColor = pV + mesh.GetVertexElementOffset( VEE::DIFFUSE_COLOR );

		if( use_vertex_attrib_array_and_ptr )
		{
			glEnableVertexAttribArray( 2 );
			glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, vertex_size, pDiffuseColor );
		}
		else
		{
			glDisableClientState(GL_COLOR_ARRAY);
//			glEnableClientState(GL_COLOR_ARRAY);
//			glColorPointer( 4, GL_FLOAT, vertex_size, pDiffuseColor );
		}
	}

	const int num_mats = mesh.GetNumMaterials();
	for( int i=0; i<num_mats; i++ )
	{
		const MeshMaterial& mat = mesh.GetMaterial(i);
		for( size_t j=0; j<mat.Texture.size(); j++ )
		{
			SetTextureGL_FFP( j, mat.Texture[j] );
		}
	}

//	if(mTexId)
	if( vert_flags & VFF::TEXCOORD2_0 )
	{
		uchar *pTex = pV + mesh.GetVertexElementOffset( VEE::TEXCOORD2_0 );

		if( use_vertex_attrib_array_and_ptr )
		{
			glEnableVertexAttribArray( 3 );
			glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, vertex_size, pTex );
		}
		else
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//			glTexCoordPointer( 2, GL_FLOAT, vertex_size, pTex );
		}

		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, mTexId);
		/*glColor4f(1.0f, 1.0f, 1.0f,1.0f);*/
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

//	if( vert_flags & VFF::DIFFUSE_COLOR )
//		glDisableClientState(GL_COLOR_ARRAY);
//	if( vert_flags & VFF::NORMAL )
//		glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
}


void CGLCustomMeshRenderer::RenderSubset( CustomMesh& mesh, int subset_index )
{
	LOG_PRINT_ERROR( " Not implemented." );
}


void CGLCustomMeshRenderer::RenderMesh( CustomMesh& mesh )
{
	glUseProgram( 0 );

	RenderMeshWithCurrentProgram( mesh );
}


void CGLCustomMeshRenderer::RenderMesh( CustomMesh& mesh, ShaderManager& shader_mgr )
{
//	LOG_PRINT_ERROR( " Not implemented." );

	shader_mgr.Begin();

	bool using_programmable_shader = (&shader_mgr != &FixedFunctionPipelineManager()) ? true : false;

	RenderMeshWithCurrentProgram( mesh );//, using_programmable_shader );
}


void CGLCustomMeshRenderer::RenderSubset( CustomMesh& mesh, ShaderManager& shader_mgr, int subset_index )
{
	LOG_PRINT_ERROR( " Not implemented." );
}


} // namespace amorphous
