#ifndef __HLSLShaderManager_HPP__
#define __HLSLShaderManager_HPP__


#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include "3DMath/Matrix34.hpp"
#include "ShaderTechniqueHandle.hpp"
#include "ShaderParameter.hpp"

#include "../../base.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/TextureHandle.hpp"

#include "Support/stream_buffer.hpp"


typedef D3DXMATRIX Matrix44;

class CHLSLShaderLightManager;


class CShaderManager
{
/*
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
*/
	bool m_RegisteredToHub;

private:

	virtual bool Init() { return true; }

protected:

	int GetTechniqueIndex( CShaderTechniqueHandle& tech_handle ) { return tech_handle.GetTechniqueIndex(); }

	void SetInvalidTechnique( CShaderTechniqueHandle& tech_handle ) { tech_handle.SetTechniqueIndex( CShaderTechniqueHandle::INVALID_INDEX ); }

	void SetTechniqueIndex( CShaderTechniqueHandle& tech_handle, int index ) { return tech_handle.SetTechniqueIndex( index ); }

	bool IsUninitializedTechnique( CShaderTechniqueHandle& tech_handle ) { return tech_handle.GetTechniqueIndex() == CShaderTechniqueHandle::UNINITIALIZED; }

	bool IsInvalidTechnique( CShaderTechniqueHandle& tech_handle ) { return tech_handle.GetTechniqueIndex() == CShaderTechniqueHandle::INVALID_INDEX; }

	template<typename T>
	T GetParameterValue( CShaderParameter<T> param ) { return param.m_Parameter; }

	template<typename T>
	int GetParameterIndex( CShaderParameter<T> param ) { return param.m_ParameterIndex; }

	template<typename T>
	void SetParameterIndex( CShaderParameter<T>& param, int index ) { param.m_ParameterIndex = index; }

public:

	CShaderManager();

	virtual ~CShaderManager();

	virtual bool LoadShaderFromFile( const std::string& filename ) { return false; }

	virtual bool LoadShaderFromText( const stream_buffer& buffer ) { return false; }

	virtual LPD3DXEFFECT GetEffect() { return NULL; }

	virtual void Release() {}

	virtual void Reload() {}

	virtual void SetWorldTransform( const Matrix34& world_pose ) {}

	virtual void SetWorldTransform( const Matrix44& matWorld ) {}

	virtual void SetViewTransform( const Matrix44& matView ) {}

	virtual void SetProjectionTransform( const Matrix44& matProj ) {}

	virtual void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  ) {}

	virtual void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj ) {}


	virtual void GetWorldTransform( Matrix44& matWorld ) const {}

	virtual void GetViewTransform( Matrix44& matView ) const {}


	virtual inline void SetViewerPosition( const D3DXVECTOR3& vEyePosition ) {}


	virtual HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture ) { return E_FAIL; }

	virtual HRESULT SetTexture( const int iStage, const CTextureHandle& texture ) { return E_FAIL; }

	virtual HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture ) { return E_FAIL; }

//	virtual void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture ) {}

	virtual Result::Name SetTechnique( const unsigned int id ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle ) { return Result::UNKNOWN_ERROR; }

//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

//	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	virtual void SetParam( CShaderParameter< std::vector<float> >& float_param ) {}

//	void SetParam( CShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()

	virtual boost::shared_ptr<CShaderLightManager> GetShaderLightManager() { return boost::shared_ptr<CShaderLightManager>(); }

	friend class CShaderManagerHub;
};


/**
 - When created, automatically registers itself to HLSLShaderManagerHub



*/
class CHLSLShaderManager : public CShaderManager
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

	inline void SetWorldTransform( const Matrix34& world_pose );

	inline void SetWorldTransform( const Matrix44& matWorld );

	inline void SetViewTransform( const Matrix44& matView );

	inline void SetProjectionTransform( const Matrix44& matProj );

	inline void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  );

	inline void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj );


	inline void GetWorldTransform( Matrix44& matWorld ) const;

	inline void GetViewTransform( Matrix44& matView ) const;


	inline void SetViewerPosition( const D3DXVECTOR3& vEyePosition );


	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );

	inline HRESULT SetTexture( const int iStage, const CTextureHandle& texture );

	inline HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

//	inline void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture );

	inline Result::Name SetTechnique( const unsigned int id );

	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );

	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	void SetParam( CShaderParameter< std::vector<float> >& float_param );

//	void SetParam( CShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()

	boost::shared_ptr<CShaderLightManager> GetShaderLightManager();
};


//================================== inline implementations ==================================


inline void CHLSLShaderManager::SetWorldTransform( const Matrix34& world_pose )
{
	D3DXMATRIX matWorld;
	world_pose.GetRowMajorMatrix44( matWorld );

	SetWorldTransform( matWorld );
}


inline void CHLSLShaderManager::SetWorldTransform( const Matrix44& matWorld )
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


inline void CHLSLShaderManager::SetViewTransform( const Matrix44& matView )
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


inline void CHLSLShaderManager::SetProjectionTransform( const Matrix44& matProj )
{
	m_pEffect->SetMatrix( "Proj", &matProj );

	D3DXMATRIX matWorldView;
	m_pEffect->GetMatrix( "WorldView", &matWorldView );

	D3DXMatrixMultiply( &matWorldView, &matWorldView, &matProj );

	m_pEffect->SetMatrix( "WorldViewProj", &matWorldView );
}


inline void CHLSLShaderManager::SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  )
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


inline void CHLSLShaderManager::SetWorldViewProjectionTransform( const Matrix44& matWorld,
															 const Matrix44& matView,
															 const Matrix44& matProj )
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


inline void CHLSLShaderManager::GetWorldTransform( Matrix44& matWorld ) const
{
	m_pEffect->GetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );
}


inline void CHLSLShaderManager::GetViewTransform( Matrix44& matView ) const
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


inline HRESULT CHLSLShaderManager::SetTexture( const int iStage, const CTextureHandle& texture )
{
	return m_pEffect->SetTexture( m_aTextureHandle[iStage], texture.GetTexture() );
}


inline HRESULT CHLSLShaderManager::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	if( m_aCubeTextureHandle[index] )
        return m_pEffect->SetTexture( m_aCubeTextureHandle[index], pCubeTexture );
	else
		return E_FAIL;
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


#endif  /*  __HLSLShaderManager_HPP__  */
