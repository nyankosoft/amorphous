#include "2DPrimitiveRenderer_GL.hpp"
#include "../GLGraphicsDevice.hpp"
//#include "App/GameWindowManager_Win32_GL.hpp"
#include "3DMath/Matrix44.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/GenericShaderHelpers.hpp"
#include "Graphics/Shader/Generic2DShaderGenerator.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DRoundRect.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/wavefront_obj_output.hpp"


namespace amorphous
{


class C2DPrimitiveCommonShaders
{
public:
	enum ShaderType
	{
		ST_DIFFUSE_COLOR,
		ST_DIFFUSE_COLOR_AND_TEXTURE,
		ST_DIFFUSE_COLOR_AND_2_TEXTURES,
		NUM_SHADERS
	};
private:

	ShaderHandle m_Shaders[NUM_SHADERS];

	int m_Initialized[NUM_SHADERS];

public:

	void Initialize( ShaderType type )
	{
		Generic2DShaderDesc shader_desc;

		switch( type )
		{
		case ST_DIFFUSE_COLOR:
			break;

		case ST_DIFFUSE_COLOR_AND_TEXTURE:
			shader_desc.diffuse_color_and_tex0_blend.rgb.op = '*';
			shader_desc.diffuse_color_and_tex0_blend.alpha.op = '*';
			shader_desc.textures[0].sampler = 0;
			shader_desc.textures[0].coord   = 0;
			break;

		case ST_DIFFUSE_COLOR_AND_2_TEXTURES:
			shader_desc.diffuse_color_and_tex0_blend.rgb.op = '*';
			shader_desc.diffuse_color_and_tex0_blend.alpha.op = '*';
			shader_desc.textures[0].sampler = 0;
			shader_desc.textures[0].coord   = 0;
			shader_desc.tex0_and_tex1_blend.rgb.op = '*';
			shader_desc.tex0_and_tex1_blend.alpha.op = '*';
			shader_desc.textures[1].sampler = 1;
			shader_desc.textures[1].coord   = 0;
			break;

		default:
			break;
		}

		m_Shaders[type] = CreateGeneric2DShader( shader_desc );

		if( !m_Shaders[type].IsLoaded() )
			return;

		m_Initialized[type] = true;
	}

	ShaderHandle GetShader( ShaderType type )
	{
		if( !m_Initialized[type] )
			Initialize( type );

		return m_Shaders[type];
	}
};


static C2DPrimitiveCommonShaders sg_2DPrimitiveCommonShaders;


GLenum ToGLPrimitiveType( PrimitiveType::Name pt )
{
	switch( pt )
	{
	case PrimitiveType::TRIANGLE_LIST:  return GL_TRIANGLES;
	case PrimitiveType::TRIANGLE_FAN:   return GL_TRIANGLE_FAN;
	case PrimitiveType::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
	default:
		LOG_PRINT_ERROR( "An unsupported primitive type: " + to_string((int)pt) );
		return GL_TRIANGLES;
	}
}



void C2DPrimitiveRenderer_GL::Init()
{
}


void C2DPrimitiveRenderer_GL::Release()
{
}


void C2DPrimitiveRenderer_GL::RenderViaVertexAttribArray(
	const General2DVertex *pVertices,
	uint num_vertices,
	ushort *indices,
	uint num_indices,
	GLenum primitive_type
	)
{
	const uint vertex_size = sizeof(General2DVertex);

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	// Unbind GL_ARRAY_BUFFER and GL_ELEMENT_ARRAY_BUFFER to source a standard memory location (RAM).
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// position
	const uchar *pV = (uchar *)pVertices;
	const uchar *pPos = pV + 0;//mesh.GetVertexElementOffset( VEE::POSITION );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, vertex_size, pPos );

	// diffuse color
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, vertex_size, pV + sizeof(float) * 4 );

	// 2D texture coordinates
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, vertex_size, pV + sizeof(float) * 8 );

	uchar *pI = NULL;


	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	if( indices )
	{
		pI = (uchar *)indices;

		GLenum index_type = GL_UNSIGNED_SHORT;

		glDrawElements( primitive_type, num_indices, index_type, pI );

		LOG_GL_ERROR( "glDrawElements() failed." );
	}
	else
	{
		GLint first = 0;
		GLsizei count = 0;
		switch( primitive_type )
		{
		case GL_TRIANGLE_STRIP: count = num_vertices;     break;
		case GL_TRIANGLE_FAN:   count = num_vertices;     break;
		case GL_TRIANGLES:      count = num_vertices;     break;
			break;
		default:
			break;
		}

		glDrawArrays( primitive_type, first, count );

		LOG_GL_ERROR( "glDrawArrays() failed." );
	}
}


void C2DPrimitiveRenderer_GL::Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	ShaderHandle shader
		= sg_2DPrimitiveCommonShaders.GetShader( C2DPrimitiveCommonShaders::ST_DIFFUSE_COLOR );

	ShaderManager *pShaderMgr = shader.GetShaderManager();

	if( !pShaderMgr )
	{
		LOG_PRINT_ERROR( "The shader for 2D primitives is not available." );
		return;
	}

	pShaderMgr->Begin();

	RenderViaVertexAttribArray( paVertex, num_vertices, NULL, 0, ToGLPrimitiveType(primitive_type) );
}


void C2DPrimitiveRenderer_GL::Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type, const TextureHandle& texture )
{
	ShaderHandle shader
		= sg_2DPrimitiveCommonShaders.GetShader( C2DPrimitiveCommonShaders::ST_DIFFUSE_COLOR_AND_TEXTURE );

	ShaderManager *pShaderMgr = shader.GetShaderManager();

	if( !pShaderMgr )
	{
		LOG_PRINT_ERROR( "The shader for single texture 2D primitives is not available." );
		return;
	}

	pShaderMgr->SetTexture( 0, texture );
	pShaderMgr->Begin();

	RenderViaVertexAttribArray( paVertex, num_vertices, NULL, 0, ToGLPrimitiveType(primitive_type) );
}


void C2DPrimitiveRenderer_GL::Render( const General2DVertex *pVertices, uint num_vertices, U16 *indices, uint num_indices, PrimitiveType::Name primitive_type )
{
	RenderViaVertexAttribArray( pVertices, num_vertices, indices, num_indices, ToGLPrimitiveType(primitive_type) );
}


void C2DPrimitiveRenderer_GL::Render( const General2DVertex *pVertices, uint num_vertices, U16 *indices, uint num_indices, PrimitiveType::Name primitive_type, TextureHandle texture )
{
	ShaderHandle shader
		= sg_2DPrimitiveCommonShaders.GetShader( C2DPrimitiveCommonShaders::ST_DIFFUSE_COLOR_AND_TEXTURE );

	ShaderManager *pShaderMgr = shader.GetShaderManager();

	if( !pShaderMgr )
	{
		return;
	}

	pShaderMgr->SetTexture( 0, texture );

	pShaderMgr->Begin();

	RenderViaVertexAttribArray( pVertices, num_vertices, indices, num_indices, ToGLPrimitiveType(primitive_type) );
}


void C2DPrimitiveRenderer_GL::Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	rShaderManager.Begin();

	RenderViaVertexAttribArray( paVertex, num_vertices, NULL, 0, ToGLPrimitiveType( primitive_type ) );
}


void C2DPrimitiveRenderer_GL::RenderRect( ShaderManager& rShaderManager, C2DRect& rect )
{
	rShaderManager.Begin();
//	glUseProgram( 0 );

	glDisable( GL_CULL_FACE );

//	ushort triangle_fan[] = {0,1,2,3};
//	GLsizei num_indices = 4;

//	RenderViaVertexAttribArray( &(rect.GetVertex(0)), 4, triangle_fan, num_indices, GL_TRIANGLE_FAN );
	RenderViaVertexAttribArray( &(rect.GetVertex(0)), 4, NULL, 0, GL_TRIANGLE_FAN );
}


} // namespace amorphous
