#include "GLTextureRenderTarget.hpp"
#include "GLExtensions.hpp"
#include "GLGraphicsDevice.hpp" // LOG_GL_ERROR macro
#include "GLGraphicsResources.hpp"
#include "GLTextureUtilities.hpp"


namespace amorphous
{


bool sg_use_grm = true;


void CheckFramebufferStatus( GLenum target )
{
	GLenum ret = glCheckFramebufferStatusEXT( target );
	switch( ret )
	{
//		case GL_FRAMEBUFFER_UNDEFINED:
//			LOG_PRINT_ERROR("FBO undefined.");
//			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			LOG_PRINT_ERROR("FBO incomplete attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			LOG_PRINT_ERROR("FBO missing attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			LOG_PRINT_ERROR("FBO incomplete draw buffer.");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			LOG_PRINT_ERROR("FBO unsupported.");
			break;
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			LOG_PRINT_VERBOSE("FBO is complete.");
			break;
		default:
			printf("An unknown FBO Problem.");
	}
}


void PrintFramebufferInfo(GLenum target, GLuint fbo)
{
	glBindFramebufferEXT(target,fbo);

	int res;

	GLint buffer;
	int i = 0;
	do
	{
		glGetIntegerv(GL_DRAW_BUFFER0_ARB+i, &buffer);

		if( buffer != GL_NONE )
		{
			LOG_PRINTF(("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0_EXT));
			
			glGetFramebufferAttachmentParameterivEXT(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &res);
			LOG_PRINTF(("  Attachment type: %s\n", res==GL_TEXTURE?"Texture":"Render Buffer"));
			glGetFramebufferAttachmentParameterivEXT(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &res);
			LOG_PRINTF(("  Attachment object name: %d\n",res));
		}
		++i;

	}
	while( buffer != GL_NONE );

	glBindFramebufferEXT(target,0);
}


class GLFramebufferTextureAttachmentVisitor : public TextureResourceVisitor
{
public:
	Result::Name Visit( CGLTextureResource& texture_resource )
	{
		LOG_GL_ERROR( " Clearing OpenGL errors..." );

		GLuint texture = texture_resource.GetGLTextureID();
		if( texture == 0 )
		{
			LOG_PRINT_ERROR( " No texture loaded." );
			return Result::UNKNOWN_ERROR;
		}

		glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);

		LOG_GL_ERROR( "glFramebufferTexture2DEXT() failed." );
		return Result::SUCCESS;
	}
};


class GLFramebufferTextureResourceVisitor : public TextureResourceVisitor
{
	GLenum m_Target;

public:

	GLFramebufferTextureResourceVisitor( GLenum target ) : m_Target(target) {}
	~GLFramebufferTextureResourceVisitor() {}

	Result::Name Visit( CGLTextureResource& texture_resource )
	{
		LOG_GL_ERROR( " Clearing OpenGL errors..." );

		// Attach a level of a texture object as a logical buffer to the currently bound framebuffer object
		GLuint texture = texture_resource.GetGLTextureID();
//		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_Target, texture, 0);

		LOG_GL_ERROR( "glFramebufferTexture2DEXT() failed." );

		// Set the list of draw buffers.
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0_EXT};
		glDrawBuffers(1, DrawBuffers);

		LOG_GL_ERROR( "glDrawBuffers() failed." );

		GLenum fb_status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if( fb_status != GL_FRAMEBUFFER_COMPLETE_EXT )
			LOG_PRINT_ERROR( "The FBO is not framebuffer complete." );

		LOG_GL_ERROR( "glCheckFramebufferStatusEXT() failed." );

		return Result::SUCCESS;
	}
};


	
CGLTextureRenderTarget::CGLTextureRenderTarget()
{
	m_Framebuffer = 0;
	m_DepthRenderBuffer = 0;
	m_RenderTargetTextureID = 0;
	m_OrigFrameBuffer = 0;

//	m_RenderTargetTexture      = 0;
}


CGLTextureRenderTarget::CGLTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
:
TextureRenderTarget( texture_width, texture_height, texture_format )
{
	m_Framebuffer = 0;
	m_DepthRenderBuffer = 0;
	m_RenderTargetTextureID = 0;
	m_OrigFrameBuffer = 0;

//	m_RenderTargetTexture      = 0;

	LoadTextures();
}


CGLTextureRenderTarget::CGLTextureRenderTarget( const TextureResourceDesc& texture_desc )
:
TextureRenderTarget(texture_desc)
{
	m_Framebuffer = 0;
	m_DepthRenderBuffer = 0;
	m_RenderTargetTextureID = 0;
	m_OrigFrameBuffer = 0;

//	m_RenderTargetTexture      = 0;

	LoadTextures();
}


CGLTextureRenderTarget::~CGLTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void CGLTextureRenderTarget::ReleaseTextures()
{
	// Delete resources
//	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffersEXT(1, &m_DepthRenderBuffer);
	m_DepthRenderBuffer = 0;

	// Bind 0, which means render to back buffer.
	// As a result, m_Framebuffer is unbound
//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glDeleteFramebuffersEXT(1, &m_Framebuffer );
	m_Framebuffer = 0;
}


bool CGLTextureRenderTarget::Init( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
{
	m_TextureDesc.Width  = texture_width;
	m_TextureDesc.Height = texture_height;

	m_TextureDesc.Format = texture_format;

	m_BackgroundColor = SFloatRGBAColor(0,0,0,0);

	return LoadTextures();
}


bool CGLTextureRenderTarget::Init( const TextureResourceDesc& texture_desc )
{
	m_TextureDesc = texture_desc;

	m_BackgroundColor = SFloatRGBAColor(0,0,0,0);

	return LoadTextures();
}


bool CGLTextureRenderTarget::InitScreenSizeRenderTarget()
{
	m_bScreenSizeRenderTarget = true;

	return Init( GetScreenWidth(), GetScreenHeight(), TextureFormat::A8R8G8B8, 0 );
}


bool CGLTextureRenderTarget::LoadTextures()
{
	ReleaseTextures();

	if( !(m_TextureDesc.UsageFlags & UsageFlag::RENDER_TARGET) )
		LOG_PRINT_ERROR( " UsageFlag::RENDER_TARGET was not specified for the render target texture." );

//	if( !extensionSupported((char *)glGetString(GL_EXTENSIONS),"GL_EXT_framebuffer_object") )
	if( !glGenFramebuffersEXT
	 || !glGenRenderbuffersEXT
	 || !glBindFramebufferEXT )
	{
//		LOG_PRINT_ERROR( " The OpenGL extension, 'GL_EXT_framebuffer_object', is not supported." );
		LOG_PRINT_ERROR( " Seems like your video card does not support framebuffer object of OpenGL." );
		return false;
	}

//	GLuint m_Framebuffer;  // color render target
//	GLuint depth_rb = m_DepthRenderBuffer; // depth render target
//	GLuint stencil_rb; // depth render target

//	m_TextureDesc = ...;

	// Need to create render target texture with these params. See the commented out code (A) below.
	GLenum format = GL_RGBA;
	GLenum type = GL_FLOAT;
	GLenum texInternalFormat = GL_RGBA8;

	if( sg_use_grm )
	{
		bool texture_created = m_RenderTargetTexture.Load( m_TextureDesc );
		if( !texture_created )
		{
			LOG_PRINT_ERROR( " Failed to create the render target texture." );
			return false;
		}
	}
	else
	{
		LOG_GL_ERROR( " Clearing OpenGL errors (creating a texture without using GRM)..." );

		GLuint tex = 0;

		glGenTextures(1, &tex);
		LOG_GL_ERROR( "glGenTextures() failed." );

		glBindTexture(GL_TEXTURE_2D, tex);
		LOG_GL_ERROR( "glBindTexture() failed." );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		LOG_GL_ERROR( "One of glTexParameteri() calls failed." );

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
							m_TextureDesc.Width,
							m_TextureDesc.Height, 
							0, GL_RGBA, GL_UNSIGNED_BYTE,
							NULL); 
		LOG_GL_ERROR( "glTexImage2D() failed." );

		m_RenderTargetTextureID = tex;

		glBindTexture(GL_TEXTURE_2D, 0);
		LOG_GL_ERROR( "glBindTexture() failed." );
	}

	GLenum texTarget = GL_TEXTURE_2D;
	GLenum filter_mode = (texTarget == GL_TEXTURE_RECTANGLE) ? GL_NEAREST : GL_LINEAR;

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	glEnable(GL_DEPTH_TEST);

	LOG_GL_ERROR( "glEnable(GL_DEPTH_TEST) failed." );

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	LOG_GL_ERROR( "glClearColor() failed." );

	glGenFramebuffersEXT(1, &m_Framebuffer);

	LOG_GL_ERROR( "glGenFramebuffersEXT() failed." );

	//	glGenTextures(1, &tex);
//	GLuint tex = m_RenderTargetTexture.GetGLTextureID();

//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Framebuffer);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_Framebuffer);

	LOG_GL_ERROR( "glGenRenderbuffersEXT() failed." );

	int tex_width  = m_TextureDesc.Width;
	int tex_height = m_TextureDesc.Height;

	LOG_GL_ERROR( "glBindFramebufferEXT() failed." );

	// (A) init texture
	//	glBindTexture(texTarget, tex);
	//	glTexImage2D(texTarget, 0, texInternalFormat, tex_width, tex_height, 0, 
	//				 GL_RGBA, GL_FLOAT, NULL);

	if( sg_use_grm )
	{
		GLFramebufferTextureAttachmentVisitor fbo_visitor;
		m_RenderTargetTexture.AcceptTextureResourceVisitor( fbo_visitor );
	}
	else
	{
		glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_RenderTargetTextureID, 0);
		LOG_GL_ERROR( " glFramebufferTexture2DEXT() failed." );
	}

	glGenRenderbuffersEXT(1, &m_DepthRenderBuffer);

	LOG_GL_ERROR( " glGenRenderbuffersEXT() failed." );

//	GET_GLERROR(NULL);

	glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, filter_mode);
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, filter_mode);
	glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	LOG_GL_ERROR( " One or more glTexParameteri() call(s) failed." );

//	GLFramebufferTextureResourceVisitor visitor(texTarget);
//	m_RenderTargetTexture.AcceptTextureResourceVisitor( visitor );

//	GET_GLERROR(0);
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	// initialize depth renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_DepthRenderBuffer);

	LOG_GL_ERROR( " glBindRenderbufferEXT() failed." );

	// Before we can bind a renderbuffer to a framebuffer object,
	// we must allocate storage for the renderbuffer.
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, tex_width, tex_height);

	LOG_GL_ERROR( " glRenderbufferStorageEXT() failed." );

	// Attach the renderbuffer object to the framebuffer object
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
		GL_RENDERBUFFER_EXT, m_DepthRenderBuffer);

//	GET_GLERROR(0);
	LOG_GL_ERROR( " glFramebufferRenderbufferEXT() failed." );

	CheckFramebufferStatus( GL_DRAW_FRAMEBUFFER_EXT );

//	CheckFramebufferStatus();

	// Bind 0, which means render to back buffer
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

	LOG_GL_ERROR( " glBindFramebufferEXT() returned error(s)" );

//	PrintFramebufferInfo( GL_DRAW_FRAMEBUFFER_EXT, m_Framebuffer );

	return true;
}


void CGLTextureRenderTarget::CopyRenderTarget()
{
	//	DIRECT3D9.GetDevice()->GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface );
}


void CGLTextureRenderTarget::SetBackgroundColor( const SFloatRGBAColor& bg_color )
{
	TextureRenderTarget::SetBackgroundColor( bg_color );

	glClearColor(
		bg_color.red,
		bg_color.green,
		bg_color.blue,
		bg_color.alpha
		);
}


void CGLTextureRenderTarget::SetRenderTarget()
{
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	// save the current framebuffer
	glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_OrigFrameBuffer );

	LOG_GL_ERROR( " glGetIntegerv() failed. Failed to get the original frame buffer." );

	GLenum texTarget = GL_TEXTURE_2D;
	glBindTexture(texTarget, 0);

	LOG_GL_ERROR( " glBindTexture() failed." );

//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Framebuffer );
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_Framebuffer );

	LOG_GL_ERROR( " glBindFramebufferEXT() failed." );

//	glPushAttrib( GL_VIEWPORT_BIT );

//	GLFramebufferTextureResourceVisitor visitor(texTarget);
//	m_RenderTargetTexture.AcceptTextureResourceVisitor( visitor );

	// Set Drawing buffers
	GLuint attachments[1] = {GL_COLOR_ATTACHMENT0_EXT};
	glDrawBuffers(1, attachments);

	LOG_GL_ERROR( " glDrawBuffers() failed." );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	LOG_GL_ERROR( " glClear() failed." );

	glViewport( 0, 0, m_TextureDesc.Width, m_TextureDesc.Height );

	LOG_GL_ERROR( " glViewport() failed." );
}


void CGLTextureRenderTarget::ResetRenderTarget()
{
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

//	glPopAttrib();

	// Bind the original frame buffer. 0 if it's the back buffer.
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, m_OrigFrameBuffer );

	bool save_rt_texture = false;
	if( save_rt_texture )
		SaveGL2DTextureToImageFile( m_RenderTargetTextureID, m_TextureDesc.Width, m_TextureDesc.Height, GL_RGBA, GL_UNSIGNED_BYTE, "rt_tex.png" );

	LOG_GL_ERROR( " glBindFramebufferEXT() failed." );
}


void CGLTextureRenderTarget::OutputImageFile( const std::string& image_file_path )
{
	LOG_PRINT_ERROR( " not implemented." );
}


} // namespace amorphous
