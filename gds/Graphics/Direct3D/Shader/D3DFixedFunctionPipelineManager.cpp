#include "D3DFixedFunctionPipelineManager.hpp"
#include "HLSLShaderLightManager.hpp"
#include "Graphics/Direct3D9.hpp"

#include "Support/Log/DefaultLog.hpp"

using namespace std;


CD3DFixedFunctionPipelineManager::CD3DFixedFunctionPipelineManager()
{
}


CD3DFixedFunctionPipelineManager::~CD3DFixedFunctionPipelineManager()
{
	Release();
}


void CD3DFixedFunctionPipelineManager::Release()
{
}


void CD3DFixedFunctionPipelineManager::Reload()
{
/*	LoadShaderFromFile( m_strFilename );

	// reload techniques
	for( int i=0; i<NUM_MAX_TECHNIQUES; i++ )
	{
		if( 0 < m_astrTechniqueName[i].length() )
			m_aTechniqueHandle[i] = m_pEffect->GetTechniqueByName( m_astrTechniqueName[i].c_str() );
	}

	for( size_t i=0; i<m_vecParamHandle.size(); i++ )
	{
		m_vecParamHandle[i].Handle
			= m_pEffect->GetParameterByName( NULL, m_vecParamHandle[i].ParameterName.c_str() );
	}*/
}


void CD3DFixedFunctionPipelineManager::PrintCompilerErrors( LPD3DXBUFFER pCompileErrors )
{
	if( pCompileErrors )
	{
		char *pBuffer = (char *)pCompileErrors->GetBufferPointer();
		g_Log.Print( WL_ERROR, "CD3DFixedFunctionPipelineManager::LoadShaderFromFile() - %s", pBuffer );
		pCompileErrors->Release();
	}
/*
	if(hr == D3DERR_INVALIDCALL)			return;
	else if(hr == D3DXERR_INVALIDDATA)		return;
	else if(hr == E_OUTOFMEMORY)			return;
	else									return;
*/
	return;
}


bool CD3DFixedFunctionPipelineManager::Init()
{
/*	m_aMatrixHandle[MATRIX_WORLD]           = m_pEffect->GetParameterBySemantic( NULL, "WORLD" );
	m_aMatrixHandle[MATRIX_VIEW]            = m_pEffect->GetParameterBySemantic( NULL, "VIEW" );
	m_aMatrixHandle[MATRIX_PROJ]            = m_pEffect->GetParameterBySemantic( NULL, "PROJ" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW]      = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEW" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ] = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEWPROJ" );

	int i;
	char acStr[256];
	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
	{
		sprintf( acStr, "Texture%d", i );
		m_aTextureHandle[i] = m_pEffect->GetParameterByName( NULL, acStr );
	}

	m_aCubeTextureHandle[0] = m_pEffect->GetParameterByName( NULL, "g_txCubeMap" );

	m_aHandle[HANDLE_VIEWER_POS]    = m_pEffect->GetParameterByName( NULL, "g_vEyePos" );
	m_aHandle[HANDLE_AMBIENT_COLOR] = m_pEffect->GetParameterByName( NULL, "g_vAmbientColor" );

	// create shader light manager
	CHLSLShaderLightManager *pD3DShaderLightMgr = new CHLSLShaderLightManager( m_pEffect );

	pD3DShaderLightMgr->Init();

	m_pLightManager = boost::shared_ptr<CHLSLShaderLightManager>( pD3DShaderLightMgr );

	m_vecParamHandle.reserve( 8 );
*/
	return true;
}


bool CD3DFixedFunctionPipelineManager::LoadShaderFromFile( const string& filename )
{
	return true;
}


bool CD3DFixedFunctionPipelineManager::LoadShaderFromText( const stream_buffer& buffer )
{
	return true;
}


void CD3DFixedFunctionPipelineManager::SetParam( CShaderParameter< std::vector<float> >& float_param )
{
/*	int index = float_param.m_ParameterIndex;
	if( index < 0 )
	{
		// init

		size_t i;
		for( i=0; i<m_vecParamHandle.size(); i++ )
		{
			if( m_vecParamHandle[i].ParameterName == float_param.GetParameterName() )
			{
				float_param.m_ParameterIndex = index = (int)i;
				break;
			}
		}

		if( i == m_vecParamHandle.size() )
		{
			// not found - get the parameter from the name
			D3DXHANDLE param_handle = m_pEffect->GetParameterByName( NULL, float_param.GetParameterName().c_str() );
			if( param_handle )
			{
				float_param.m_ParameterIndex = index = (int)m_vecParamHandle.size();
				m_vecParamHandle.push_back( CD3DShaderParameterHandle(float_param.GetParameterName(),param_handle) );
			}
		}
	}

	HRESULT hr;
	if( 0 <= index && index < (int)m_vecParamHandle.size() )
		hr = m_pEffect->SetFloatArray( m_vecParamHandle[index].Handle, &(float_param.GetParameter()[0]), (UINT)float_param.GetParameter().size() );
*/
}

