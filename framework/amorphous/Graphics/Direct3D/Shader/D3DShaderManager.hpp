#ifndef __D3DShaderManager_HPP__
#define __D3DShaderManager_HPP__


#include "Graphics/Direct3D/fwd.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "3DMath/Transform.hpp"


namespace amorphous
{


class CD3DShaderManager : public ShaderManager
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
		NUM_MAX_CUBETEXTURES = 4,
		NUM_MAX_VERTEX_BLEND_TRANSFORMS = 128,
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

	D3DXHANDLE m_aVertexBlendTransform[NUM_MAX_VERTEX_BLEND_TRANSFORMS];

	int m_NumBlendTransforms;

	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	/// the first vacant table entry for registering a technique
	/// If m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES,
	/// it indicates no vacancy is left
	int m_VacantTechniqueEntryIndex;

//	UINT m_Passes;

	boost::shared_ptr<CHLSLShaderLightManager> m_pHLSLShaderLightManager;

private:

	HRESULT SetNewTechnique( ShaderTechniqueHandle& tech_handle );

	void UpdateVacantTechniqueIndex();

	void PrintCompilerErrors( LPD3DXBUFFER pCompileErrors );

	bool Init();

	void LoadNullShader();

	template<typename T>
	inline int RegisterHLSLParam( ShaderParameter<T>& param );

public:

	CHLSLShaderManager();

	~CHLSLShaderManager();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	void Release();

	void Reload();

	void SetHandlesToNULL();

	inline LPD3DXEFFECT GetEffect() { return m_pEffect; }


	inline void SetViewerPosition( const Vector3& vEyePosition );

//	inline void SetVertexBlendMatrix( int i, const Matrix34& mat );

//	inline void SetVertexBlendMatrix( int i, const Matrix44& mat );


//	inline HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture );

	inline Result::Name SetTexture( const int iStage, const TextureHandle& texture );

	inline Result::Name SetCubeTexture( const int index, const TextureHandle& cube_texture );

	inline void Begin();

	inline void End();

	inline Result::Name SetTechnique( const unsigned int id );

	inline Result::Name SetTechnique( ShaderTechniqueHandle& tech_handle );

	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	D3DXHANDLE GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	// Sets a single integer value
	void SetParam( ShaderParameter<int>& int_param );

	// Sets a single float value
	void SetParam( ShaderParameter<float>& float_param );

	// Sets a single float2 (Vector2) value
	void SetParam( ShaderParameter<Vector2>& vec2_param );

	// Sets a single float3 (Vector3) value
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

	// Sets a single float2 (Vector2) value
	void SetParam( const char *parameter_name, const Vector2& vec2_param );

	// Sets a single float3 (Vector3) value
	void SetParam( const char *parameter_name, const Vector3& vec3_param );

	// Sets a color value as 4 floats in RGBA order
	void SetParam( const char *parameter_name, const SFloatRGBAColor& color_param );

	// Sets one or more float values
	void SetParam( const char *parameter_name, const float *float_param, uint num_float_values );

	// Sets a Vector2 array
	void SetParam( const char *parameter_name, const Vector2 *vec2_param, uint num_vec2_values );

	// Sets a Vector3 array
	void SetParam( const char *parameter_name, const Vector3 *vec3_param, uint num_vec3_values );

	// Sets a Vector4 array
	void SetParam( const char *parameter_name, const Vector4 *vec4_param, uint num_vec4_values );

	// Sets a column-major 4x4 matrix
	void SetParam( const char *parameter_name, const Matrix44& mat44_param );

	void SetBool( const char *parameter_name, bool bool_param );

//	void SetTextureParam()

	boost::shared_ptr<ShaderLightManager> GetShaderLightManager();

	inline void SetWorldTransform( const D3DXMATRIX& matWorld );

	inline void SetViewTransform( const D3DXMATRIX& matView );

	inline void SetProjectionTransform( const D3DXMATRIX& matProj );

	inline void SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView  );

	inline void SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj );

	inline void GetWorldTransform( D3DXMATRIX& matWorld ) const;

	inline void GetViewTransform( D3DXMATRIX& matView ) const;

	void InitBlendTransformVariables( const std::string& variable_name );

	void InitBlendTransformVariables();

	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );
};


//================================== inline implementations ==================================

inline void CHLSLShaderManager::SetWorldTransform( const D3DXMATRIX& matWorld )
{
	// Update cache
	m_matWorld = matWorld;

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );

	D3DXMATRIX matWorldView;
	D3DXMatrixMultiply( &matWorldView, &matWorld, &m_matView );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW], &matWorldView );

	// set WorldViewProj
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &m_matProj );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matWorldViewProj );
}


inline void CHLSLShaderManager::SetViewTransform( const D3DXMATRIX& matView )
{
	// Update cache
	m_matView = matView;

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_VIEW], &matView );

	D3DXMATRIX matWorldView;
	D3DXMatrixMultiply( &matWorldView, &m_matWorld, &matView );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW], &matWorldView );

	// set WorldViewProj
	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &m_matProj );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matWorldViewProj );
}


inline void CHLSLShaderManager::SetProjectionTransform( const D3DXMATRIX& matProj )
{
	// Update cache
	m_matProj = matProj;

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_PROJ], &matProj );

	D3DXMATRIX matWorldView;
	D3DXMatrixMultiply( &matWorldView, &m_matWorld, &m_matView );

	D3DXMatrixMultiply( &matWorldView, &matWorldView, &matProj );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matWorldView );
}


inline void CHLSLShaderManager::SetWorldViewTransform( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView )
{
	// Update cache
	m_matWorld = matWorld;
	m_matView  = matView;

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_VIEW], &matView );

	D3DXMATRIX matWorldView;
	D3DXMatrixMultiply( &matWorldView, &matWorld, &matView );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW], &matWorldView );

	D3DXMATRIX matWorldViewProj;
	D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &m_matProj );

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matWorldViewProj );
}


inline void CHLSLShaderManager::SetWorldViewProjectionTransform( const D3DXMATRIX& matWorld,
															 const D3DXMATRIX& matView,
															 const D3DXMATRIX& matProj )
{
	// Update cache
	m_matWorld = matWorld;
	m_matView  = matView;
	m_matProj  = matProj;

	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD], &matWorld );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_VIEW],  &matView );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_PROJ],  &matProj );

	D3DXMATRIX matTrans;

	D3DXMatrixMultiply( &matTrans, &matWorld, &matView );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW], &matTrans );

	D3DXMatrixMultiply( &matTrans, &matTrans, &matProj );
	m_pEffect->SetMatrix( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], &matTrans );


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
	matWorld = m_matWorld;
}


inline void CHLSLShaderManager::GetViewTransform( D3DXMATRIX& matView ) const
{
	matView = m_matView;
}


inline void CHLSLShaderManager::SetViewerPosition( const Vector3& vEyePosition )
{
	m_pEffect->SetValue( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(D3DXVECTOR3) );
}

/*
inline void CHLSLShaderManager::SetVertexBlendMatrix( int i, const Matrix34& mat )
{
	if( i < 0 || (int)m_vecVertBlendMatrixHandle.size() <= i )
		return;

	const Matrix33& r = mat.matOrient; // rotation
	const Vector3& t = mat.vPosition; // translation
	D3DXMATRIX src; // row-major matrix
	src._11 = r(0,0); src._12 = r(1,0); src._13 = r(2,0); src._14 = 0.0;
	src._21 = r(0,1); src._22 = r(1,1); src._23 = r(2,1); src._24 = 0.0;
	src._31 = r(0,2); src._32 = r(1,2); src._33 = r(2,2); src._34 = 0.0;
	src._41 = t.x;    src._42 = t.y;    src._43 = t.z;    src._44 = 1.0;

	m_pEffect->SetMatrix( m_vecVertBlendMatrixHandle[i], &src );
}


inline void CHLSLShaderManager::SetVertexBlendMatrix( int i, const Matrix44& mat )
{
	if( i < 0 || (int)m_vecVertBlendMatrixHandle.size() <= i )
		return;

	m_pEffect->SetMatrixTranspose( m_vecVertBlendMatrixHandle[i], (D3DXMATRIX*)&mat );
}
*/

//inline HRESULT CHLSLShaderManager::SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture )
//{
//	return m_pEffect->SetTexture( m_aTextureHandle[iStage], pTexture );
//}


inline Result::Name CHLSLShaderManager::SetTexture( const int iStage, const TextureHandle& texture )
{
	HRESULT hr = E_FAIL;
	if( m_aTextureHandle[iStage] )
		hr = m_pEffect->SetTexture( m_aTextureHandle[iStage], texture.GetTexture() );
	else
	{
		// We assume that the shader uses register()s, e.g. "sampler s0 : register(s0);".
		hr = DIRECT3D9.GetDevice()->SetTexture( iStage, texture.GetTexture() );
	}

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline Result::Name CHLSLShaderManager::SetCubeTexture( int index, const TextureHandle& cube_texture )
{
	if( m_aCubeTextureHandle[index] )
	{
        HRESULT hr = m_pEffect->SetTexture( m_aCubeTextureHandle[index], cube_texture.GetTexture() );
		return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}
	else
		return Result::UNKNOWN_ERROR;
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


inline Result::Name CHLSLShaderManager::SetTechnique( ShaderTechniqueHandle& tech_handle )
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


template<typename T>
inline int CHLSLShaderManager::RegisterHLSLParam( ShaderParameter<T>& param )
{
//	int index = float_param.m_ParameterIndex;
	int index = GetParameterIndex( param );
	if( index < 0 )
	{
		// init

		size_t i;
		for( i=0; i<m_vecParamHandle.size(); i++ )
		{
			if( m_vecParamHandle[i].ParameterName == param.GetParameterName() )
			{
				index = (int)i;
				SetParameterIndex( param, index );
				break;
			}
		}

		if( i == m_vecParamHandle.size() )
		{
			// not found - get the parameter from the name
			D3DXHANDLE param_handle = m_pEffect->GetParameterByName( NULL, param.GetParameterName().c_str() );
			if( param_handle )
			{
				index = (int)m_vecParamHandle.size();
				SetParameterIndex( param, index );
				m_vecParamHandle.push_back( CD3DShaderParameterHandle(param.GetParameterName(),param_handle) );
			}
		}
	}

	return index;
}


} // namespace amorphous



#endif /* __D3DShaderManager_HPP__ */
