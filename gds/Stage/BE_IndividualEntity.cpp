
#include "BE_IndividualEntity.h"

#include "GameMessage.h"
#include "CopyEntityDesc.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"

#include "JigLib/JL_PhysicsActor.h"

#include "Support/Log/DefaultLog.h"


CBE_IndividualEntity::CBE_IndividualEntity()
{
	m_bLighting = true;
	m_bNoClip = false;
}


CBE_IndividualEntity::~CBE_IndividualEntity()
{
	ReleaseGraphicsResources();
}


void CBE_IndividualEntity::Init()
{
	Init3DModel();

	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
}


void CBE_IndividualEntity::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	if( m_EntityFlag & BETYPE_USE_ZSORT )
		pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;

	if( m_EntityFlag & BETYPE_ENVMAPTARGET )
	{
		pCopyEnt->EntityFlag |= BETYPE_ENVMAPTARGET;
		m_pStage->GetEntitySet()->GetRenderManager()->EnableEnvironmentMap();
		m_pStage->GetEntitySet()->GetRenderManager()->AddEnvMapTarget( pCopyEnt );

	}
}


void CBE_IndividualEntity::Act(CCopyEntity* pCopyEnt)
{
	pCopyEnt->Update( m_pStage->GetFrameTime() );
}


void CBE_IndividualEntity::Draw(CCopyEntity* pCopyEnt)
{
	pCopyEnt->sState |= CESTATE_LIGHT_INFORMATION_INVALID;

	if( m_MeshProperty.m_MeshObjectHandle.IsLoaded() )
	{
		Draw3DModel(pCopyEnt);
	}
}


void CBE_IndividualEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );
}


/*
void CBE_IndividualEntity::ReleaseGraphicsResources()
{
}

void CBE_IndividualEntity::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}
*/

/*
bool CBE_IndividualEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	return false;
}
*/
