#include "BE_IndividualEntity.hpp"
#include "GameMessage.hpp"
#include "CopyEntityDesc.hpp"
#include "CopyEntity.hpp"
#include "EntityRenderManager.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Physics/Actor.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace physics;


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

	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
}


void CBE_IndividualEntity::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	if( m_EntityFlag & BETYPE_USE_ZSORT )
		pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );

	if( m_EntityFlag & BETYPE_ENVMAPTARGET )
	{
		pCopyEnt->RaiseEntityFlags( BETYPE_ENVMAPTARGET );
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

//	pCopyEnt->Draw();
}


void CBE_IndividualEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );
}


/*
bool CBE_IndividualEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	return false;
}
*/
