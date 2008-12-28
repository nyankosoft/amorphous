#include "BE_DirectionalLight.h"
#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/Camera.h"
#include "3DCommon/Shader/ShaderManager.h"

#include "Stage/LightEntity.h"



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
	short light_type;
	if( m_TypeFlag & TYPE_STATIC )
	{
		// register static light to the stage
		light_type = CLE_LIGHT_STATIC;
	}
	else if( m_TypeFlag & TYPE_DYNAMIC )
	{
		// register dynamic light to the stage
		light_type = CLE_LIGHT_DYNAMIC;
	}
	else
	{
		// neither static or dynamic - invalid
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	// set light properties
	CHemisphericDirectionalLight light = m_DirLight;
	if( pCopyEnt->GetDirection() != Vector3(0,0,0) )
		light.vDirection = pCopyEnt->GetDirection();

//	if( pCopyEnt->v1 != Vector3(0,0,0) )	// if color is not specified
//		memcpy( pCopyEnt->v1, m_afBaseColor, sizeof(float) * 3 );	// use the base color 

	// register light
	CLightEntityManager* pLightManager = m_pStage->GetEntitySet()->GetLightEntityManager();

	int light_index = pLightManager->RegisterHemisphericDirLight( light, light_type );

//	MsgBoxFmt( "created a directional light (index: %d)", light_index );

//	CheckEntitiesInLightRange(pCopyEnt, light_index);

	// save the index. index will be used later to remove the light
	pCopyEnt->iExtraDataIndex = light_index;
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
