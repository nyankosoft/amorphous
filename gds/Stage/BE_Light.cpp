#include "BE_Light.hpp"

#include "GameMessage.hpp"
#include "Stage.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"


namespace amorphous
{

using namespace std;

const SFloatRGBAColor CBE_Light::ms_InvalidColor = SFloatRGBAColor(0,0,0,-1000);
const Vector3 CBE_Light::ms_vInvalidDirection = Vector3(0,0,0);
const int CBE_Light::ms_InvalidLightGroup = -1;
const float CBE_Light::ms_fInvalidIntensity = -1000;
const float CBE_Light::ms_fInvalidAttenuation = -1000;


CBE_Light::CBE_Light()
{
	m_TypeFlag = 0;

	m_bNoClip = true;
}


CLightHolder *CBE_Light::GetPooledLight( Light::Type light_type )
{
	switch( light_type )
	{
//	case Light::AMBIENT:
//		m_pLightHolder = shared_ptr<CAmbientLight>( new CAmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
//		break;
	case Light::DIRECTIONAL:
		return m_DirectionalLightPool.get_new_object();
		// new CDirectionalLight( vLightDir, pLightDesc->aColor[0], fLightIntensity, vLightPos );
		break;
	case Light::POINT:
		return m_PointLightPool.get_new_object();
		//( new CPointLight( vLightPos, pLightDesc->aColor[0], fLightIntensity );
		break;
	case Light::SPOTLIGHT:
		return m_SpotlightPool.get_new_object();
		break;
	case Light::HEMISPHERIC_DIRECTIONAL:
		return m_HSDirectionalLightPool.get_new_object();
		//( new CHemisphericDirectionalLight( vLightDir, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity, vLightPos );
		break;
	case Light::HEMISPHERIC_POINT:
		return m_HSPointLightPool.get_new_object();
		//( new CHemisphericPointLight( vLightPos, pLightDesc->aColor[0], pLightDesc->aColor[1], fLightIntensity );
		break;
	case Light::HEMISPHERIC_SPOTLIGHT:
		return m_HSSpotlightPool.get_new_object();
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
//	case Light::AMBIENT:
//		m_pLightHolder = shared_ptr<CAmbientLight>( new CAmbientLight( pLightDesc->aColor[0], fLightIntensity ) );
//		break;
	case Light::DIRECTIONAL:
		m_DirectionalLightPool.release( pLightHolder );
		break;
	case Light::POINT:
		m_PointLightPool.release( pLightHolder );
		break;
	case Light::SPOTLIGHT:
		m_SpotlightPool.release( pLightHolder );
		break;
	case Light::HEMISPHERIC_DIRECTIONAL:
		m_HSDirectionalLightPool.release( pLightHolder );
		break;
	case Light::HEMISPHERIC_POINT:
		m_HSPointLightPool.release( pLightHolder );
		break;
	case Light::HEMISPHERIC_SPOTLIGHT:
		m_HSSpotlightPool.release( pLightHolder );
		break;
	default:
		break;
	}
}


void CBE_Light::Init()
{
	CLightHolderInitializer<PointLight> initializer;
	m_PointLightPool.init( 64, initializer );

	m_DirectionalLightPool.init(   16, CLightHolderInitializer<DirectionalLight>() );
	m_HSDirectionalLightPool.init( 16, CLightHolderInitializer<HemisphericDirectionalLight>() );
	m_HSPointLightPool.init(       64, CLightHolderInitializer<HemisphericPointLight>() );
}


void CBE_Light::InitCopyEntity( CCopyEntity* pCopyEnt )
{
//	CLightEntity *pLightEntity = dynamic_cast<CLightEntity *>(pCopyEnt);

//	pLightEntity->m_pLightHolder = GetPooledLight( pLightEntity->GetLightType() );
}


static inline SFloatRGBAColor ToRGBA( const SFloatRGBColor& src )
{
	return SFloatRGBAColor(src.red,src.green,src.blue,1.0f);
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
//			if( vecFlag[i] == "STATIC" )		m_TypeFlag |= TYPE_STATIC;
//			else if( vecFlag[i] == "DYNAMIC" )	m_TypeFlag |= TYPE_DYNAMIC;
			if( vecFlag[i] == "FADEOUT" )	m_TypeFlag |= TYPE_FADEOUT;
			else if( vecFlag[i] == "TIMER" )	m_TypeFlag |= TYPE_TIMER;
			else if( vecFlag[i] == "GLARE" )	m_TypeFlag |= TYPE_GLARE;
		};

		return true;
	}

	SFloatRGBColor c;
	if( scanner.TryScanLine( "BASE_COLOR",   c ) ) { m_DefaultDesc.aColor[0] = ToRGBA(c); return true; }
	if( scanner.TryScanLine( "BASE_COLOR_0", c ) ) { m_DefaultDesc.aColor[0] = ToRGBA(c); return true; }
	if( scanner.TryScanLine( "BASE_COLOR_1", c ) ) { m_DefaultDesc.aColor[1] = ToRGBA(c); return true; }
	if( scanner.TryScanLine( "BASE_COLOR_2", c ) ) { m_DefaultDesc.aColor[2] = ToRGBA(c); return true; }

	// HS lights
	if( scanner.TryScanLine( "UPPER_COLOR",  c ) ) { m_DefaultDesc.aColor[0] = ToRGBA(c); return true; }
	if( scanner.TryScanLine( "LOWER_COLOR",  c ) ) { m_DefaultDesc.aColor[1] = ToRGBA(c); return true; }

	if( scanner.TryScanLine( "BASE_ATTENU_FACTORS",
		m_DefaultDesc.afAttenuation[0],
		m_DefaultDesc.afAttenuation[1],
		m_DefaultDesc.afAttenuation[2] ) ) return true;

	return false;
}


void CBE_Light::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_TypeFlag;

//	if( ar.GetMode() == IArchive::MODE_INPUT )
//		MsgBoxFmt( "loaded light type for %s: %d", m_strName.c_str(), m_TypeFlag );

	ar & (int&)m_DefaultDesc.LightType;

	for( int i=0; i<numof(m_DefaultDesc.aColor); i++ )
		ar & m_DefaultDesc.aColor[i];

	for( int i=0; i<numof(m_DefaultDesc.afAttenuation); i++ )
		ar & m_DefaultDesc.afAttenuation[i];

	ar & m_DefaultDesc.fIntensity;

	ar & m_DefaultDesc.LightGroup;

	// world pose contains direction for directional light
	ar & m_DefaultDesc.WorldPose;
}


} // namespace amorphous
