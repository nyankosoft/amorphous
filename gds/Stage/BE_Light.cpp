#include "BE_Light.hpp"

#include "GameMessage.hpp"
#include "Stage.hpp"
#include "../Graphics/Camera.hpp"
#include "../Graphics/Shader/ShaderManager.hpp"

#include "Support/Log/DefaultLog.hpp"

using namespace std;


CBE_Light::CBE_Light()
{
	m_TypeFlag = 0;

	m_afBaseColor[0] = m_afBaseColor[1] = m_afBaseColor[2] = 1.0f;

	m_bNoClip = true;
}


CLightHolder *CBE_Light::GetPooledLight( CLight::Type light_type )
{
	switch( light_type )
	{
//	case CLight::AMBIENT:
//		m_pLightHolder = shared_ptr<CAmbientLight>( new CAmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
//		break;
	case CLight::DIRECTIONAL:
		return m_DirectionalLightPool.get_new_object();
		// new CDirectionalLight( vLightDir, pLightDesc->aColor[0], fLightIntensity, vLightPos );
		break;
	case CLight::POINT:
		return m_PointLightPool.get_new_object();
		//( new CPointLight( vLightPos, pLightDesc->aColor[0], fLightIntensity );
		break;
	case CLight::HEMISPHERIC_DIRECTIONAL:
		return m_HSDirectionalLightPool.get_new_object();
		//( new CHemisphericDirectionalLight( vLightDir, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity, vLightPos );
		break;
	case CLight::HEMISPHERIC_POINT:
		return m_HSPointLightPool.get_new_object();
		//( new CHemisphericPointLight( vLightPos, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity );
		break;
	default:
		return NULL;
		break;
	}
}


void CBE_Light::ReleasePooledLight( CLightHolder *pLightHolder )
{
	switch( pLightHolder->pLight->GetLightType() )
	{
//	case CLight::AMBIENT:
//		m_pLightHolder = shared_ptr<CAmbientLight>( new CAmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
//		break;
	case CLight::DIRECTIONAL:
		m_DirectionalLightPool.release( pLightHolder );
		break;
	case CLight::POINT:
		m_PointLightPool.release( pLightHolder );
		break;
	case CLight::HEMISPHERIC_DIRECTIONAL:
		m_HSDirectionalLightPool.release( pLightHolder );
		break;
	case CLight::HEMISPHERIC_POINT:
		m_HSPointLightPool.release( pLightHolder );
		break;
	default:
		break;
	}
}


void CBE_Light::Init()
{
	CLightHolderInitializer<CPointLight> initializer;
	m_PointLightPool.init( 64, initializer );

	m_DirectionalLightPool.init(   16, CLightHolderInitializer<CDirectionalLight>() );
	m_HSDirectionalLightPool.init( 16, CLightHolderInitializer<CHemisphericDirectionalLight>() );
	m_HSPointLightPool.init(       64, CLightHolderInitializer<CHemisphericPointLight>() );
}


void CBE_Light::InitCopyEntity( CCopyEntity* pCopyEnt )
{
//	CLightEntity *pLightEntity = dynamic_cast<CLightEntity *>(pCopyEnt);

//	pLightEntity->m_pLightHolder = GetPooledLight( pLightEntity->GetLightType() );
}


bool CBE_Light::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string light_type;

	if( scanner.TryScanLine( "LIGHT_TYPE", light_type ) )
	{
		// separate the string
		std::vector<std::string> vecFlag;
		SeparateStrings( vecFlag, light_type.c_str(), " |\n");

		size_t i, num_strings = vecFlag.size();
		for( i=0; i<num_strings; i++ )
		{
			if( vecFlag[i] == "STATIC" )		m_TypeFlag |= TYPE_STATIC;
			else if( vecFlag[i] == "DYNAMIC" )	m_TypeFlag |= TYPE_DYNAMIC;
			else if( vecFlag[i] == "FADEOUT" )	m_TypeFlag |= TYPE_FADEOUT;
			else if( vecFlag[i] == "TIMER" )	m_TypeFlag |= TYPE_TIMER;
			else if( vecFlag[i] == "GLARE" )	m_TypeFlag |= TYPE_GLARE;
		};
		
		return true;
	}

	if( scanner.TryScanLine( "BASE_COLOR", m_afBaseColor[0], m_afBaseColor[1], m_afBaseColor[2] ) ) return true;

	return false;
}


void CBE_Light::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_TypeFlag;

//	if( ar.GetMode() == IArchive::MODE_INPUT )
//		MsgBoxFmt( "loaded light type for %s: %d", m_strName.c_str(), m_TypeFlag );

	for( int i=0; i<3; i++ )
        ar & m_afBaseColor[i];
}
