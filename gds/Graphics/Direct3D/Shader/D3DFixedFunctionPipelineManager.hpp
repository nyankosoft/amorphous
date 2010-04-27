#ifndef __D3DFixedFunctionPipelineManager_HPP__
#define __D3DFixedFunctionPipelineManager_HPP__


#include <d3dx9.h>
#include <string>
#include "D3DShaderManager.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"

class CD3DFixedFunctionPipelineLightManager;


/**
 - When created, automatically registers itself to ShaderManagerHub

 - Problems:
   - How to change some render states that are specified in the shader when the probrammable shader is used.


*/
class CD3DFixedFunctionPipelineManager : public CD3DShaderManager
{
/*	enum eShaderConstParam
	{
		NUM_MAX_TECHNIQUES = 64,
		NUM_TEXTURE_STAGES = 8,
		NUM_MAX_CUBETEXTURES = 4
	};

	enum eHandleID
	{
		HANDLE_VIEWER_POS = 0,
		HANDLE_AMBIENT_COLOR,
		NUM_HANDLES,
	};
*/

//	std::string m_strFilename;
/*
	D3DXHANDLE m_aHandle[NUM_HANDLES];
	D3DXHANDLE m_aMatrixHandle[NUM_MATRIX_HANDLES];
	D3DXHANDLE m_aTextureHandle[NUM_TEXTURE_STAGES];
	D3DXHANDLE m_aCubeTextureHandle[NUM_MAX_CUBETEXTURES];
	D3DXHANDLE m_aTechniqueHandle[NUM_MAX_TECHNIQUES];
	std::string m_astrTechniqueName[NUM_MAX_TECHNIQUES];
	std::vector<CD3DShaderParameterHandle> m_vecParamHandle;

	/// the first vacant table entry for registering a technique
	/// If m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES,
	/// it indicates no vacancy is left
	int m_VacantTechniqueEntryIndex;
*/
	boost::shared_ptr<CD3DFixedFunctionPipelineLightManager> m_pFFPLightManager;

private:

	bool Init();

public:

	CD3DFixedFunctionPipelineManager();

	~CD3DFixedFunctionPipelineManager();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	void Release();

	void Reload();

	inline void SetViewerPosition( const D3DXVECTOR3& vEyePosition );


	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );

	inline Result::Name SetTexture( const int iStage, const CTextureHandle& texture );

	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

	inline void Begin();

	inline void End();

//	inline void SetMaterial();

//	inline void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture );

//	inline Result::Name SetTechnique( const unsigned int id );
//	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );

	void SetParam( CShaderParameter< std::vector<float> >& float_param );

//	void SetParam( CShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()

	boost::shared_ptr<CShaderLightManager> GetShaderLightManager();

	inline void SetWorldTransform( const D3DXMATRIX& matWorld );

	inline void SetViewTransform( const D3DXMATRIX& matView );

	inline void SetProjectionTransform( const D3DXMATRIX& matProj );

	inline void SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  );

	inline void SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj );

	inline void GetWorldTransform( D3DXMATRIX& matWorld ) const;

	inline void GetViewTransform( D3DXMATRIX& matView ) const;
};


inline CD3DFixedFunctionPipelineManager& D3DFixedFunctionPipelineManager()
{
	static CD3DFixedFunctionPipelineManager s_obj;
	return s_obj;
}


//================================== inline implementations ==================================


inline void CD3DFixedFunctionPipelineManager::SetWorldTransform( const D3DXMATRIX& matWorld )
{
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );
}


inline void CD3DFixedFunctionPipelineManager::SetViewTransform( const D3DXMATRIX& matView )
{
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW, &matView );
}


inline void CD3DFixedFunctionPipelineManager::SetProjectionTransform( const D3DXMATRIX& matProj )
{
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
}


inline void CD3DFixedFunctionPipelineManager::SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  )
{
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW, &matView );
}


inline void CD3DFixedFunctionPipelineManager::SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld,
															 const D3DXMATRIX& matView,
															 const D3DXMATRIX& matProj )
{
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD,      &matWorld );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW,       &matView );
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
}


inline void CD3DFixedFunctionPipelineManager::GetWorldTransform( D3DXMATRIX& matWorld ) const
{
	DIRECT3D9.GetDevice()->GetTransform( D3DTS_WORLD,      &matWorld );
}


inline void CD3DFixedFunctionPipelineManager::GetViewTransform( D3DXMATRIX& matView ) const
{
	DIRECT3D9.GetDevice()->GetTransform( D3DTS_VIEW, &matView );
}


inline void CD3DFixedFunctionPipelineManager::SetViewerPosition( const D3DXVECTOR3& vEyePosition )
{
//	m_pEffect->SetValue( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(D3DXVECTOR3) );
}


inline HRESULT CD3DFixedFunctionPipelineManager::SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture )
{
	return DIRECT3D9.GetDevice()->SetTexture( iStage, pTexture );
}


inline Result::Name CD3DFixedFunctionPipelineManager::SetTexture( const int iStage, const CTextureHandle& texture )
{
	HRESULT hr = DIRECT3D9.GetDevice()->SetTexture( iStage, texture.GetTexture() );
	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline HRESULT CD3DFixedFunctionPipelineManager::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
//	if( m_aCubeTextureHandle[index] )
        return DIRECT3D9.GetDevice()->SetTexture( index, pCubeTexture );
//	else
//		return E_FAIL;
}


inline void CD3DFixedFunctionPipelineManager::Begin()
{
	HRESULT hr;
	hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
	hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );
}


inline void CD3DFixedFunctionPipelineManager::End()
{
}


/*
inline Result::Name CD3DFixedFunctionPipelineManager::SetTechnique( const unsigned int id )
{
//	HRESULT hr = m_pEffect->SetTechnique( m_aTechniqueHandle[id] );

//	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );

	return Result::UNKNOWN_ERROR;
}


inline Result::Name CD3DFixedFunctionPipelineManager::SetTechnique( CShaderTechniqueHandle& tech_handle )
{
	return Result::UNKNOWN_ERROR;
}
*/


#endif  /*  __D3DFixedFunctionPipelineManager_HPP__  */
