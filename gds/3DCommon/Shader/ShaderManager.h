
#ifndef __SHADERMANAGER_S_H__
#define __SHADERMANAGER_S_H__


#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#include "ShaderTechniqueHandle.h"

#include "3DCommon/TextureHandle.h"


class CShaderManager
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


	std::string m_strFilename;

	LPD3DXEFFECT	m_pEffect;

	D3DXHANDLE m_aHandle[NUM_HANDLES];

	D3DXHANDLE m_aMatrixHandle[NUM_MATRIX_HANDLES];

	D3DXHANDLE m_aTextureHandle[NUM_TEXTURE_STAGES];

	D3DXHANDLE m_aCubeTextureHandle[NUM_MAX_CUBETEXTURES];


	D3DXHANDLE m_aTechniqueHandle[NUM_MAX_TECHNIQUES];

	std::string m_astrTechniqueName[NUM_MAX_TECHNIQUES];

	/// the first vacant table entry for registering a technique
	/// If m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES,
	/// it indicates no vacancy is left
	int m_VacantTechniqueEntryIndex;

	HRESULT SetNewTechnique( CShaderTechniqueHandle& tech_handle );

	void UpdateVacantTechniqueIndex();

public:
	
	CShaderManager();

	~CShaderManager();

	bool LoadShaderFromFile( const std::string& filename );

	void Release();

	inline LPD3DXEFFECT GetEffect() { return m_pEffect; }

	inline void SetWorldTransform( const D3DXMATRIX& matWorld );

	inline void SetViewTransform( const D3DXMATRIX& matView );

	inline void SetProjectionTransform( const D3DXMATRIX& matProj );

	inline void SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  );

	inline void SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj );


	inline void GetWorldTransform( D3DXMATRIX& matWorld ) const;

	inline void GetViewTransform( D3DXMATRIX& matView ) const;


	inline void SetViewerPosition( const D3DXVECTOR3& vEyePosition );


	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );

	inline HRESULT SetTexture( const int iStage, const CTextureHandle& texture );

	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

//	inline void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture );

	inline HRESULT SetTechnique( const unsigned int id );

	inline HRESULT SetTechnique( CShaderTechniqueHandle& tech_handle );

	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }


//	void SetLight( int index, CLight light );
};


//================================== inline implementations ==================================


inline void CShaderManager::SetWorldTransform( const D3DXMATRIX& matWorld )
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


inline void CShaderManager::SetViewTransform( const D3DXMATRIX& matView )
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


inline void CShaderManager::SetProjectionTransform( const D3DXMATRIX& matProj )
{
	m_pEffect->SetMatrix( "Proj", &matProj );

	D3DXMATRIX matWorldView;
	m_pEffect->GetMatrix( "WorldView", &matWorldView );

	D3DXMatrixMultiply( &matWorldView, &matWorldView, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matWorldView );
}


inline void CShaderManager::SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  )
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


inline void CShaderManager::SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld,
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


inline void CShaderManager::GetWorldTransform( D3DXMATRIX& matWorld ) const
{
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );
}


inline void CShaderManager::GetViewTransform( D3DXMATRIX& matView ) const
{
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_VIEW], &matView );
}


inline void CShaderManager::SetViewerPosition( const D3DXVECTOR3& vEyePosition )
{
	m_pEffect->SetValue( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(D3DXVECTOR3) );
}


inline HRESULT CShaderManager::SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture )
{
	return m_pEffect->SetTexture( m_aTextureHandle[iStage], pTexture );
}


inline HRESULT CShaderManager::SetTexture( const int iStage, const CTextureHandle& texture )
{
	return m_pEffect->SetTexture( m_aTextureHandle[iStage], texture.GetTexture() );
}


inline HRESULT CShaderManager::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	if( m_aCubeTextureHandle[index] )
        return m_pEffect->SetTexture( m_aCubeTextureHandle[index], pCubeTexture );
	else
		return E_FAIL;
}


inline HRESULT CShaderManager::SetTechnique( const unsigned int id )
{
	return m_pEffect->SetTechnique( m_aTechniqueHandle[id] );
}


inline HRESULT CShaderManager::SetTechnique( CShaderTechniqueHandle& tech_handle )
{
	const int tech_index = tech_handle.GetTequniqueIndex();

	if( 0 <= tech_index )
	{
		// valid index has already been set to handle
		return m_pEffect->SetTechnique( m_aTechniqueHandle[tech_index] );
	}
	else
		return SetNewTechnique( tech_handle );
}


#endif  /*  __SHADERMANAGER_S_H__  */
