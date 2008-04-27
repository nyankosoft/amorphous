
#include "ShaderManager.h"

#include "ShaderManagerHub.h"

#include "3DCommon/Direct3D9.h"
#include "Support/msgbox.h"
#include "Support/Log/DefaultLog.h"

using namespace std;


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


bool CShaderManager::LoadShaderFromFile( const string& filename )
{
	Release();

	m_strFilename = filename;

	HRESULT hr;
	LPD3DXBUFFER pCompileErrors;
	hr = D3DXCreateEffectFromFile( DIRECT3D9.GetDevice(), filename.c_str(), NULL, NULL, 0, 
                                        NULL, &m_pEffect, &pCompileErrors );

	if( FAILED(hr) )
	{
//		MsgBoxFmt( "cannot create effect object from the HLSL effect file: %s", filename.c_str() );
		g_Log.Print( WL_ERROR,
			"CShaderManager::LoadShaderFromFile() - cannot create effect object from the HLSL effect file: %s", filename.c_str() );

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

	m_aMatrixHandle[MATRIX_WORLD] = m_pEffect->GetParameterBySemantic( NULL, "WORLD" );
	m_aMatrixHandle[MATRIX_VIEW] = m_pEffect->GetParameterBySemantic( NULL, "VIEW" );
	m_aMatrixHandle[MATRIX_PROJ] = m_pEffect->GetParameterBySemantic( NULL, "PROJ" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW] = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEW" );
	m_aMatrixHandle[MATRIX_WORLD_VIEW_PROJ] = m_pEffect->GetParameterBySemantic( NULL, "WORLDVIEWPROJ" );

	int i;
	char acStr[256];
	for( i=0; i<NUM_TEXTURE_STAGES; i++ )
	{
		sprintf( acStr, "Texture%d", i );
		m_aTextureHandle[i] = m_pEffect->GetParameterByName( NULL, acStr );
	}

	m_aCubeTextureHandle[0] = m_pEffect->GetParameterByName( NULL, "g_txCubeMap" );

	m_aHandle[HANDLE_VIEWER_POS] = m_pEffect->GetParameterByName( NULL, "g_vEyePos" );
	m_aHandle[HANDLE_AMBIENT_COLOR] = m_pEffect->GetParameterByName( NULL, "g_vAmbientColor" );

	return true;
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
		g_Log.Print( "CShaderManager::SetNewTechnique() - registering a new technique: %s",
			tech_handle.GetTechniqueName() );
		HRESULT hr = m_pEffect->SetTechnique( tech_handle.GetTechniqueName() );
		if( FAILED(hr) )
		{
			// mark the handle as invalid
			g_Log.Print( WL_ERROR, "CShaderManager::SetNewTechnique() - invalid technique: %s",
				tech_handle.GetTechniqueName() );
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

