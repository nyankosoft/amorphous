#include "HLSLShaderLightManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Log/DefaultLog.hpp"


CHLSLShaderLightManager::CHLSLShaderLightManager()
:
m_pEffect(NULL)
{
	memset( m_aPropertyHandle, 0, sizeof(m_aPropertyHandle) );
	memset( m_aHandle,         0, sizeof(m_aHandle) );
}


CHLSLShaderLightManager::CHLSLShaderLightManager( LPD3DXEFFECT pEffect )
:
m_pEffect(pEffect)
{
	memset( m_aPropertyHandle, 0, sizeof(m_aPropertyHandle) );
	memset( m_aHandle,         0, sizeof(m_aHandle) );
}


CHLSLShaderLightManager::~CHLSLShaderLightManager()
{
}


bool CHLSLShaderLightManager::Init()
{
	return SetShaderHandles();
}


bool CHLSLShaderLightManager::SetShaderHandles()
{
	if( !m_pEffect )
	{
		LOG_PRINT_ERROR( " m_pEffect is NULL." );
		return false;
	}

	m_aPropertyHandle[LPH_NUM_DIRECTIONAL_LIGHTS]	= m_pEffect->GetParameterByName( NULL, "iLightDirNum" );
	m_aPropertyHandle[LPH_DIRECTIONAL_LIGHT_OFFSET]	= m_pEffect->GetParameterByName( NULL, "iLightDirIni" );
	m_aPropertyHandle[LPH_NUM_POINT_LIGHTS]			= m_pEffect->GetParameterByName( NULL, "iLightPointNum" );
	m_aPropertyHandle[LPH_POINT_LIGHT_OFFSET]		= m_pEffect->GetParameterByName( NULL, "iLightPointIni" );

	int i;
	for( i=0; i<NUM_LIGHTING_PROPERTIES; i++ )
	{
//		assert( m_aPropertyHandle[i] != 0 );
		if( m_aPropertyHandle[i] == 0 )
			return false;
	}

	// test
	D3DXHANDLE handle = m_pEffect->GetParameterByName( NULL, "g_vEyePos" );

	D3DXHANDLE light0 = NULL, light1 = NULL;
	light0 = m_pEffect->GetParameterByName( NULL, "g_aLight[0]" );
	light1 = m_pEffect->GetParameterByName( NULL, "g_aLight[1]" );

	D3DXHANDLE hLight = NULL;

	char acLight[256];
	char acStr[256];
//	int i;
	for( i=0; i<NUM_MAX_LIGHTS; i++ )
	{
		sprintf( acLight, "g_aLight[%d]", i );
		hLight = m_pEffect->GetParameterByName( NULL, acLight );

		sprintf( acStr, "vPos" );
		m_aHandle[i][LIGHT_POSITION] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vDir" );
		m_aHandle[i][LIGHT_DIRECTION] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vAmbient" );
		m_aHandle[i][LIGHT_AMBIENT_COLOR] = m_pEffect->GetParameterByName( hLight, acStr );

//		sprintf( acStr, "vDiffuse" );
		sprintf( acStr, "vDiffuseUpper" );
		m_aHandle[i][LIGHT_DIFFUSE_COLOR] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "fRange" );
		m_aHandle[i][LIGHT_RANGE] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vAttenuation" );
		m_aHandle[i][LIGHT_ATTENUATION] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vAttenuation[0]" );
		m_aHandle[i][LIGHT_ATTENUATION0] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vAttenuation[1]" );
		m_aHandle[i][LIGHT_ATTENUATION1] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vAttenuation[2]" );
		m_aHandle[i][LIGHT_ATTENUATION2] = m_pEffect->GetParameterByName( hLight, acStr );


		sprintf( acStr, "vDiffuseUpper" );
		m_aHandle[i][LIGHT_UPPER_DIFFUSE_COLOR] = m_pEffect->GetParameterByName( hLight, acStr );

		sprintf( acStr, "vDiffuseLower" );
		m_aHandle[i][LIGHT_LOWER_DIFFUSE_COLOR] = m_pEffect->GetParameterByName( hLight, acStr );

		if( !m_aHandle[i][LIGHT_DIFFUSE_COLOR] )
		{
			// use the upper hemisphere color if it is available
			m_aHandle[i][LIGHT_DIFFUSE_COLOR] = m_aHandle[i][LIGHT_UPPER_DIFFUSE_COLOR];
		}


//		sprintf( acStr, "g_aLight[%d].vPos", i );
//		m_aHandle[i][LIGHT_POSITION] = m_pEffect->GetParameterByName( NULL, acStr );
	}

	return true;
}


/// Support only the following types of lights
/// - hemispheric directional lights
/// - hemispheric point lights
void CHLSLShaderLightManager::CommitChanges()
{
	int i;

	const int num_directional_lights = (int)m_LightCache.vecHSDirecitonalLight.size();
	const int num_point_lights       = (int)m_LightCache.vecHSPointLight.size();

	SetDirectionalLightOffset( 0 );
	SetNumDirectionalLights( num_directional_lights );

	for( i=0; i<num_directional_lights; i++ )
	{
		SetLight( i, m_LightCache.vecHSDirecitonalLight[i] );
	}

	SetPointLightOffset( num_directional_lights );
	SetNumPointLights( num_point_lights );

	for( i=0; i<num_point_lights; i++ )
	{
		SetLight( num_directional_lights + i, m_LightCache.vecHSPointLight[i] );
	}

	m_pEffect->CommitChanges();
}


void CHLSLShaderLightManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	SetShaderHandles();
}


void CHLSLShaderLightManager::ReleaseGraphicsResources()
{
}
