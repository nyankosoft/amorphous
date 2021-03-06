#ifndef __D3DCgEffect_HPP__
#define __D3DCgEffect_HPP__


#include "../../Shader/CgEffectBase.hpp"


namespace amorphous
{


#pragma comment( lib, "cgD3D9.lib" )


class CD3DCgManager;


class CD3DCgEffect : public CCgEffectBase
{
	void SetCGTextureParameter( CGparameter& param, TextureHandle& texture );

	static uint ms_RefCount;

	static std::shared_ptr<CD3DCgManager> ms_pD3DCgManager;

	void InitCgContext();

public:

	CD3DCgEffect();

	~CD3DCgEffect();

//	bool LoadShaderFromFile( const std::string& filename );

//	bool LoadShaderFromText( const stream_buffer& buffer );

//	void Release();

//	void Reload();

//	void SetHandlesToNULL();

//	inline void SetVertexBlendMatrix( int i, const Matrix34& mat );

//	inline void SetVertexBlendMatrix( int i, const Matrix44& mat );


//	inline Result::Name SetTexture( const int iStage, const TextureHandle& texture );

//	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

//	inline void Begin();

//	inline void End();

//	inline Result::Name SetTechnique( const unsigned int id );

//	inline Result::Name SetTechnique( ShaderTechniqueHandle& tech_handle );

//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

//	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

//	void SetTextureParam()

//	std::shared_ptr<ShaderLightManager> GetShaderLightManager();

//	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );
};

} // namespace amorphous



#endif /* __D3DCgEffect_HPP__ */
