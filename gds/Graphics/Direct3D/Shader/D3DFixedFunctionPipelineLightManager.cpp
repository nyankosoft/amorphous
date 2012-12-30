#include "D3DFixedFunctionPipelineLightManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"


namespace amorphous
{


CD3DFixedFunctionPipelineLightManager::CD3DFixedFunctionPipelineLightManager()
:
m_NumLights(0)
{
}


CD3DFixedFunctionPipelineLightManager::~CD3DFixedFunctionPipelineLightManager()
{
}


bool CD3DFixedFunctionPipelineLightManager::Init()
{
	return true;
}


/// Support only the following types of lights
/// - hemispheric directional lights
/// - hemispheric point lights
void CD3DFixedFunctionPipelineLightManager::CommitChanges()
{
	if( 0 < m_NumLights )
		DIRECT3D9.GetDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	else
		DIRECT3D9.GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

/*	m_LightCache.size();
	for( int i=0; i<m_NumLights; i++ )
	{
		DIRECT3D9.GetDevice()->SetLight( i, m_LightCache[i] );
		DIRECT3D9.GetDevice()->LightEnable( i, TRUE );
	}*/

/*	int i;

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
*/
}


void CD3DFixedFunctionPipelineLightManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CD3DFixedFunctionPipelineLightManager::ReleaseGraphicsResources()
{
}


} // namespace amorphous
