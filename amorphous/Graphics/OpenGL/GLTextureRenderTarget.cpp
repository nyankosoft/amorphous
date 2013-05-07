#include "GLTextureRenderTarget.hpp"
#include "GLExtensions.hpp"
#include "GLGraphicsDevice.hpp" // LOG_GL_ERROR macro
#include "GLGraphicsResources.hpp"


namespace amorphous
{


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
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_Target, texture, 0);

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


//	m_RenderTargetTexture      = 0;
}


CGLTextureRenderTarget::CGLTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
:
TextureRenderTarget( texture_width, texture_height, texture_format )
{
	m_Framebuffer = 0;
	m_DepthRenderBuffer = 0;
	m_RenderTargetTextureID = 0;

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

//	m_RenderTargetTexture      = 0;

	LoadTextures();
}


CGLTextureRenderTarget::~CGLTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


void CGLTextureRenderTarget::Release()
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


void CGLTextureRenderTarget::ReleaseTextures()
{
//	SAFE_RELEASE( m_RenderTargetTexture );
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

	bool texture_created = m_RenderTargetTexture.Load( m_TextureDesc );
	if( !texture_created )
	{
		LOG_PRINT_ERROR( " Failed to create the render target texture." );
		return false;
	}

	GLenum texTarget = GL_TEXTURE_2D;
	GLenum filter_mode = (texTarget == GL_TEXTURE_RECTANGLE) ? GL_NEAREST : GL_LINEAR;
/*
	if (!glh_init_extensions("GL_ARB_fragment_program "
		"GL_ARB_vertex_program "
		"GL_NV_float_buffer "
		"GL_EXT_framebuffer_object "))
	{
		printf("Unable to load the following extension(s): %s\n\nExiting...\n", 
			glh_get_unsupported_extensions());
		return false;
//		quitapp(-1);
	}
*/
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	glGenFramebuffersEXT(1, &m_Framebuffer);

	LOG_GL_ERROR( "glGenFramebuffersEXT() failed." );

	//	glGenTextures(1, &tex);
//	GLuint tex = m_RenderTargetTexture.GetGLTextureID();

	glGenRenderbuffersEXT(1, &m_DepthRenderBuffer);

	LOG_GL_ERROR( "glGenRenderbuffersEXT() failed." );

	int tex_width  = m_TextureDesc.Width;
	int tex_height = m_TextureDesc.Height;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Framebuffer);

	LOG_GL_ERROR( "glBindFramebufferEXT() failed." );

	// (A) init texture
	//	glBindTexture(texTarget, tex);
	//	glTexImage2D(texTarget, 0, texInternalFormat, tex_width, tex_height, 0, 
	//				 GL_RGBA, GL_FLOAT, NULL);

//	GET_GLERROR(NULL);

	glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, filter_mode);
	glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, filter_mode);
	glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
//		texTarget, tex, 0);
	GLFramebufferTextureResourceVisitor visitor(texTarget);
	m_RenderTargetTexture.AcceptTextureResourceVisitor( visitor );

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

//	CheckFramebufferStatus();

	// Bind 0, which means render to back buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	LOG_GL_ERROR( " glBindFramebufferEXT() returned error(s)" );

	/*
	// load fragment programs
	const char* strTextureProgram2D = 
	"!!ARBfp1.0\n"
	"TEX result.color, fragment.texcoord[0], texture[0], 2D;\n"
	"END\n";

	const char* strTextureProgramRECT = 
	"!!ARBfp1.0\n"
	"TEX result.color, fragment.texcoord[0], texture[0], RECT;\n"
	"END\n";

	glGenProgramsARB(1, &textureProgram);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, textureProgram);
	// load correct program based on texture target
	if (texTarget == GL_TEXTURE_RECTANGLE_NV) {
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
	(GLsizei)strlen(strTextureProgramRECT), strTextureProgramRECT);
	} else {
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
	(GLsizei)strlen(strTextureProgram2D), strTextureProgram2D);
	}

	GET_GLERROR(0);

	const char* strRenderProgram = 
	"!!ARBfp1.0\n"
	"MOV result.color, fragment.color;\n"
	"END\n";

	glGenProgramsARB(1, &renderProgram);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, renderProgram);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, 
	GL_PROGRAM_FORMAT_ASCII_ARB,
	(GLsizei)strlen(strRenderProgram), strRenderProgram);
	*/
//	GET_GLERROR(0);

	return true;
}


void CGLTextureRenderTarget::CopyRenderTarget()
{
	//	DIRECT3D9.GetDevice()->GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface );
}


void CGLTextureRenderTarget::SetRenderTarget()
{
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

	// save the current framebuffer

	GLenum texTarget = GL_TEXTURE_2D;
	glBindTexture(texTarget, 0);

	LOG_GL_ERROR( " glBindTexture() failed." );

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Framebuffer );

	LOG_GL_ERROR( " glBindFramebufferEXT() failed." );

//	glPushAttrib( GL_VIEWPORT_BIT );

	glViewport( 0, 0, m_TextureDesc.Width, m_TextureDesc.Height );

	LOG_GL_ERROR( " glViewport() failed." );
}


void CGLTextureRenderTarget::ResetRenderTarget()
{
	LOG_GL_ERROR( " Clearing OpenGL errors..." );

//	glPopAttrib();

	// Bind 0, which means render to back buffer
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	LOG_GL_ERROR( " glBindFramebufferEXT() failed." );
}


void CGLTextureRenderTarget::OutputImageFile( const std::string& image_file_path )
{
	LOG_PRINT_ERROR( " not implemented." );
}


} // namespace amorphous
