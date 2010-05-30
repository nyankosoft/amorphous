#ifndef __ShaderManager_HPP__
#define __ShaderManager_HPP__


#include <d3dx9.h>
#include <boost/shared_ptr.hpp>

#include "../../base.hpp"
#include "3DMath/Matrix34.hpp"
#include "3DMath/Matrix44.hpp"
#include "3DMath/Transform.hpp"
#include "ShaderTechniqueHandle.hpp"
#include "ShaderParameter.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Support/stream_buffer.hpp"


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


	virtual void SetViewerPosition( const Vector3& vEyePosition ) {}

	virtual void SetVertexBlendMatrix( int i, const Matrix34& mat ) {}

	virtual void SetVertexBlendMatrix( int i, const Matrix44& mat ) {}

	virtual void SetVertexBlendTransforms( const std::vector<Transform>& src_transforms ) {}


	virtual HRESULT SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture ) { return E_FAIL; }

	virtual Result::Name SetTexture( const int iStage, const CTextureHandle& texture ) { return Result::UNKNOWN_ERROR; }

	virtual HRESULT SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture ) { return E_FAIL; }

	virtual void Begin() {}

	virtual void End() {}

//	virtual void SetTexture( const char *pName, const LPDIRECT3DTEXTURE9 pTexture ) {}

	virtual Result::Name SetTechnique( const unsigned int id ) { return Result::UNKNOWN_ERROR; }

	virtual Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle ) { return Result::UNKNOWN_ERROR; }

//	bool RegisterTechnique( const unsigned int id, const char *pcTechnique );

	// Sets a single integer value
	virtual void SetParam( CShaderParameter<int>& int_param ) {}

	// Sets a single float value
	virtual void SetParam( CShaderParameter<float>& float_param ) {}

	// Sets a single float value
	virtual void SetParam( CShaderParameter<Vector3>& vec3_param ) {}

	// Sets a color value as 4 floats in RGBA order
	virtual void SetParam( CShaderParameter<SFloatRGBAColor>& color_param ) {}

	// Sets one or more float values
	virtual void SetParam( CShaderParameter< std::vector<float> >& float_param ) {}

	// Sets a texture
	virtual void SetParam( CShaderParameter<CTextureParam>& tex_param ) {}

	// Sets a column-major 4x4 matrix
	virtual void SetParam( CShaderParameter<Matrix44>& mat44_param ) {}

	// Sets a single float value
	virtual void SetParam( const char *parameter_name, int int_param ) {}

	// Sets a single float value
	virtual void SetParam( const char *parameter_name, float float_param ) {}

	// Sets a single float3 (Vector3) value
	virtual void SetParam( const char *parameter_name, const Vector3& vec3_param ) {}

	// Sets a color value as 4 floats in RGBA order
	virtual void SetParam( const char *parameter_name, const SFloatRGBAColor& color_param ) {}

	// Sets one or more float values
	virtual void SetParam( const char *parameter_name, const float *float_param, uint num_float_values ) {}

	// Sets one or more float values
	inline void SetParam( const char *parameter_name, const std::vector<float>& float_param );

	// Sets a column-major 4x4 matrix
	virtual void SetParam( const char *parameter_name, const Matrix44& mat44_param ) {}

//	void SetParam( CShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()

	virtual boost::shared_ptr<CShaderLightManager> GetShaderLightManager() { return boost::shared_ptr<CShaderLightManager>(); }

	friend class CShaderManagerHub;
};


//============================ inline implementations ============================

inline void CShaderManager::SetParam( const char *parameter_name, const std::vector<float>& float_param )
{
	if( float_param.empty() )
		return;

	SetParam( parameter_name, &float_param[0], (uint)float_param.size() );
}



#endif  /*  __HLSLShaderManager_HPP__  */
