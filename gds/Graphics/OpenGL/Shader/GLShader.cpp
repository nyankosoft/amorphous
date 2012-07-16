#include "GLShader.hpp"
#include <gl/gl.h>
#include "gds/Graphics/OpenGL/Shader/GLSLShaderLightManager.hpp"
#include "gds/Support/lfs.hpp"
#include "gds/Support/SafeDelete.hpp"

using namespace std;
using namespace boost;

/*

{
	CShaderHandle m_Shader;
	CMeshHandle m_Mesh;
}
{
	bool res = m_Shader.Load( "shaer.vert|shaer.frag" );

	m_Mesh.Load( "mesh.msh" );

	// ...

	m_Mesh.Render( m_Shader );
}

*/

int read_text_file( const std::string& filepath, std::vector<char>& buffer )
{
    if (filepath.length() == 0)
		return -1;

	if( !lfs::path_exists(filepath) )
	{
	//	fprintf(stderr,"Cannot open \"%s\" for stat read!\n", filename);
		return -1;
	}

	long size = lfs::get_filesize(filepath);

//	char * buf = new char[size+1];
	buffer.resize( size + 1, 0 );

	FILE *fp = fopen( filepath.c_str(), "rb" ); // Check if the file should be opened with "rb" or "r" option
	if ( !fp )
	{
//		fprintf(stderr,"Cannot open \"%s\" for read!\n", filepath.c_str());
		return -1;
	}

	size_t bytes = fread( &(buffer[0]), 1, size, fp );

	buffer[bytes] = 0;

	fclose(fp);
	return 0;
}

/*
// OpenGL 2.0 syntax:

void setShaders()
{

	char *vs,*fs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);	

	vs = textFileRead("toon.vert");
	fs = textFileRead("toon.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();
	
	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	glUseProgram(p);
}
*/

// ARB extension syntax:
#if 0
void setShaders()
{

	char *vs,*fs;

	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	

	vs = textFileRead("toon.vert");
	fs = textFileRead("toon.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSourceARB(v, 1, &vv,NULL);
	glShaderSourceARB(f, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShaderARB(v);
	glCompileShaderARB(f);

	p = glCreateProgramObjectARB();
	
	glAttachObjectARB(p,v);
	glAttachObjectARB(p,f);

	glLinkProgramARB(p);
	glUseProgramObjectARB(p);
}
#endif /* 0 */


CGLShader::CGLShader()
:
m_Shader(0)
{
}


bool CGLShader::LoadFromFile( const std::string& filepath )
{
	if( glCreateShader )
		m_Shader = glCreateShader( GetShaderType() );
	else if( glCreateShaderObjectARB )
		m_Shader = glCreateShaderObjectARB( GetShaderType() );
	else
		return false;

	vector<char> buffer;
	int ret = read_text_file( filepath, buffer ); // load .vert or .frag file here
	if( ret != 0 || buffer.empty() )
	{
		return false;
	}

//	const char * vv = vs;

	const char *pBuffer = &(buffer[0]);
	glShaderSourceARB( m_Shader, 1, &pBuffer, NULL );

	LOG_GL_ERROR( "glShaderSourceARB() failed." );

	glCompileShaderARB( m_Shader );

	LOG_GL_ERROR( "glCompileShaderARB() failed. Failed to compile a shader" );

	const int max_log_length = 2048;
	char log_buffer[max_log_length];
	memset( log_buffer, 0, sizeof(log_buffer) );
	int log_length = 0;
	glGetInfoLogARB( m_Shader, max_log_length - 1, &log_length, log_buffer );

	if( 0 < log_length )
		LOG_PRINT_ERROR( log_buffer );

	return true;
/*
	p = glCreateProgramObjectARB();
	
	glAttachObjectARB(p,m_Shader);
//	glAttachObjectARB(p,f);

	glLinkProgramARB(p);
	glUseProgramObjectARB(p);
*/
}


bool CGLShader::LoadShaderFromText( const stream_buffer& buffer )
{
	return false;
}


void CGLShader::Release()
{
	// release m_Shader
}


void CGLShader::Reload()
{
}


CGLFragmentShader::CGLFragmentShader()
{
	for( int i=0; i<GLSL_NUM_MAX_TEXTURE_STAGES; i++ )
		m_aTextureLocation[i] = 0;
}


void CGLFragmentShader::InitGLShader()
{
	if( !glGetUniformLocation ) glGetUniformLocation = glGetUniformLocationARB;
	if( !glUniform1i )          glUniform1i          = glUniform1iARB;

	char sampler_name[16];
	memset( sampler_name, 0, sizeof(sampler_name) );
	for( int i=0; i<GLSL_NUM_MAX_TEXTURE_STAGES; i++ )
	{
		sprintf( sampler_name, "tex%d", i );
		m_aTextureLocation[i] = glGetUniformLocation( m_Shader, sampler_name );

		glUniform1i( m_aTextureLocation[i], i );
	}
}


//=============================================================================
// CGLProgram
//=============================================================================

bool sg_UseGRM = false;

CGLProgram::CGLProgram()
:
m_Program(0),
m_pVertexShader(NULL),
m_pFragmentShader(NULL)
{
	m_pLightManager.reset( new CGLSLShaderLightManager );
}


CGLProgram::~CGLProgram()
{
	Release();
}


bool CGLProgram::LoadShaderFromFile( const std::string& filename )
{
	size_t separator_pos = filename.find( "|" );
	if( separator_pos == string::npos
	 || filename.length() - 1 <= separator_pos )
	{
		return false;
	}

	const string vs_path = filename.substr( 0, separator_pos );
	const string fs_path = filename.substr( separator_pos + 1 ); // range is checked by the code above

	bool vs_loaded = false, fs_loaded = false;
	if( sg_UseGRM )
	{
		// use handles

//		CShaderHandle m_VertexShader;
//		CShaderHandle m_FragmentShader;

		CShaderResourceDesc vs_desc, fs_desc;

		// vertex program
		vs_desc.ResourcePath = vs_path;
		vs_desc.ShaderType = CShaderType::VERTEX_SHADER;
		vs_loaded = m_VertexShader.Load( vs_desc );

		// fragment program
		fs_desc.ResourcePath = fs_path;
		vs_desc.ShaderType = CShaderType::PIXEL_SHADER;
		fs_loaded = m_FragmentShader.Load( fs_desc );

//		m_pVertexShader   = dynamic_pointer_cast<CGLVertexShader,CShaderManager>( m_VertexShader.GetShaderManager() );
//		m_pFragmentShader = dynamic_pointer_cast<CGLFragmentShader,CShaderManager>( m_FragmentShader.GetShaderManager() );
		m_pVertexShader   = dynamic_cast<CGLVertexShader *>( m_VertexShader.GetShaderManager() );
		m_pFragmentShader = dynamic_cast<CGLFragmentShader *>( m_FragmentShader.GetShaderManager() );

	}
	else
	{
		// These vertex and fragment shaders are managed by this shader manager
		// and not registered to graphics resource manager (GRM)
		// rationale: shader manager classes are modules that belongs to layer lower than that of GRM
		// but SetTexture function of shader manager takes texture handle as its argument...
//		m_pVertexShader   = shared_ptr<CGLVertexShader>( new CGLVertexShader );
//		m_pFragmentShader = shared_ptr<CGLFragmentShader>( new CGLFragmentShader );
		m_pVertexShader   = new CGLVertexShader;
		m_pFragmentShader = new CGLFragmentShader;

		// how to share vertex & fragment shaders between different shader managers

		vs_loaded = m_pVertexShader->LoadFromFile( vs_path );
		fs_loaded = m_pFragmentShader->LoadFromFile( fs_path );
	}

	if( !vs_loaded )
		LOG_PRINT_ERROR( "Failed to load a vertex shader: " + vs_path );

	if( !fs_loaded )
		LOG_PRINT_ERROR( "Failed to load a fragment shader: " + fs_path );

	if( !m_pVertexShader || !m_pFragmentShader )
		return false;

	m_Program = glCreateProgramObjectARB();

	glAttachObjectARB( m_Program, m_pVertexShader->GetGLHandle() );
	glAttachObjectARB( m_Program, m_pFragmentShader->GetGLHandle() );

//	glLinkProgramARB( m_Program );
//	glUseProgramObjectARB( m_Program );

	LOG_GL_ERROR( "Detected a GL error at the end of the function." );

	return true;
}


bool CGLProgram::LoadShaderFromText( const stream_buffer& buffer )
{
	return false;
}


void CGLProgram::Release()
{
	// release m_Program

	if( sg_UseGRM )
	{
		m_pVertexShader = NULL;
		m_pFragmentShader = NULL;
		m_VertexShader.Release();
		m_FragmentShader.Release();
	}
	else
	{
		SafeDelete( m_pVertexShader );
		SafeDelete( m_pFragmentShader );
	}
}


void CGLProgram::Reload()
{
}


void CGLProgram::SetViewerPosition( const Vector3& vEyePosition )
{
}


void CGLProgram::SetVertexBlendMatrix( int i, const Matrix34& mat )
{
//	if( m_pVertexShader )
//		m_pVertexShader
}


void CGLProgram::SetVertexBlendMatrix( int i, const Matrix44& mat )
{
//	if( m_pVertexShader )
//		m_pVertexShader
}


void CGLProgram::SetVertexBlendTransforms( const std::vector<Transform>& src_transforms )
{
//	if( m_pVertexShader )
//		m_pVertexShader
}



Result::Name CGLProgram::SetTexture( const int iStage, const CTextureHandle& texture )
{
	if( glActiveTexture )
		glActiveTexture( GL_TEXTURE0 + iStage );
	else if( glActiveTextureARB )
		glActiveTextureARB( GL_TEXTURE0_ARB + iStage );

	glEnable( GL_TEXTURE_2D );

	// glMultiTexCoord2fARB

	glBindTexture( GL_TEXTURE_2D, texture.GetGLTextureID() );

	return Result::UNKNOWN_ERROR;
}


void CGLProgram::Begin()
{
	if( m_Program == 0 )
		return;

	glLinkProgramARB( m_Program );
	glUseProgramObjectARB( m_Program );
}


void CGLProgram::End()
{
}


boost::shared_ptr<CShaderLightManager> CGLProgram::GetShaderLightManager()
{
	return m_pLightManager;
}


/*
//==================================================================================
// 
//==================================================================================
void CGLProgramBase::LoadShaderFromFile( const std::string& filepath )
{
	vector<char> buffer;
	int ret = read_text_file( filepath, buffer );
	if( ret != 0 )
		return;

	GLenum program_type = GetProgramType();
	glGenProgramsARB(1, &m_ProgramID);
	glBindProgramARB(program_type, m_ProgramID);
//	glProgramStringARB(program_type, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) strlen(code), (GLubyte *) code);
	glProgramStringARB(program_type, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) buffer.size(), (GLubyte *) &(buffer[0]));

	GLint error_pos;
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_pos);
	if (error_pos != -1)
	{
		const GLubyte *error_string;
		error_string = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		string error_msg = fmt_string( "%s program error at position: %d\n%s\n",
			(program_type == GL_VERTEX_PROGRAM_ARB) ? "vertex" : "fragment",
			error_pos, error_string);

		LOG_PRINT_ERROR( error_msg );
	}

//	return program_id;
}


GLenum CGLVertexProgram::GetProgramType() const { return GL_VERTEX_PROGRAM_ARB; }


GLenum CGLFragmentProgram::GetProgramType() const { return GL_FRAGMENT_PROGRAM_ARB; }
*/

/*
void InitVertexBlendTransforms(  )
{
}


void SetVertexBlendMatrices(  )
{
	char name[128];
	memset( name, 0, sizeof(name) );
	sprintf( name, "g_BlendTransforms" );

	GLint m_Locaiton = glGetUniformLocation( program, name );
}


void SetVertexBlendTransforms(  )
{
	glUniform4f( m_Locaiton, f0, f1, f2, f3 );

	float values[] = {...};
	int count = 1;
	glUniform4fv( m_Locaiton, count, values );
}
*/
