#include "2DPrimitiveRenderer_GL.hpp"
#include "../GLGraphicsDevice.hpp"
//#include "App/GameWindowManager_Win32_GL.hpp"
#include "3DMath/Matrix44.hpp"
#include "Graphics/OpenGL/GLExtensions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{


void RenderViaVertexAttribArray( const General2DVertex *pVertices, uint num_vertices, GLenum primitive_type )
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
	ushort triangle_fan[] = {0,1,2,3};
	GLsizei num_indices = 4;
	switch( primitive_type )
	{
	case GL_TRIANGLE_STRIP:
		pI = (uchar *)triangle_fan;
		break;
	case GL_TRIANGLE_FAN:
		pI = (uchar *)triangle_fan;
		break;
	case GL_TRIANGLES:
		break;
	default:
		break;
	}

	if( !pI )
		return;

	GLenum index_type = GL_UNSIGNED_SHORT;

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	glDrawElements( GL_TRIANGLE_FAN, num_indices, index_type, pI );
//	glDrawArrays( GL_TRIANGLE_FAN, 0, 2 );

	LOG_GL_ERROR( "glDrawElements() failed." );
}


GLenum ToGLPrimitiveType( PrimitiveType::Name pt )
{
	switch( pt )
	{
	case PrimitiveType::TRIANGLE_LIST:  return GL_TRIANGLES;
	case PrimitiveType::TRIANGLE_FAN:   return GL_TRIANGLE_FAN;
	case PrimitiveType::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
	default:
		return GL_TRIANGLES;
	}
}



void C2DPrimitiveRenderer_GL::Init()
{
}


void C2DPrimitiveRenderer_GL::Release()
{
}


void C2DPrimitiveRenderer_GL::Render( General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	RenderGL( paVertex, num_vertices, ToGLPrimitiveType( primitive_type ) );
}


void C2DPrimitiveRenderer_GL::Render( ShaderManager& rShaderManager, General2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
}


void C2DPrimitiveRenderer_GL::RenderRect( ShaderManager& rShaderManager, C2DRect& rect )
{
	rShaderManager.Begin();
//	glUseProgram( 0 );

	glDisable( GL_CULL_FACE );

	RenderViaVertexAttribArray( &(rect.GetVertex(0)), 4, GL_TRIANGLE_FAN );
//	rect.draw();
}


void C2DPrimitiveRenderer_GL::RenderGL( General2DVertex *paVertex, int num_vertices, GLenum primitive_type )
{
	PROFILE_FUNCTION_IF( m_Profile );

	if( !paVertex )
		return;

//	GLenum gl_pt = ToGLPrimitiveType( primitive_type );
	GLenum gl_pt = primitive_type;

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
//	glTranslatef(0.0f,0.0f,-6.0f);

	Matrix44 matModelView, matProj;
	GLint view[4];

	glGetFloatv( GL_MODELVIEW_MATRIX, (GLfloat *)&matModelView );
	glGetFloatv( GL_PROJECTION_MATRIX, (GLfloat *)&matProj );
	glGetIntegerv( GL_VIEWPORT, view );

	Matrix44 matIdentity = Matrix44Identity();
	matIdentity( 0, 0 ) =  2.0f / view[2];
	matIdentity( 1, 1 ) = -2.0f / view[3];
	matIdentity( 0, 3 ) = -1;
	matIdentity( 1, 3 ) = 1;
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( (GLfloat *)&matIdentity );

	glDisable( GL_CULL_FACE );

	if( glUseProgramObjectARB )
		glUseProgramObjectARB( 0 );


/*
	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
		glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
		glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
		glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
	glEnd();											// Done Drawing The Quad
*/
	glBegin(gl_pt);							// Drawing Using Triangles

//	int w = GameWindowManager_Win32().GetScreenWidth();
//	int h = GameWindowManager_Win32().GetScreenHeight();

//	Vector3 saved_pos[20];
//	memset(saved_pos, 0, sizeof(saved_pos));

	for( int i=0; i<num_vertices /*&& i<numof(saved_pos)*/; i++ )
	{
		const SFloatRGBAColor& color = paVertex[i].m_DiffuseColor;
		glColor4f( color.red, color.green, color.blue, color.alpha );
//		glColor3f( color.red, color.green, color.blue );

		const TEXCOORD2& tex = paVertex[i].m_TextureCoord[0];
		glTexCoord2f( tex.u, tex.v );

		const Vector3& pos = paVertex[i].m_vPosition;
/*		Vector3 dest;
		dest.x = pos.x / w;
		dest.y = pos.y / h * (-1.0f);
		dest.z = pos.z;

		saved_pos[i] = pos;
*/
		glVertex3f( pos.x, pos.y, pos.z );
//		glVertex3f( dest.x, dest.y, dest.z );
	}

	glEnd();											// Finished Drawing The Triangle

	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( (GLfloat *)&matProj );
}


} // namespace amorphous
