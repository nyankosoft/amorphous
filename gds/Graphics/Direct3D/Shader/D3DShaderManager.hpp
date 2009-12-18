#ifndef __D3DShaderManager_HPP__
#define __D3DShaderManager_HPP__


#include "Graphics/Shader/ShaderManager.hpp"


class CD3DShaderManager : public CShaderManager
{
public:

	virtual ~CD3DShaderManager() {}

	inline void SetWorldTransform( const Matrix34& world_pose );

	inline void SetWorldTransform( const Matrix44& matWorld ) { D3DXMATRIX m; matWorld.GetRowMajorMatrix44((Scalar *)&m); SetWorldTransform((Scalar *)m); }

	inline void SetViewTransform( const Matrix44& matView ) { D3DXMATRIX m; matView.GetRowMajorMatrix44((Scalar *)&m); SetViewTransform((Scalar *)m); }

	inline void SetProjectionTransform( const Matrix44& matProj ) { D3DXMATRIX m; matProj.GetRowMajorMatrix44((Scalar *)&m); SetProjectionTransform((Scalar *)m); }

	inline void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView );

	inline void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj );


	inline void GetWorldTransform( Matrix44& matWorld ) const;

	inline void GetViewTransform( Matrix44& matView ) const;


	virtual void SetWorldTransform( const D3DXMATRIX& matWorld ) = 0;

	virtual void SetViewTransform( const D3DXMATRIX& matView ) = 0;

	virtual void SetProjectionTransform( const D3DXMATRIX& matProj ) = 0;

	virtual void SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  ) = 0;

	virtual void SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj ) = 0;

	virtual void GetWorldTransform( D3DXMATRIX& matWorld ) const = 0;

	virtual void GetViewTransform( D3DXMATRIX& matView ) const = 0;
};


//================================== inline implementations ==================================

inline void CD3DShaderManager::SetWorldTransform( const Matrix34& world_pose )
{
	D3DXMATRIX matWorld;
	world_pose.GetRowMajorMatrix44( matWorld );

	SetWorldTransform( matWorld );
}


inline void CD3DShaderManager::SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView )
{
	D3DXMATRIX world, view;
	matWorld.GetRowMajorMatrix44( (Scalar *)&world );
	matView.GetRowMajorMatrix44( (Scalar *)&view );
	SetWorldViewTransform( world, view );
}


inline void CD3DShaderManager::SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj )
{
	D3DXMATRIX world, view, proj;
	matWorld.GetRowMajorMatrix44( (Scalar *)&world );
	matView.GetRowMajorMatrix44( (Scalar *)&view );
	matProj.GetRowMajorMatrix44( (Scalar *)&proj );
	SetWorldViewProjectionTransform( world, view, proj );
}


inline void CD3DShaderManager::GetWorldTransform( Matrix44& matWorld ) const
{
	D3DXMATRIX world;
	GetWorldTransform( world );
	matWorld.SetRowMajorMatrix44( (Scalar *)&world );
}


inline void CD3DShaderManager::GetViewTransform( Matrix44& matView ) const
{
	D3DXMATRIX view;
	GetViewTransform( view );
	matView.SetRowMajorMatrix44( (Scalar *)&view );
}


/**
 - When created, automatically registers itself to HLSLShaderManagerHub



*/
class CHLSLShaderManager : public CD3DShaderManager
{
	enum eShaderConstParam
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

	enum eMatHandleID
	{
		MATRIX_WORLD = 0,
		MATRIX_VIEW,
		MATRIX_PROJ,
		MATRIX_WORLD_VIEW,
		MATRIX_WORLD_VIEW_PROJ,
		NUM_MATRIX_HANDLES
	};

	class CD3DShaderParameterHandle
	{
	public:
		std::string ParameterName;
		D3DXHANDLE Handle;

	public:

		CD3DShaderParameterHandle( const std::string& name, D3DXHANDLE handle )
			:
		ParameterName(name),
		Handle(handle)
		{}
	};


	std::string m_strFilename;

	LPD3DXEFFECT	m_pEffect;

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

//	UINT m_Passes;

	boost::shared_ptr<CHLSLShaderLightManager> m_pHLSLShaderLightManager;

private:

	HRESULT SetNewTechnique( CShaderTechniqueHandle& tech_handle );

	void UpdateVacantTechniqueIndex();

	void PrintCompilerErrors( LPD3DXBUFFER pCompileErrors );

	bool Init();

public:

	CHLSLShaderManager();

	~CHLSLShaderManager();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	void Release();

	void Reload();

	inline LPD3DXEFFECT GetEffect() { return m_pEffect; }


	inline void SetViewerPosition( const D3DXVECTOR3& vEyePosition );


	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );

	inline Result::Name SetTexture( const int iStage, const CTextureHandle& texture );

	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

	inline void Begin();

	inline void End();

//	inline void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture );

	inline Result::Name SetTechnique( const unsigned int id );

	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );

	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

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


//================================== inline implementations ==================================

inline void CHLSLShaderManager::SetWorldTransform( const D3DXMATRIX& matWorld )
{
//	m_pEffect->SetMatrix( "World", &matWorld );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );

	D3DXMATRIX matView;
//	m_pEffect->GetMatrix ( "View", &matView );
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_VIEW], &matView );

	D3DXMatrixMultiply( &matView, &matWorld, &matView );

//	m_pEffect->SetMatrix( "WorldView", &matView );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW], &matView );

	// set WorldViewProj
	D3DXMATRIX matProj;
//	m_pEffect->GetMatrix ( "Proj", &matProj );
	m_pEffect->GetMatrix ( m_aMatrixHandle[MATRIX_PROJ], &matProj );

	D3DXMatrixMultiply( &matProj, &matView, &matProj );

//	m_pEffect->SetMatrix( "WorldViewProj", &matProj );
	m_pEffect->SetMatrix ( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matProj );
}


inline void CHLSLShaderManager::SetViewTransform( const D3DXMATRIX& matView )
{
	m_pEffect->SetMatrix( "View", &matView );

	D3DXMATRIX matWorld;
	m_pEffect->GetMatrix ( "World", &matWorld );

	D3DXMatrixMultiply( &matWorld, &matWorld, &matView );

	m_pEffect->SetMatrix( "WorldView", &matWorld );

	// set WorldViewProj
	D3DXMATRIX matProj;
	m_pEffect->GetMatrix ( "Proj", &matProj );

	D3DXMatrixMultiply( &matProj, &matWorld, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matProj );}


inline void CHLSLShaderManager::SetProjectionTransform( const D3DXMATRIX& matProj )
{
	m_pEffect->SetMatrix( "Proj", &matProj );

	D3DXMATRIX matWorldView;
	m_pEffect->GetMatrix( "WorldView", &matWorldView );

	D3DXMatrixMultiply( &matWorldView, &matWorldView, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matWorldView );
}


inline void CHLSLShaderManager::SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  )
{
	m_pEffect->SetMatrix( "World", &matWorld );

	m_pEffect->SetMatrix( "View", &matView );

	D3DXMATRIX matWorldView;

	D3DXMatrixMultiply( &matWorldView, &matWorld, &matView );

	m_pEffect->SetMatrix( "WorldView", &matWorldView );

	D3DXMATRIX matProj;
	m_pEffect->GetMatrix ( "Proj", &matProj );
	D3DXMatrixMultiply( &matWorldView, &matWorldView, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matWorldView );
}


inline void CHLSLShaderManager::SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld,
															 const D3DXMATRIX& matView,
															 const D3DXMATRIX& matProj )
{
	m_pEffect->SetMatrix( "World", &matWorld );
	m_pEffect->SetMatrix( "View", &matView );
	m_pEffect->SetMatrix( "Proj", &matProj );

	D3DXMATRIX matTrans;

	D3DXMatrixMultiply( &matTrans, &matWorld, &matView );

	m_pEffect->SetMatrix( "WorldView", &matTrans );


	D3DXMatrixMultiply( &matTrans, &matTrans, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matTrans);


/*	D3DXMATRIX matWorldView_ = matWorldView;	// save the original world view for test

	D3DXMatrixInverse( &matWorldView, NULL, &matWorldView );
	pEffect->SetMatrixTranspose( "WorldViewIT", &matWorldView );

	D3DXMatrixInverse( &matView, NULL, &matView );
	pEffect->SetMatrixTranspose( "ViewIT", &matView );

	pEffect->CommitChanges();

	D3DXMatrixTranspose( &matWorldView, &matWorldView );*/
}


inline void CHLSLShaderManager::GetWorldTransform( D3DXMATRIX& matWorld ) const
{
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );
}


inline void CHLSLShaderManager::GetViewTransform( D3DXMATRIX& matView ) const
{
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_VIEW], &matView );
}


inline void CHLSLShaderManager::SetViewerPosition( const D3DXVECTOR3& vEyePosition )
{
	m_pEffect->SetValue( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(D3DXVECTOR3) );
}


inline HRESULT CHLSLShaderManager::SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture )
{
	return m_pEffect->SetTexture( m_aTextureHandle[iStage], pTexture );
}


inline Result::Name CHLSLShaderManager::SetTexture( const int iStage, const CTextureHandle& texture )
{
	HRESULT hr = m_pEffect->SetTexture( m_aTextureHandle[iStage], texture.GetTexture() );
	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline HRESULT CHLSLShaderManager::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	if( m_aCubeTextureHandle[index] )
        return m_pEffect->SetTexture( m_aCubeTextureHandle[index], pCubeTexture );
	else
		return E_FAIL;
}


inline void CHLSLShaderManager::Begin()
{
//	m_pEffect->Begin( &m_Passes, 0 );
}


inline void CHLSLShaderManager::End()
{
//	m_pEffect->End();
}


inline Result::Name CHLSLShaderManager::SetTechnique( const unsigned int id )
{
	HRESULT hr = m_pEffect->SetTechnique( m_aTechniqueHandle[id] );

	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}


inline Result::Name CHLSLShaderManager::SetTechnique( CShaderTechniqueHandle& tech_handle )
{
	const int tech_index = GetTechniqueIndex( tech_handle );

	HRESULT hr;
	if( 0 <= tech_index )
	{
		// valid index has already been set to handle
		hr = m_pEffect->SetTechnique( m_aTechniqueHandle[tech_index] );
	}
	else
		hr = SetNewTechnique( tech_handle );

	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}



#endif /* __D3DShaderManager_HPP__ */
