#ifndef __GLCgEffect_HPP__
#define __GLCgEffect_HPP__


#include "../../Shader/CgEffectBase.hpp"


#pragma comment( lib, "cgGL.lib" )


class CGLCgEffect : public CCgEffectBase
{
	void SetCGTextureParameter( CGparameter& param, CTextureHandle& texture );

	void InitCgContext();

public:
	CGLCgEffect();

	~CGLCgEffect(){}

//	bool LoadShaderFromFile( const std::string& filename );
//	bool LoadShaderFromText( const stream_buffer& buffer );

//	void Release();
//	void Reload();

//	void SetHandlesToNULL();

//	inline void SetViewerPosition( const Vector3& vEyePosition );

//	inline void SetVertexBlendMatrix( int i, const Matrix34& mat );
//	inline void SetVertexBlendMatrix( int i, const Matrix44& mat );

//	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );
//	inline Result::Name SetTexture( const int iStage, const CTextureHandle& texture );
//	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

//	inline void Begin();

//	inline void End();

//	inline void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture );

//	inline Result::Name SetTechnique( const unsigned int id );
//	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );
//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );
//	GLXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

//	void SetTextureParam()

//	boost::shared_ptr<CShaderLightManager> GetShaderLightManager();

//	void InitBlendTransformVariables( const std::string& variable_name );
//	void InitBlendTransformVariables();
//	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );
};



#endif /* __GLCgEffect_HPP__ */
