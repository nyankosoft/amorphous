#ifndef __GLFixedFunctionPipelineManager_HPP__
#define __GLFixedFunctionPipelineManager_HPP__


#include "../../../base.hpp" // windows.h is included in this header. Need to be included before gl.h on Windows platform
#include "../../../Support/stream_buffer.hpp"

#include <GL/gl.h>
#include "../../fwd.hpp"
#include "../../Shader/ShaderTechniqueHandle.hpp"
#include "../../Shader/ShaderParameter.hpp"
#include "../../Shader/ShaderManager.hpp"
#include "../fwd.hpp"


/**
 - When created, automatically registers itself to ShaderManagerHub

 - Problems:
   - How to change some render states that are specified in the shader when the probrammable shader is used.


*/
class CGLFixedFunctionPipelineManager : public CShaderManager
{
	Matrix44 m_matWorld;
	Matrix44 m_matView;
	Matrix44 m_matProjection;

	boost::shared_ptr<CGLFixedPipelineLightManager> m_pFFPLightManager;

private:

	CGLFixedFunctionPipelineManager();

	bool Init();

public:

	~CGLFixedFunctionPipelineManager();

	bool LoadShaderFromFile( const std::string& filename );

	bool LoadShaderFromText( const stream_buffer& buffer );

	void Release();

	void Reload();

	boost::shared_ptr<CShaderLightManager> GetShaderLightManager();

	inline void SetWorldTransform( const Matrix44& matWorld );

	inline void SetViewTransform( const Matrix44& matView );

	inline void SetProjectionTransform( const Matrix44& matProj );

	inline void SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  );

	inline void SetWorldViewProjectionTransform( const Matrix44& matWorld, const Matrix44& matView, const Matrix44& matProj );


	inline void GetWorldTransform( Matrix44& matWorld ) const;

	inline void GetViewTransform( Matrix44& matView ) const;


	inline void SetViewerPosition( const Vector3& vEyePosition );


	inline Result::Name SetTexture( const int iStage, const CTextureHandle& texture );

//	inline Result::Name SetCubeTexture( const int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture );

	inline Result::Name SetTechnique( const unsigned int id );
	inline Result::Name SetTechnique( CShaderTechniqueHandle& tech_handle );

	void SetParam( CShaderParameter< std::vector<float> >& float_param );

//	void SetParam( CShaderParameter< std::vector<int> >& integer_param );

//	void SetTextureParam()

//	boost::shared_ptr<CShaderLightManager> GetShaderLightManager() { return m_pLightManager; }

	friend CGLFixedFunctionPipelineManager& GLFixedFunctionPipelineManager();
	friend class CGLShaderResource;
};


inline CGLFixedFunctionPipelineManager& GLFixedFunctionPipelineManager()
{
	static CGLFixedFunctionPipelineManager s_obj;
	return s_obj;
}


//================================== inline implementations ==================================


inline void CGLFixedFunctionPipelineManager::SetWorldTransform( const Matrix44& matWorld )
{
	m_matWorld = matWorld;
	Matrix44 matWorldView = m_matView * matWorld;

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( matWorldView.GetData() );
//	glLoadIdentity(); // debug - reset the projection matrix
}


inline void CGLFixedFunctionPipelineManager::SetViewTransform( const Matrix44& matView )
{
	m_matView = matView;
	Matrix44 matWorldView = matView * m_matWorld;

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( matWorldView.GetData() );
//	glLoadIdentity(); // debug - reset the projection matrix

	// debug - check the values of the current matrix
	float mat[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
}


inline void CGLFixedFunctionPipelineManager::SetProjectionTransform( const Matrix44& matProj )
{
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( matProj.GetData() );
//	glLoadIdentity(); // debug - reset the projection matrix

	// debug - check the values of the current matrix
	float mat[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
}


inline void CGLFixedFunctionPipelineManager::SetWorldViewTransform( const Matrix44& matWorld, const Matrix44& matView  )
{
	m_matWorld = matWorld;
	m_matView  = matView;
	Matrix44 matWorldView = matView * matWorld;

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( matWorldView.GetData() );
//	glLoadIdentity(); // debug - reset the projection matrix

	// debug - get the current matrix
	float mat[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
}


inline void CGLFixedFunctionPipelineManager::SetWorldViewProjectionTransform( const Matrix44& matWorld,
															 const Matrix44& matView,
															 const Matrix44& matProj )
{
	m_matView       = matView;
	m_matWorld      = matWorld;
	m_matProjection = matProj;
	Matrix44 matWorldView = matView * matWorld;

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( matWorldView.GetData() );
//	glLoadIdentity(); // debug - reset the projection matrix

	glMatrixMode( GL_PROJECTION );
//	glLoadMatrixf( matProj.GetData() );
	glLoadIdentity(); // debug - reset the projection matrix
}


inline void CGLFixedFunctionPipelineManager::GetWorldTransform( Matrix44& matWorld ) const
{
	matWorld = m_matWorld;
}


inline void CGLFixedFunctionPipelineManager::GetViewTransform( Matrix44& matView ) const
{
	matView = m_matView;
}


inline void CGLFixedFunctionPipelineManager::SetViewerPosition( const Vector3& vEyePosition )
{
//	m_pEffect->SetValue( m_aHandle[HANDLE_VIEWER_POS], &vEyePosition, sizeof(D3DXVECTOR3) );
}

/*
inline HRESULT CGLFixedFunctionPipelineManager::SetTexture( const int iStage, const LPDIRECT3DTEXTURE9 pTexture )
{
	int break_here = 1;
}*/


inline Result::Name CGLFixedFunctionPipelineManager::SetTexture( const int iStage, const CTextureHandle& texture )
{
//	glBindTexture( GL_TEXTURE_2D, texture.GetTextureID() );
	return Result::UNKNOWN_ERROR;
}

/*
inline Result::Name CGLFixedFunctionPipelineManager::SetCubeTexture( int index, const LPDIRECT3DCUBETEXTURE9 pCubeTexture )
{
	if( m_aCubeTextureHandle[index] )
	{
        m_pEffect->SetTexture( m_aCubeTextureHandle[index], pCubeTexture );
		return ???;
	}
	else
		return Result::UNKNOWN_ERROR;
}
*/

inline Result::Name CGLFixedFunctionPipelineManager::SetTechnique( const unsigned int id )
{
//	HRESULT hr = m_pEffect->SetTechnique( m_aTechniqueHandle[id] );

//	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );

	return Result::UNKNOWN_ERROR;
}


inline Result::Name CGLFixedFunctionPipelineManager::SetTechnique( CShaderTechniqueHandle& tech_handle )
{
/*	const int tech_index = tech_handle.GetTequniqueIndex();

	HRESULT hr;
	if( 0 <= tech_index )
	{
		// valid index has already been set to handle
		hr = m_pEffect->SetTechnique( m_aTechniqueHandle[tech_index] );
	}
	else
		hr = SetNewTechnique( tech_handle );

	return ( SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR );*/

	return Result::UNKNOWN_ERROR;
}


#endif  /*  __GLFixedFunctionPipelineManager_HPP__  */
