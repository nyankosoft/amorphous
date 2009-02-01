#include "BE_DirectionalLight.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Stage/LightEntity.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"


CBE_DirectionalLight::CBE_DirectionalLight()
{
	m_bNoClip = true;

	m_DirLight.UpperColor().SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	m_DirLight.LowerColor().SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );

	// direction is set per-entity basis
	// but a default direction is set for safety
	Vec3Normalize( m_DirLight.vDirection, Vector3(-1.0f, -3.0f, -0.5f) );
}


void CBE_DirectionalLight::Init()
{
	CBE_Light::Init();
}


void CBE_DirectionalLight::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;
/*
//	MsgBoxFmt( "created a directional light (index: %d)", light_index );

//	CheckEntitiesInLightRange(pCopyEnt, light_index);
*/
}


/*
void CBE_DirectionalLight::DeleteDynamicLight( CCopyEntity* pCopyEnt )
{
	m_pStage->GetEntitySet()->DeleteLight( (short)pCopyEnt->iExtraDataIndex, CLE_LIGHT_DYNAMIC );
	CheckEntitiesInLightRange( pCopyEnt, (short)pCopyEnt->iExtraDataIndex );
//	pCopyEnt->Terminate();
	m_pStage->TerminateEntity( pCopyEnt );
}*/


void CBE_DirectionalLight::Act( CCopyEntity* pCopyEnt )
{
//	MsgBoxFmt( "CBE_DirectionalLight::Act() (index: %d)", pCopyEnt->iExtraDataIndex );
}

/*
void CBE_DirectionalLight::CheckEntitiesInLightRange( CCopyEntity* pCopyEnt, short light_index )
{
}
*/

/*
void CBE_DirectionalLight::Draw( CCopyEntity* pCopyEnt )
{
}

void CBE_DirectionalLight::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_TERMINATE:
		DeleteDynamicLight( pCopyEnt_Self );
		break;
	}
}
*/


bool CBE_DirectionalLight::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_Light::LoadSpecificPropertiesFromFile( scanner );

	CHemisphericDirectionalLight& light = m_DirLight;
	float r,g,b;

	if( scanner.TryScanLine( "UPPER_COLOR", r, g, b ) )
	{
		m_DirLight.UpperColor().SetRGB( r,g,b );
		return true;
	}

	if( scanner.TryScanLine( "LOWER_COLOR", r, g, b ) )
	{
		m_DirLight.LowerColor().SetRGB( r,g,b );
		return true;
	}

	if( scanner.TryScanLine( "DEFAULT_DIRECTION", m_DirLight.vDirection ) )
	{
		Vec3Normalize( m_DirLight.vDirection, m_DirLight.vDirection );
		return true;
	}

	return false;
}


void CBE_DirectionalLight::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Light::Serialize( ar, version );

	ar & m_DirLight;
}
