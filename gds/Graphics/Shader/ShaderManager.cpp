#include "ShaderManager.h"
#include "ShaderManagerHub.h"
#include "HLSLShaderLightManager.h"
#include "Graphics/Direct3D9.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


class CD3DXInclude : public ID3DXInclude
{
public:

	HRESULT CALLBACK Open(
		D3DXINCLUDE_TYPE IncludeType,
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID * ppData,
		UINT * pBytes
		);

	HRESULT CALLBACK Close( LPCVOID pData );
};


HRESULT CD3DXInclude::Open(
							D3DXINCLUDE_TYPE IncludeType,
							LPCSTR pFileName,
							LPCVOID pParentData,
							LPCVOID * ppData,
							UINT * pBytes
							)
{
	return S_OK;
}


HRESULT CD3DXInclude::Close( LPCVOID pData )
{
	return S_OK;
}



CShaderManager::CShaderManager()
:
m_pEffect(NULL)
{
	int i;
	for( i=0; i<NUM_HANDLES; i++ )
		m_aHandle[i] = NULL;

	for( i=0; i<NUM_MAX_TECHNIQUES; i++ )
		m_aTechniqueHandle[i] = NULL;

	for( i=0; i<NUM_MATRIX_HANDLES; i++ )
		m_aMatrixHandle[i] = NULL;

	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
		m_aTextureHandle[i] = NULL;

	for( i=0; i<NUM_MAX_CUBETEXTURES; i++ )
		m_aCubeTextureHandle[i] = NULL;

	m_VacantTechniqueEntryIndex = 0;

	// register the instance to the shader manager hub
	ShaderManagerHub.RegisterShaderManager( this );
}


CShaderManager::~CShaderManager()
{
	// release from the list in shader manager hub
	ShaderManagerHub.ReleaseShaderManager( this );

	Release();
}


void CShaderManager::Release()
{
	SAFE_RELEASE(m_pEffect);
}


void CShaderManager::Reload()
{
	LoadShaderFromFile( m_strFilename );

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
	}
}


bool CShaderManager::LoadShaderFromFile( const string& filename )
{
	Release();

	m_strFilename = filename;

	CD3DXInclude d3dx_include;
	ID3DXInclude *pD3DXInclude = NULL; //&d3dx_include

	HRESULT hr;	
	LPD3DXBUFFER pCompileErrors;
	hr = D3DXCreateEffectFromFile( DIRECT3D9.GetDevice(), filename.c_str(), NULL, pD3DXInclude, 0, 
                                        NULL, &m_pEffect, &pCompileErrors );

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( " - Cannot create an effect object from the HLSL effect file: " + filename );

		if( pCompileErrors )
		{
			char *pBuffer = (char *)pCompileErrors->GetBufferPointer();
			g_Log.Print( WL_ERROR, "CShaderManager::LoadShaderFromFile() - %s", pBuffer );
			pCompileErrors->Release();
		}

		if(hr == D3DERR_INVALIDCALL)			return false;
		else if(hr == D3DXERR_INVALIDDATA)		return false;
		else if(hr == E_OUTOFMEMORY)			return false;
		else									return false;

		return false;
	}

	m_aMatrixHandle[MATRIX_WORLD]           = m_pEffect->GetParameterBySemantic( NULL, "WORLD" );
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

	return true;
}


void CShaderManager::SetParam( CShaderParameter< std::vector<float> >& float_param )
{
	int index = float_param.m_ParameterIndex;
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
}


void CShaderManager::UpdateVacantTechniqueIndex()
{
	int i, start = m_VacantTechniqueEntryIndex;
	for( i=start; i<NUM_MAX_TECHNIQUES; i++ )
	{
		if( !m_aTechniqueHandle[i] )
		{
			m_VacantTechniqueEntryIndex = i;
			return;	// found an empty slot
		}
	}

	m_VacantTechniqueEntryIndex = NUM_MAX_TECHNIQUES;
}


bool CShaderManager::RegisterTechnique( const unsigned int id, const char *pcTechnique )
{
	if( id < 0 || NUM_MAX_TECHNIQUES <= id )
		return false;

	D3DXHANDLE handle =  m_pEffect->GetTechniqueByName( pcTechnique );

	if( !handle )
		return false;

	m_aTechniqueHandle[id] = handle;
	m_astrTechniqueName[id] = pcTechnique;

	UpdateVacantTechniqueIndex();

	return true;
}


/// called if SetTechnique() is called with an unregistered technique handle
/// Through the checks in SetTechnique(), the value of tech_index is known to be
/// either CShaderTechniqueHandle::UNINITIALIZED
/// or CShaderTechniqueHandle::INVALID_INDEX
HRESULT CShaderManager::SetNewTechnique( CShaderTechniqueHandle& tech_handle )
{
	const int tech_index = tech_handle.GetTequniqueIndex();

	if( tech_index == CShaderTechniqueHandle::UNINITIALIZED )
	{
		if( tech_handle.GetTechniqueName() == NULL )
		{
			tech_handle.SetTequniqueIndex( CShaderTechniqueHandle::INVALID_INDEX );
			return E_FAIL;
		}

		int num_techniques = NUM_MAX_TECHNIQUES;
		for( int i=0; i<num_techniques; i++ )
		{
			if( m_astrTechniqueName[i] == tech_handle.GetTechniqueName() )
			{
				// cache index to avoid search in subsequent calls
				tech_handle.SetTequniqueIndex( i );

				return m_pEffect->SetTechnique( m_aTechniqueHandle[i] );
			}
		}

		// the requested technique has not been registered to the table yet
		// - set it as a current techqniue and register it to the table
		LOG_PRINT( " - Registering a new technique: " + string(tech_handle.GetTechniqueName()) );
		HRESULT hr = m_pEffect->SetTechnique( tech_handle.GetTechniqueName() );
		if( FAILED(hr) )
		{
			// mark the handle as invalid
			LOG_PRINT_ERROR( " - An invalid technique: " + string(tech_handle.GetTechniqueName()) );
			tech_handle.SetTequniqueIndex( CShaderTechniqueHandle::INVALID_INDEX );
			return E_FAIL;
		}
		else if( m_VacantTechniqueEntryIndex < NUM_MAX_TECHNIQUES )
		{
			// register the technique to the table
			D3DXHANDLE handle =  m_pEffect->GetTechniqueByName( tech_handle.GetTechniqueName() );
			if( handle )
			{
				m_aTechniqueHandle[m_VacantTechniqueEntryIndex] = handle;
				m_astrTechniqueName[m_VacantTechniqueEntryIndex] = tech_handle.GetTechniqueName();
				UpdateVacantTechniqueIndex();
				return S_OK;
			}
			else
			{
				tech_handle.SetTequniqueIndex( CShaderTechniqueHandle::INVALID_INDEX );
				return E_FAIL;
			}
		}
	}
	else //( tech_index == CShaderTechniqueHandle::INVALID_INDEX )
	{
		return E_FAIL;
	}

	return E_FAIL;
}

