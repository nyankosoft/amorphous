#include "GLGraphicsDevice.hpp"
#include "GLExtensions.hpp"
#include "../TextureStage.hpp"
#include "../FogParams.hpp"
#include "../../3DMath/Matrix44.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/Macro.h"
#include "../../Support/Profile.hpp"


namespace amorphous
{

using namespace std;


const char *get_gl_error_text( GLenum gl_error )
{
	switch( gl_error )
	{
	case GL_NO_ERROR:          return "GL_NO_ERROR";
	case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
	default: return "unknown error";
	}
}


void LogGLError( const char *fname, const char *msg )
{
//	GLenum error_flags[12];

	string errors_text;
	while(1)
	{
		GLenum gl_error = glGetError();
		if( gl_error == GL_NO_ERROR )
			break;
		else
			errors_text += string( 0 < errors_text.length() ? "|" : "" ) + get_gl_error_text(gl_error);
	}

	if( 0 < errors_text.length() )
		LOG_PRINT_ERROR( " OpenGL error(s) '" + errors_text + "' in function " + string(fname) + "() - " + string(msg) );
}


static inline GLenum ToGLAlphaFunc( CompareFunc::Name alpha_func )
{
	switch( alpha_func )
	{
	case CompareFunc::ALWAYS :                  return GL_ALWAYS;
	case CompareFunc::NEVER:                    return GL_NEVER;
	case CompareFunc::LESS_THAN:                return GL_LESS;
	case CompareFunc::LESS_THAN_OR_EQUAL_TO:    return GL_LEQUAL;
	case CompareFunc::GREATER_THAN:             return GL_GREATER;
	case CompareFunc::GREATER_THAN_OR_EQUAL_TO: return GL_GEQUAL;
	default: return GL_ALWAYS;
	}
}



//========================================================================
// CGLGraphicsDevice
//========================================================================

// definition of singleton instance
CGLGraphicsDevice CGLGraphicsDevice::ms_CGLGraphicsDevice_;


CGLGraphicsDevice::CGLGraphicsDevice()
:
m_AlphaFunc(GL_ALWAYS),
m_fReferenceAlphaValue(0.0f)
{
	m_SourceBlend = ToGLBlendModeEnum( AlphaBlend::SrcAlpha );
	m_DestBlend   = ToGLBlendModeEnum( AlphaBlend::InvSrcAlpha );
}


bool CGLGraphicsDevice::Init( int iWindowWidth, int iWindowHeight, ScreenMode::Name screen_mode )
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glEnable(GL_TEXTURE_2D);
	LOG_GL_ERROR( "glEnable() failed." );

	m_State = CGraphicsDevice::STATE_INITIALIZED;

	return true;
}


void CGLGraphicsDevice::SetDefaultRenderStates()
{
	// some default render states
}


void CGLGraphicsDevice::Release()
{
}


void CGLGraphicsDevice::GetAdapterModesForDefaultAdapter( std::vector<CAdapterMode>& dest_buffer )
{
	LOG_PRINT_ERROR( " Not implemented." );
/*	if( m_vecAdapterMode.size() == 0 )
		LOG_PRINT_WARNING( "No adapeter modes are in m_vecAdapterMode." );

	dest_buffer = m_vecAdapterMode;*/
}


bool CGLGraphicsDevice::IsCurrentDisplayMode( const CDisplayMode& display_mode ) const
{
	LOG_PRINT_ERROR( " Not implemented." );

	return false;
}


void CGLGraphicsDevice::EnumAdapterModesForDefaultAdapter()
{
}


Result::Name CGLGraphicsDevice::SetTexture( int stage, const CTextureHandle& texture )
{
//	PROFILE_FUNCTION();

	glBindTexture( GL_TEXTURE_2D, texture.GetGLTextureID() );

	LOG_GL_ERROR( "glBindTexture() failed." );

	return Result::SUCCESS;
}


GLint ToGLCombine( int operation )
{
	return GL_MODULATE;
}


GLint ToGLTextureStageSource( int src )
{
	switch( src )
	{
	case TexStageArg::DIFFUSE:  return GL_PRIMARY_COLOR;
	case TexStageArg::PREV:     return GL_PREVIOUS;
	case TexStageArg::TEXTURE:  return GL_TEXTURE;
	default:
		return GL_TEXTURE;
	}
}


GLint ToGLOperand( int src )
{
	return 0;
}


Result::Name CGLGraphicsDevice::SetTextureStageParams( uint stage, const CTextureStage& params )
{
	LOG_PRINT_ERROR( " Not implemented yet." );
//	return Result::UNKNOWN_ERROR;

	if( stage == 0 )
	{
//		glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE,   GL_COMBINE);
		glTexEnvi(GL_TEXTURE_2D, GL_COMBINE_RGB_ARB,    GL_MODULATE);
		glTexEnvi(GL_TEXTURE_2D, GL_SOURCE0_RGB_ARB,    ToGLTextureStageSource(params.ColorArg0) );
		glTexEnvi(GL_TEXTURE_2D, GL_OPERAND0_RGB_ARB,   GL_SRC_COLOR); // vertex colors (lit portion)
		glTexEnvi(GL_TEXTURE_2D, GL_SOURCE1_RGB_ARB,    ToGLTextureStageSource(params.ColorArg1) );
		glTexEnvi(GL_TEXTURE_2D, GL_OPERAND1_RGB_ARB,   GL_SRC_COLOR); // this texture's colors (stage 0)
//		glTexEnvi(GL_TEXTURE_2D, GL_COMBINE_ALPHA_ARB,  GL_MODULATE);
//		glTexEnvi(GL_TEXTURE_2D, GL_SOURCE0_ALPHA_ARB,  GL_PRIMARY_COLOR);
//		glTexEnvi(GL_TEXTURE_2D, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA); // vertex alpha
//		glTexEnvi(GL_TEXTURE_2D, GL_SOURCE1_ALPHA_ARB,  GL_TEXTURE);
//		glTexEnvi(GL_TEXTURE_2D, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA); // this texture's alpha (stage 0)
	}
	return Result::SUCCESS;
/*
	glActiveTextureARB( GL_TEXTURE0_ARB + stage );	 // start populating the stage
	glEnable(GL_TEXTURE_2D);

	glBindTexture (GL_TEXTURE_2D, texture0ID); // lit texture

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,   GL_COMBINE);
	glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,    ToGLCombine( params.ColorOp ) );

	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,    ToGLTextureStageSource( params.ColorArg0 ) );
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,   GL_SRC_COLOR );
	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,    ToGLTextureStageSource( params.ColorArg1 ) );
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,   GL_SRC_COLOR );

	glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB,  GL_MODULATE);

	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB,  ToGLTextureStageSource( params.AlphaArg0 ) );
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA ); // vertex alpha
	glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB,  ToGLTextureStageSource( params.AlphaArg1 ) );
	glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA ); // this texture's alpha (stage 0)

	//glBindTexture (GL_TEXTURE_2D, texture0ID); // lit texture
	//glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE,   GL_COMBINE);
	//glTexEnvi(GL_TEXTURE_2D, GL_COMBINE_RGB_ARB,    GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_2D, GL_SOURCE0_RGB_ARB,    GL_PRIMARY_COLOR);
	//glTexEnvi(GL_TEXTURE_2D, GL_OPERAND0_RGB_ARB,   GL_SRC_COLOR); // vertex colors (lit portion)
	//glTexEnvi(GL_TEXTURE_2D, GL_SOURCE1_RGB_ARB,    GL_TEXTURE);
	//glTexEnvi(GL_TEXTURE_2D, GL_OPERAND1_RGB_ARB,   GL_SRC_COLOR); // this texture's colors (stage 0)
	//glTexEnvi(GL_TEXTURE_2D, GL_COMBINE_ALPHA_ARB,  GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_2D, GL_SOURCE0_ALPHA_ARB,  GL_PRIMARY_COLOR);
	//glTexEnvi(GL_TEXTURE_2D, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA); // vertex alpha
	//glTexEnvi(GL_TEXTURE_2D, GL_SOURCE1_ALPHA_ARB,  GL_TEXTURE);
	//glTexEnvi(GL_TEXTURE_2D, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA); // this texture's alpha (stage 0)

	return Result::SUCCESS;*/
}


Result::Name CGLGraphicsDevice::SetTextureTrasnformParams( uint stage, const CTextureTransformParams& params )
{
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetTextureCoordTrasnform( uint stage, const Matrix44& transform )
{
	glMatrixMode( GL_TEXTURE );

	glLoadMatrixf( transform.GetData() );

	return Result::SUCCESS;
}


bool ToGLenum( RenderStateType::Name type, GLenum& cap )
{
	switch(type)
	{
	case RenderStateType::DEPTH_TEST:                cap = GL_DEPTH_TEST;   return true;
	case RenderStateType::ALPHA_BLEND:               cap = GL_BLEND;        return true;
	case RenderStateType::ALPHA_TEST:                cap = GL_ALPHA_TEST;   return true;
	case RenderStateType::LIGHTING:	                 cap = GL_LIGHTING;     return true;
	case RenderStateType::FOG:                       cap = GL_FOG;          return true;
	case RenderStateType::FACE_CULLING:              cap = GL_CULL_FACE;    return true;
	case RenderStateType::WRITING_INTO_DEPTH_BUFFER: // Handled separately. See GetRenderState() & SetRenderState()
	case RenderStateType::SCISSOR_TEST:              cap = GL_SCISSOR_TEST; return true;
	default:
		return false;
	}
}


bool CGLGraphicsDevice::GetRenderState( RenderStateType::Name type )
{
	GLenum cap = 0;
	bool res = ToGLenum( type, cap );

	if( res )
	{
		return false;
	}

	// 'type' is not a state type to switch on/off by glEnable() / glDisable()
	switch(type)
	{
	case RenderStateType::WRITING_INTO_DEPTH_BUFFER:
		return false;
//	case RenderStateType::UNKNOWN:
//		break;
	default:
		return Result::INVALID_ARGS;
	}
}


Result::Name CGLGraphicsDevice::SetRenderState( RenderStateType::Name type, bool enable )
{
	GLenum cap = 0;
	bool res = ToGLenum( type, cap );

	if( res )
	{
		if( enable )
			glEnable( cap );
		else
			glDisable( cap );

		return Result::SUCCESS;
	}

	// 'type' is not a state type to switch on/off by glEnable() / glDisable()
	switch(type)
	{
	case RenderStateType::WRITING_INTO_DEPTH_BUFFER:
		glDepthMask( enable ? GL_TRUE : GL_FALSE );
		break;
//	case RenderStateType::UNKNOWN:
//		break;
	default:
		return Result::INVALID_ARGS;
	}

	return Result::SUCCESS;
}


void CGLGraphicsDevice::SetAlphaFunction( CompareFunc::Name alpha_func )
{
	m_AlphaFunc = ToGLAlphaFunc(alpha_func);
	glAlphaFunc( m_AlphaFunc, m_fReferenceAlphaValue );
}


void CGLGraphicsDevice::SetReferenceAlphaValue( float ref_alpha )
{
	m_fReferenceAlphaValue = ref_alpha;
	glAlphaFunc( m_AlphaFunc, m_fReferenceAlphaValue );
}


static inline int ToGLFogMode( FogMode::Name fog_mode )
{
	switch( fog_mode )
	{
	case FogMode::LINEAR: return GL_LINEAR;
	case FogMode::EXP:    return GL_EXP;
	case FogMode::EXP2:   return GL_EXP2;
	default: return GL_LINEAR;
	}
}


Result::Name CGLGraphicsDevice::SetFogParams( const CFogParams& fog_params )
{
	glFogi(GL_FOG_MODE, ToGLFogMode(fog_params.Mode) );		// Fog Mode

	float rgba[] = { fog_params.Color.red, fog_params.Color.green, fog_params.Color.blue, fog_params.Color.alpha };
	glFogfv(GL_FOG_COLOR, rgba );

	glFogf( GL_FOG_START,   (float)fog_params.Start );

	glFogf( GL_FOG_END,     (float)fog_params.End );

	glFogf( GL_FOG_DENSITY, (float)fog_params.Density );

	glHint( GL_FOG_HINT, GL_DONT_CARE ); // or GL_NICEST / GL_FASTEST

	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetCullingMode( CullingMode::Name cull_mode )
{
	m_CullMode = cull_mode;

	glFrontFace( GL_CCW );

	switch( cull_mode )
	{
	case CullingMode::CLOCKWISE:        glCullFace( GL_BACK );  break;
	case CullingMode::COUNTERCLOCKWISE: glCullFace( GL_FRONT ); break;
	default:
		glCullFace( GL_BACK );
		break;
	}

	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::GetViewport( CViewport& viewport )
{
	GLint params[4];
	glGetIntegerv( GL_VIEWPORT, params );
	viewport.UpperLeftX = params[0];
	viewport.UpperLeftY = params[1];
	viewport.Width      = params[2];
	viewport.Height     = params[3];

	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetViewport( const CViewport& viewport )
{
	glViewport(
		viewport.UpperLeftX,
		viewport.UpperLeftY,
		viewport.Width,
		viewport.Height
		);

	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetClearColor( const SFloatRGBAColor& color )
{
	glClearColor( color.red, color.green, color.blue, color.alpha );
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetClearDepth( float depth )
{
	glClearDepth( depth );
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::Clear( U32 buffer_mask )
{
	GLbitfield mask = 0;
	if( buffer_mask & BufferMask::COLOR )   mask |= GL_COLOR_BUFFER_BIT;
	if( buffer_mask & BufferMask::DEPTH )   mask |= GL_DEPTH_BUFFER_BIT;
	if( buffer_mask & BufferMask::STENCIL ) mask |= GL_STENCIL_BUFFER_BIT;
	glClear( mask );
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetClipPlane( uint index, const Plane& clip_plane )
{
	double coefficients[4] =
	{
		clip_plane.normal.x,
		clip_plane.normal.y,
		clip_plane.normal.z,
		clip_plane.dist * (-1.0f)
	};

	glClipPlane( GL_CLIP_PLANE0 + index, coefficients );

	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::EnableClipPlane( uint index )
{
	glEnable( GL_CLIP_PLANE0 + index );
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::DisableClipPlane( uint index )
{
	glDisable( GL_CLIP_PLANE0 + index );
	return Result::SUCCESS;
}


Result::Name CGLGraphicsDevice::SetScissorRect( const SRect& rect )
{
	glScissor( rect.left, rect.top, rect.GetWidth(), rect.GetHeight() );
	return Result::SUCCESS;
}


} // namespace amorphous
