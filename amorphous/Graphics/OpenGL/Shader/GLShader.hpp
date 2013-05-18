#ifndef __GLShader_HPP__
#define __GLShader_HPP__


#include "amorphous/Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "amorphous/Graphics/OpenGL/fwd.hpp"
#include "amorphous/Graphics/OpenGL/GLExtensions.hpp"
#include "amorphous/Graphics/OpenGL/Shader/GLFixedFunctionPipelineManager.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/3DMath/Transform.hpp"
#include "GLSLShaderLightManager.hpp"


namespace amorphous
{


//==============================================================================
// Use glCreateShaderObjectARB, glShaderSourceARB, glCompileShaderARB, etc.
//==============================================================================

class CGLShader : public ShaderManager
{
protected:

	GLhandleARB m_Shader;

	virtual GLenum GetShaderType() const = 0;

	virtual void InitGLShader() {}

public:

	CGLShader();

	virtual ~CGLShader();

	GLhandleARB GetGLHandle() { return m_Shader; }

	bool LoadFromFile( const std::string& filepath );

	bool LoadShaderFromText( const stream_buffer& buffer );

	bool CreateShader( const char *source );

	void Release();

	void Reload();
};


class CGLVertexShader : public CGLShader
{
	GLenum GetShaderType() const { return GL_VERTEX_SHADER_ARB; }

public:
};


class CGLFragmentShader : public CGLShader
{
	enum Params
	{
		GLSL_NUM_MAX_TEXTURE_STAGES = 8,
	};

	int m_aTextureLocation[GLSL_NUM_MAX_TEXTURE_STAGES];

	void InitGLShader();

protected:

	GLenum GetShaderType() const { return GL_FRAGMENT_SHADER_ARB; }

public:

	CGLFragmentShader();
};


class CGLProgram : public ShaderManager
{
	enum PredefinedMatrixUniforms
	{
		MATRIX_WORLD = 0,
		MATRIX_VIEW,
		MATRIX_PROJ,
		MATRIX_VIEW_WORLD,
		MATRIX_PROJ_VIEW_WORLD,
		NUM_PREDEFINED_MATRIX_UNIFORMS
	};

	enum Params
	{
		NUM_TEXTURE_SAMPLER_UNIFORMS = 4,
	};

	GLhandleARB m_Program;

//	boost::shared_ptr<CGLVertexShader> m_pVertexShader;
//	boost::shared_ptr<CGLFragmentShader> m_pFragmentShader;

	// Why raw ptr?
	// - graphis resources manager currenly stores shader managers with raw pointers
	CGLVertexShader *m_pVertexShader;
	CGLFragmentShader *m_pFragmentShader;

	ShaderHandle m_VertexShader;
	ShaderHandle m_FragmentShader;

	Matrix44 m_ProjectionMatrix, m_ViewMatrix, m_WorldMatrix;

	GLint m_PredefinedMatrixUniforms[NUM_PREDEFINED_MATRIX_UNIFORMS];

	GLuint m_TextureSamplerUniforms[NUM_TEXTURE_SAMPLER_UNIFORMS];

	boost::shared_ptr<CGLSLShaderLightManager> m_pLightManager;

	void InitUniforms();

	Result::Name InitProgram();

	bool LoadMimalShaders();

public:

	CGLProgram();

	~CGLProgram();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	bool LoadShaderFromText( const char *vertex_shader, const char *fragment_shader );

	bool LoadShaderFromText( const std::string& vertex_shader, const std::string& fragment_shader ) { return LoadShaderFromText( vertex_shader.c_str(), fragment_shader.c_str() ); }

	void Release();

	void Reload();

//	void SetWorldTransform( const Matrix34& world_pose ) { GLFixedFunctionPipelineManager().SetWorldTransform(world_pose); }

	inline void SetWorldTransform( const Matrix44& world );

	inline void SetViewTransform( const Matrix44& view );

	inline void SetProjectionTransform( const Matrix44& proj );

	inline void SetWorldViewTransform( const Matrix44& world, const Matrix44& view );

	inline void SetWorldViewProjectionTransform( const Matrix44& world, const Matrix44& view, const Matrix44& proj );

	void SetBlendTransforms( const std::vector<Transform>& src_transforms );

/*
	virtual void GetWorldTransform( Matrix44& matWorld ) const {}

	virtual void GetViewTransform( Matrix44& matView ) const {}
*/
	void SetViewerPosition( const Vector3& vEyePosition );

	void SetVertexBlendMatrix( int i, const Matrix34& mat );

	void SetVertexBlendMatrix( int i, const Matrix44& mat );

	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );


	Result::Name SetTexture( const int iStage, const TextureHandle& texture );

//	Result::Name SetCubeTexture( const int index, const TextureHandle& cube_texture ) { return Result::UNKNOWN_ERROR; }

	void Begin();

	void End();
/*
	virtual Result::Name SetTechnique( const unsigned int id ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name SetTechnique( ShaderTechniqueHandle& tech_handle ) { return Result::UNKNOWN_ERROR; }

//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

//	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	virtual void SetParam( ShaderParameter< std::vector<float> >& float_param ) {}

//	void SetParam( ShaderParameter< std::vector<int> >& integer_param );
*/
	// Sets a single integer value
	void SetParam( ShaderParameter<int>& int_param );

	// Sets a single float value
	void SetParam( ShaderParameter<float>& float_param );

	// Sets a single float value
	void SetParam( ShaderParameter<Vector3>& vec3_param );

	// Sets a color value as 4 floats in RGBA order
	void SetParam( ShaderParameter<SFloatRGBAColor>& color_param );

	// Sets one or more float values
	void SetParam( ShaderParameter< std::vector<float> >& float_param );

	void SetParam( ShaderParameter<TextureParam>& tex_param );

	// Sets a column-major 4x4 matrix
	void SetParam( ShaderParameter<Matrix44>& mat44_param );

	// Sets a single float value
	void SetParam( const char *parameter_name, int int_param );

	// Sets a single float value
	void SetParam( const char *parameter_name, float float_param );

	// Sets a single float3 (Vector3) value
	void SetParam( const char *parameter_name, const Vector3& vec3_param );

	// Sets a color value as 4 floats in RGBA order
	void SetParam( const char *parameter_name, const SFloatRGBAColor& color_param );

	// Sets one or more float values
	void SetParam( const char *parameter_name, const float *float_param, uint num_float_values );

	// Sets a column-major 4x4 matrix
	void SetParam( const char *parameter_name, const Matrix44& mat44_param );

//	void SetTextureParam()

	boost::shared_ptr<ShaderLightManager> GetShaderLightManager();

	friend class ShaderManagerHub;
};


inline void CGLProgram::SetWorldTransform( const Matrix44& world )
{
	LOG_GL_ERROR( " Entered." );

	glUseProgram( m_Program );

	LOG_GL_ERROR( " glUseProgram() failed." );

	// Update cache
	m_WorldMatrix = world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_WORLD], 1, GL_FALSE, (GLfloat *)world.GetData() );

	Matrix44 view_world = m_ViewMatrix * world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)view_world.GetData() );

	// set WorldViewProj
	Matrix44 proj_view_world = m_ProjectionMatrix * view_world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)proj_view_world.GetData() );
}


inline void CGLProgram::SetViewTransform( const Matrix44& view )
{
	LOG_GL_ERROR( " Entered." );

	glUseProgram( m_Program );

	LOG_GL_ERROR( " glUseProgram() failed." );

	// Update cache
	m_ViewMatrix = view;

	if( m_pLightManager )
		m_pLightManager->SetViewTransform( view );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW], 1, GL_FALSE, (GLfloat *)view.GetData() );

	Matrix44 view_world = view * m_WorldMatrix;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)view_world.GetData() );

	// set WorldViewProj
	Matrix44 proj_view_world = m_ProjectionMatrix * view_world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)proj_view_world.GetData() );
}


inline void CGLProgram::SetProjectionTransform( const Matrix44& proj )
{
	LOG_GL_ERROR( " Entered." );

	glUseProgram( m_Program );

	LOG_GL_ERROR( " glUseProgram() failed." );

	// Update cache
	m_ProjectionMatrix = proj;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ], 1, GL_FALSE, (GLfloat *)proj.GetData() );

	Matrix44 proj_view_world = proj * m_ViewMatrix * m_WorldMatrix;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)proj_view_world.GetData() );
}


inline void CGLProgram::SetWorldViewTransform( const Matrix44& world, const Matrix44& view )
{
	LOG_GL_ERROR( " Entered." );

	glUseProgram( m_Program );

	LOG_GL_ERROR( " glUseProgram() failed." );

	// Update cache
	m_WorldMatrix = world;
	m_ViewMatrix  = view;

	if( m_pLightManager )
		m_pLightManager->SetViewTransform( view );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_WORLD], 1, GL_FALSE, (GLfloat *)world.GetData() );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW], 1, GL_FALSE, (GLfloat *)view.GetData() );

	Matrix44 view_world = view * world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)view_world.GetData() );

	Matrix44 proj_view_world = m_ProjectionMatrix * view_world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)proj_view_world.GetData() );
}


inline void CGLProgram::SetWorldViewProjectionTransform( const Matrix44& world, const Matrix44& view, const Matrix44& proj )
{
	LOG_GL_ERROR( " Entered." );

	glUseProgram( m_Program );

	LOG_GL_ERROR( " glUseProgram() failed." );

	// Update cache
	m_WorldMatrix      = world;
	m_ViewMatrix       = view;
	m_ProjectionMatrix = proj;

	if( m_pLightManager )
		m_pLightManager->SetViewTransform( view );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_WORLD], 1, GL_FALSE, (GLfloat *)world.GetData() );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW], 1, GL_FALSE, (GLfloat *)view.GetData() );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ], 1, GL_FALSE, (GLfloat *)proj.GetData() );

	Matrix44 view_world = view * world;
	Matrix44 proj_view_world = proj * view_world;

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)view_world.GetData() );

	if( 0 <= m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD] )
		glUniformMatrix4fv( m_PredefinedMatrixUniforms[MATRIX_PROJ_VIEW_WORLD], 1, GL_FALSE, (GLfloat *)proj_view_world.GetData() );
}



//==============================================================================
// Use glGenProgramsARB, glBindProgramARB, etc.
//==============================================================================
/*
class CGLProgramBase
{
	GLuint m_ProgramID;

private:

	virtual GLenum GetProgramType() const = 0;

public:

	CGLProgramBase()
		:
	m_ProgramID(0)
	{}

	virtual ~CGLProgramBase() {}

	void LoadShaderFromFile( const std::string& filepath );
};


class CGLVertexProgram : public CGLProgramBase
{
private:

	GLenum GetProgramType() const;

public:

	CGLVertexProgram() {}
};


class CGLFragmentProgram : public CGLProgramBase
{
private:

	GLenum GetProgramType() const;

public:

	CGLFragmentProgram() {}
};
*/

} // namespace amorphous



#endif /* __GLShader_HPP__ */
