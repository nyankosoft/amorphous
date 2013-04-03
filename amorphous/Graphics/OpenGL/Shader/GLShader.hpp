#ifndef __GLShader_HPP__
#define __GLShader_HPP__


#include "gds/Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "gds/Graphics/OpenGL/fwd.hpp"
#include "gds/Graphics/OpenGL/GLExtensions.hpp"
#include "gds/Graphics/OpenGL/Shader/GLFixedFunctionPipelineManager.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/3DMath/Transform.hpp"


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

	GLhandleARB GetGLHandle() { return m_Shader; }

	virtual ~CGLShader() {}

	bool LoadFromFile( const std::string& filepath );

	bool LoadShaderFromText( const stream_buffer& buffer );

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
	GLhandleARB m_Program;

//	boost::shared_ptr<CGLVertexShader> m_pVertexShader;
//	boost::shared_ptr<CGLFragmentShader> m_pFragmentShader;

	// Why raw ptr?
	// - graphis resources manager currenly stores shader managers with raw pointers
	CGLVertexShader *m_pVertexShader;
	CGLFragmentShader *m_pFragmentShader;

	ShaderHandle m_VertexShader;
	ShaderHandle m_FragmentShader;

	boost::shared_ptr<CGLSLShaderLightManager> m_pLightManager;

public:

	CGLProgram();

	~CGLProgram();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	void Release();

	void Reload();

//	void SetWorldTransform( const Matrix34& world_pose ) { GLFixedFunctionPipelineManager().SetWorldTransform(world_pose); }

	void SetWorldTransform( const Matrix44& matWorld ) { GLFixedFunctionPipelineManager().SetWorldTransform(matWorld); }

	void SetViewTransform( const Matrix44& matView ) { GLFixedFunctionPipelineManager().SetViewTransform(matView); }

	void SetProjectionTransform( const Matrix44& matProj ) { GLFixedFunctionPipelineManager().SetProjectionTransform(matProj); }

	void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView ) { GLFixedFunctionPipelineManager().SetWorldViewTransform(matWorld,matView); }

	void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj ) { GLFixedFunctionPipelineManager().SetWorldViewProjectionTransform(matWorld,matView,matProj); }

	void SetBlendTransforms( const std::vector<Transform>& src_transforms );

/*
	virtual void SetWorldTransform( const Matrix34& world_pose ) {}

	virtual void SetWorldTransform( const Matrix44& matWorld ) {}

	virtual void SetViewTransform( const Matrix44& matView ) {}

	virtual void SetProjectionTransform( const Matrix44& matProj ) {}

	virtual void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  ) {}

	virtual void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj ) {}


	virtual void GetWorldTransform( Matrix44& matWorld ) const {}

	virtual void GetViewTransform( Matrix44& matView ) const {}
*/
	void SetViewerPosition( const Vector3& vEyePosition );

	void SetVertexBlendMatrix( int i, const Matrix34& mat );

	void SetVertexBlendMatrix( int i, const Matrix44& mat );

	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );

/*
	virtual HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture ) { return E_FAIL; }
*/
	Result::Name SetTexture( const int iStage, const TextureHandle& texture );

//	HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture ) { return E_FAIL; }

	void Begin();

	void End();
/*
//	virtual void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture ) {}

	virtual Result::Name SetTechnique( const unsigned int id ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name SetTechnique( ShaderTechniqueHandle& tech_handle ) { return Result::UNKNOWN_ERROR; }

//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

//	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	virtual void SetParam( ShaderParameter< std::vector<float> >& float_param ) {}

//	void SetParam( ShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()
*/
	boost::shared_ptr<ShaderLightManager> GetShaderLightManager();

	friend class ShaderManagerHub;
};



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
