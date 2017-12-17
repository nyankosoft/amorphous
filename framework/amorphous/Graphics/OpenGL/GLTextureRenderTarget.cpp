#include "GLTextureRenderTarget.hpp"
#include "GLGraphicsDevice.hpp" // LOG_GL_ERROR macro
#include "GLGraphicsResources.hpp"
#include "GLTextureUtilities.hpp"


namespace amorphous
{


bool sg_use_grm = true;


void CheckFramebufferStatus( GLenum target )
{
	GLenum ret = glCheckFramebufferStatus( target );
	switch( ret )
	{
//		case GL_FRAMEBUFFER_UNDEFINED:
//			LOG_PRINT_ERROR("FBO undefined.");
//			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			LOG_PRINT_ERROR("FBO incomplete attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			LOG_PRINT_ERROR("FBO missing attachment.");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			LOG_PRINT_ERROR("FBO incomplete draw buffer.");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			LOG_PRINT_ERROR("FBO unsupported.");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			LOG_PRINT_VERBOSE("FBO is complete.");
			break;
		default:
			printf("An unknown FBO Problem.");
	}
}


void PrintFramebufferInfo(GLenum target, GLuint fbo)
{
	glBindFramebuffer(target,fbo);

	int res;

	GLint buffer;
	int i = 0;
	do
	{
		glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

		if( buffer != GL_NONE )
		{
			LOG_PRINTF(("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0));
			
			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
			LOG_PRINTF(("  Attachment type: %s\n", res==GL_TEXTURE?"Texture":"Render Buffer"));
			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
			LOG_PRINTF(("  Attachment object name: %d\n",res));
		}
		++i;

	}
	while( buffer != GL_NONE );

	glBindFramebuffer(target,0);
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

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		LOG_GL_ERROR( "glFramebufferTexture2D() failed." );
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
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Target, texture, 0);

		LOG_GL_ERROR( "glFramebufferTexture2D() failed." );

		// Set the list of draw buffers.
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		LOG_GL_ERROR( "glDrawBuffers() failed." );

		GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if( fb_status != GL_FRAMEBUFFER_COMPLETE)
			LOG_PRINT_ERROR( "The FBO is not framebuffer complete." );

		LOG_GL_ERROR( "glCheckFramebufferStatus() failed." );

		return Result::SUCCESS;
	}
};


	
GLTextureRenderTarget::GLTextureRenderTarget()
{
	m_Framebuffer = 0;
	m_DepthRenderBuffer = 0;
	m_RenderTargetTextureID = 0;
	m_OrigFrameBuffer = 0;

//	m_RenderTargetTexture      = 0;
}


GLTextureRenderTarget::GLTextureRenderTarget( const TextureResourceDesc& texture_desc )
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


GLTextureRenderTarget::~GLTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void GLTextureRenderTarget::ReleaseTextures()
{
	// Delete resources
//	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffers(1, &m_DepthRenderBuffer);
	m_DepthRenderBuffer = 0;

	// Bind 0, which means render to back buffer.
	// As a result, m_Framebuffer is unbound
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &m_Framebuffer );
	m_Framebuffer = 0;
}


bool GLTextureRenderTarget::InitScreenSizeRenderTarget()
{
	m_bScreenSizeRenderTarget = true;

	return Init( GetScreenWidth(), GetScreenHeight(), TextureFormat::A8R8G8B8, 0 );
}


bool GLTextureRenderTarget::LoadTextures()
{
//	LOG_GL_ERROR( "GLTextureRenderTarget::LoadTextures() entered." );

	ReleaseTextures();

	if( !(m_TextureDesc.UsageFlags & UsageFlag::RENDER_TARGET) )
		LOG_PRINT_ERROR( " UsageFlag::RENDER_TARGET was not specified for the render target texture." );

//	if( !extensionSupported((char *)glGetString(GL_EXTENSIONS),"GL_EXT_framebuffer_object") )
	if( !glGenFramebuffers
	 || !glGenRenderbuffers
	 || !glBindFramebuffer )
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

	glGenFramebuffers(1, &m_Framebuffer);

	LOG_GL_ERROR( "glGenFramebuffers() failed." );

	//	glGenTextures(1, &tex);
//	GLuint tex = m_RenderTargetTexture.GetGLTextureID();

//	glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_Framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer);

	LOG_GL_ERROR( "glGenRenderbuffers() failed." );

	int tex_width  = m_TextureDesc.Width;
	int tex_height = m_TextureDesc.Height;

	LOG_GL_ERROR( "glBindFramebuffer() failed." );

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
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RenderTargetTextureID, 0);
		LOG_GL_ERROR( " glFramebufferTexture2D() failed." );
	}

	if( m_OptionFlags & OPTFLG_NO_DEPTH_BUFFER )
	{
		// Do not create the depth buffer.
	}
	else
	{
		glGenRenderbuffers(1, &m_DepthRenderBuffer);

		LOG_GL_ERROR( " glGenRenderbuffers() failed." );

//		GET_GLERROR(NULL);

		glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, filter_mode);
		glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, filter_mode);
		glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		LOG_GL_ERROR( " One or more glTexParameteri() call(s) failed." );

//		GLFramebufferTextureResourceVisitor visitor(texTarget);
//		m_RenderTargetTexture.AcceptTextureResourceVisitor( visitor );

//		GET_GLERROR(0);
		LOG_GL_ERROR( " Clearing OpenGL errors..." );

		// initialize depth renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRenderBuffer);

		LOG_GL_ERROR( " glBindRenderbuffer() failed." );

		// Before we can bind a renderbuffer to a framebuffer object,
		// we must allocate storage for the renderbuffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, tex_width, tex_height);

		LOG_GL_ERROR( " glRenderbufferStorage() failed." );

		// Attach the renderbuffer object to the framebuffer object
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRenderBuffer);

//		GET_GLERROR(0);
		LOG_GL_ERROR( " glFramebufferRenderbuffer() failed." );

		CheckFramebufferStatus( GL_DRAW_FRAMEBUFFER);
	}

//	CheckFramebufferStatus();

	// Bind 0, which means render to back buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	LOG_GL_ERROR( " glBindFramebuffer() returned error(s)" );

//	PrintFramebufferInfo( GL_DRAW_FRAMEBUFFER, m_Framebuffer );

	return true;
}


void GLTextureRenderTarget::CopyRenderTarget()
{
	//	DIRECT3D9.GetDevice()->GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface );
}


void GLTextureRenderTarget::SetBackgroundColor( const SFloatRGBAColor& bg_color )
{
	TextureRenderTarget::SetBackgroundColor( bg_color );
}


void GLTextureRenderTarget::SetRenderTarget()
{
	// TODO: merge D3D implementation
	GraphicsDevice().GetViewport( m_OriginalViewport );

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	// save the current framebuffer
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &m_OrigFrameBuffer );

	LOG_GL_ERROR( " glGetIntegerv() failed. Failed to get the original frame buffer." );

	GLenum texTarget = GL_TEXTURE_2D;
	glBindTexture(texTarget, 0);

	LOG_GL_ERROR( " glBindTexture() failed." );

//	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer );
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer );

	LOG_GL_ERROR( " glBindFramebuffer() failed." );

//	glPushAttrib( GL_VIEWPORT_BIT );

//	GLFramebufferTextureResourceVisitor visitor(texTarget);
//	m_RenderTargetTexture.AcceptTextureResourceVisitor( visitor );

	// Set Drawing buffers
	GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);

	LOG_GL_ERROR( " glDrawBuffers() failed." );

	const SFloatRGBAColor& bg_color = m_BackgroundColor;
	glClearColor(
		bg_color.red,
		bg_color.green,
		bg_color.blue,
		bg_color.alpha
		);

	LOG_GL_ERROR( " glClearColor() failed." );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	LOG_GL_ERROR( " glClear() failed." );

	glViewport( 0, 0, m_TextureDesc.Width, m_TextureDesc.Height );

	LOG_GL_ERROR( " glViewport() failed." );
}


void GLTextureRenderTarget::ResetRenderTarget()
{
	// TODO: merge D3D implementation
	GraphicsDevice().SetViewport( m_OriginalViewport );

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

//	glPopAttrib();

	// Bind the original frame buffer. 0 if it's the back buffer.
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_OrigFrameBuffer );

//	GLenum src_format = (m_TextureDesc.Format == TextureFormat::R32F) ? 

	bool save_rt_texture = false;
	if( save_rt_texture )
		SaveGL2DTextureToImageFile( m_RenderTargetTextureID, m_TextureDesc.Width, m_TextureDesc.Height, GL_RGBA, GL_UNSIGNED_BYTE, "rt_tex.png" );

	LOG_GL_ERROR( " glBindFramebuffer() failed." );
}


void GLTextureRenderTarget::OutputImageFile( const std::string& image_file_path )
{
	LOG_PRINT_ERROR( " not implemented." );
}


} // namespace amorphous
