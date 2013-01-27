#ifndef __CgEffectBase_HPP__
#define __CgEffectBase_HPP__


#include "ShaderManager.hpp"
#include <Cg/cg.h>     /* Cg Core API: Can't include this?  Is Cg Toolkit installed! */


namespace amorphous
{


#pragma comment( lib, "cg.lib" )


class CCgEffectBase : public ShaderManager
{
	enum ShaderConstParams
	{
		NUM_MAX_TECHNIQUES = 64,
		NUM_TEXTURE_STAGES = 8,
		NUM_MAX_CUBETEXTURES = 4,
		NUM_MAX_VERTEX_BLEND_TRANSFORMS = 128,
	};

	enum HandleID
	{
		HANDLE_VIEWER_POS = 0,
		HANDLE_AMBIENT_COLOR,
		NUM_HANDLES,
	};

	enum MatrixHandleID
	{
		MATRIX_WORLD = 0,
		MATRIX_VIEW,
		MATRIX_PROJ,
		MATRIX_WORLD_VIEW,
		MATRIX_WORLD_VIEW_PROJ,
		NUM_MATRIX_HANDLES
	};

	CGpass m_FirstPass;

protected:

	class CCgShaderParameterHandle
	{
	public:
		std::string ParameterName;
		CGparameter Handle;

	public:

		CCgShaderParameterHandle( const std::string& name, CGparameter handle )
			:
		ParameterName(name),
		Handle(handle)
		{}
	};

	std::string m_EffectPath;

	CGeffect m_CgEffect;

	CGcontext m_CgContext;

	CGparameter m_aHandle[NUM_HANDLES];

	CGparameter m_aMatrixHandle[NUM_MATRIX_HANDLES];

	CGparameter m_aTextureHandle[NUM_TEXTURE_STAGES];

	CGparameter m_aCubeTextureHandle[NUM_MAX_CUBETEXTURES];


	CGtechnique m_aTechniqueHandle[NUM_MAX_TECHNIQUES];

	std::string m_astrTechniqueName[NUM_MAX_TECHNIQUES];

	std::vector<CCgShaderParameterHandle> m_vecParamHandle;

	CGparameter m_aVertexBlendTransform[NUM_MAX_VERTEX_BLEND_TRANSFORMS];

	int m_NumBlendTransforms;

	Matrix44 m_matWorld;
	Matrix44 m_matView;
	Matrix44 m_matProj;

	/// the first vacant table entry for registering a technique
	/// If m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES,
	/// it indicates no vacancy is left
	int m_VacantTechniqueEntryIndex;

//	UINT m_Passes;

//	boost::shared_ptr<CHLSLShaderLightManager> m_pHLSLShaderLightManager;

private:

	void LoadNullShader();

	bool Init();

	template<typename T>
	inline int RegisterCgParam( CShaderParameter<T>& param );

	Result::Name SetNewTechnique( CShaderTechniqueHandle& tech_handle );

	void UpdateVacantTechniqueIndex();

protected:

	void CheckForCgError( const char *situation );

	virtual void SetCGTextureParameter( CGparameter& param, TextureHandle& texture ) = 0;

public:

	CCgEffectBase();

	virtual ~CCgEffectBase(){}

	bool LoadShaderFromFile( const std::string& filename );

	void Release();

//	inline void Begin();

//	inline void End();

	inline Result::Name SetTechnique( const unsigned int id );

	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );

	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	CGtechnique GetTechniqueHandle( int id ) { return m_aTechniqueHandle[id]; }

	// Sets a single integer value
	void SetParam( CShaderParameter<int>& int_param );

	// Sets a single float value
	void SetParam( CShaderParameter<float>& float_param );

	// Sets a single float value
	void SetParam( CShaderParameter<Vector3>& vec3_param );

	// Sets a color value as 4 floats in RGBA order
	void SetParam( CShaderParameter<SFloatRGBAColor>& color_param );

	// Sets one or more float values
	void SetParam( CShaderParameter< std::vector<float> >& float_param );

	void SetParam( CShaderParameter<CTextureParam>& tex_param );

	// Sets a column-major 4x4 matrix
	void SetParam( CShaderParameter<Matrix44>& mat44_param );

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

	boost::shared_ptr<CShaderLightManager> GetShaderLightManager();

	inline void SetWorldTransform( const Matrix44& matWorld );

	inline void SetViewTransform( const Matrix44& matView );

	inline void SetProjectionTransform( const Matrix44& matProj );

	inline void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  );

	inline void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj );


	inline void GetWorldTransform( Matrix44& matWorld ) const;

	inline void GetViewTransform( Matrix44& matView ) const;

	void InitBlendTransformVariables( const std::string& variable_name );

	void InitBlendTransformVariables();

//	inline void SetViewerPosition( const Vector3& vEyePosition );

	void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms );

	const CGpass& GetFirstPass() const { return m_FirstPass; }
};


template<typename T>
inline int CCgEffectBase::RegisterCgParam( CShaderParameter<T>& param )
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
			CGparameter param_handle = cgGetNamedEffectParameter( m_CgEffect, param.GetParameterName().c_str() );
			if( param_handle )
			{
				index = (int)m_vecParamHandle.size();
				SetParameterIndex( param, index );
				m_vecParamHandle.push_back( CCgShaderParameterHandle(param.GetParameterName(),param_handle) );
			}
		}
	}

	return index;
}


inline Result::Name CCgEffectBase::SetTechnique( const unsigned int id )
{
	m_FirstPass = cgGetFirstPass( m_aTechniqueHandle[id] );
	return cgIsPass( m_FirstPass ) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline Result::Name CCgEffectBase::SetTechnique( CShaderTechniqueHandle& tech_handle )
{
	const int tech_index = GetTechniqueIndex( tech_handle );

	if( 0 <= tech_index )
	{
		// valid index has already been set to handle
		m_FirstPass = cgGetFirstPass( m_aTechniqueHandle[tech_index] );
		return cgIsPass( m_FirstPass ) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
	}
	else
		return SetNewTechnique( tech_handle );
}


inline void CCgEffectBase::SetWorldTransform( const Matrix44& matWorld )
{
	// Update cache
	m_matWorld = matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD], matWorld.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a world matrix." );

	// view * world
	Matrix44 matViewWorld = m_matView * matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW], matViewWorld.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a WorldView matrix." );

	// proj * view * world
	Matrix44 matProjViewWorld = m_matProj * matViewWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], matProjViewWorld.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a WorldViewProj matrix." );
}


inline void CCgEffectBase::SetViewTransform( const Matrix44& matView )
{
	// Update cache
	m_matView = matView;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_VIEW], matView.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a view matrix." );

	// view * world
	Matrix44 matViewWorld = matView * m_matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW], matViewWorld.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a view*world matrix." );

	// proj * view * world
	Matrix44 matProjViewWorld = m_matProj * matViewWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], matProjViewWorld.GetData() );
	CheckForCgError( "Called cgSetMatrixParameterfc() a proj*view*world matrix." );
}


inline void CCgEffectBase::SetProjectionTransform( const Matrix44& matProj )
{
	// Update cache
	m_matProj = matProj;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_PROJ], matProj.GetData() );

	// proj * view * world
	Matrix44 matProjViewWorld = matProj * m_matView * m_matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], matProjViewWorld.GetData() );
}


inline void CCgEffectBase::SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView )
{
	// Update cache
	m_matWorld = matWorld;
	m_matView  = matView;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD], matWorld.GetData() );
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_VIEW], matView.GetData() );

	// view * world
	Matrix44 matViewWorld = matView * matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW], matViewWorld.GetData() );

	// proj * view * world
	Matrix44 matProjViewWorld = m_matProj * matViewWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], matProjViewWorld.GetData() );
}


inline void CCgEffectBase::SetWorldViewProjectionTransform( const Matrix44& matWorld,
														   const Matrix44& matView,
														   const Matrix44& matProj )
{
	// Update cache
	m_matWorld = matWorld;
	m_matView  = matView;
	m_matProj  = matProj;

	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD], matWorld.GetData() );
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_VIEW],  matView.GetData() );
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_PROJ],  matProj.GetData() );

	// view * world
	Matrix44 matViewWorld = matView * matWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW], matViewWorld.GetData() );

	// proj * view * world
	Matrix44 matProjViewWorld = m_matProj * matViewWorld;
	cgSetMatrixParameterfc( m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ], matProjViewWorld.GetData() );
}


inline void CCgEffectBase::GetWorldTransform( Matrix44& matWorld ) const
{
	matWorld = m_matWorld;
}


inline void CCgEffectBase::GetViewTransform( Matrix44& matView ) const
{
	matView = m_matView;
}

/*
inline void CCgEffectBase::SetViewerPosition( const Vector3& vEyePosition )
{
	cgSetParameter3fv( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(Vector3) );
}
*/

} // namespace amorphous



#endif /* __CgEffectBase_HPP__ */
