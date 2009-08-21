#include "BE_GeneralEntity.hpp"

#include "GameMessage.hpp"
#include "CopyEntityDesc.hpp"
#include "CopyEntity.hpp"
#include "EntityRenderManager.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Serialization_BaseEntityHandle.hpp"

#include "Physics/Actor.hpp"

#include "Support/Log/DefaultLog.hpp"


CBE_GeneralEntity::CBE_GeneralEntity()
{
	m_sGEAttribute = 0;
	m_fLifeTime = 0;
	m_bLighting = true;
	m_bNoClip = false;

//	m_MeshProperty.m_ShaderTechniqueID = SHADER_TECH_MESH_PPL_PNTLIGHT;
	m_MeshProperty.m_ShaderTechnique.resize(1,1);
	m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "PerPixelSinglePointLight" );

}


CBE_GeneralEntity::~CBE_GeneralEntity()
{
	ReleaseGraphicsResources();
}


void CBE_GeneralEntity::Init()
{
	Init3DModel();

	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;

//	LoadBaseEntity( m_SmokeTrace );
	for( size_t i=0; i<m_vecSmokeTrail.size(); i++ )
		LoadBaseEntity( m_vecSmokeTrail[i].BaseEntity );

	LoadBaseEntity( m_Explosion );
	LoadBaseEntity( m_Child );
}


void CBE_GeneralEntity::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	// set the durability of this missile
	pCopyEnt->fLife = m_fLife;

	if( m_EntityFlag & BETYPE_USE_ZSORT )
		pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );

	if( m_EntityFlag & BETYPE_ENVMAPTARGET )
	{
		pCopyEnt->RaiseEntityFlags( BETYPE_ENVMAPTARGET );
		m_pStage->GetEntitySet()->GetRenderManager()->EnableEnvironmentMap();
		m_pStage->GetEntitySet()->GetRenderManager()->AddEnvMapTarget( pCopyEnt );

	}

	if( m_sGEAttribute & GETYPE_LIFETIMER )
	{
		float& rfLifeTimer = pCopyEnt->f3;
		rfLifeTimer = m_fLifeTime;
	}

//	if( 0 < strlen(m_SmokeTrace.GetBaseEntityName()) )
	if( 0 < m_vecSmokeTrail.size() )
		SetSmokeTrace( pCopyEnt );

	if( 0 < strlen(m_Child.GetBaseEntityName()) )
	{
		// dynamic lighting test
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_Child;
		light.SetWorldPose( pCopyEnt->GetWorldPose() );
		light.vVelocity = pCopyEnt->Velocity();
		light.pParent = pCopyEnt;

		m_pStage->CreateEntity( light );
	}

	pCopyEnt->m_MeshHandle = m_MeshProperty.m_MeshObjectHandle;

	if( m_EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS )
		CreateAlphaEntities( pCopyEnt );
}


void CBE_GeneralEntity::SetSmokeTrace(CCopyEntity* pCopyEnt)
{
	// generate a smoke trace entity
	CCopyEntityDesc smoke_trace;

	size_t i, num = m_vecSmokeTrail.size();
	for( i=0; i<num; i++ )
	{
		SmokeTrail& src_trail = m_vecSmokeTrail[i];

		smoke_trace.SetDefault();

//		smoke_trace.pBaseEntityHandle = &m_SmokeTrace;
		smoke_trace.pBaseEntityHandle = &src_trail.BaseEntity;
//		smoke_trace.vPosition = pCopyEnt->GetWorldPosition();
		smoke_trace.SetWorldPosition( pCopyEnt->GetWorldPosition() + src_trail.vLocalPosition );

		Vector3 vPrevPos = pCopyEnt->GetWorldPosition() + src_trail.vLocalPosition;

		Vector3& rvEmitterPrevPos = smoke_trace.v1;
		rvEmitterPrevPos = vPrevPos;

		float& rfLeftTraceLength = smoke_trace.f3;
		rfLeftTraceLength = 0.0f;

		float& rfCurrentTime = smoke_trace.f2;
		rfCurrentTime = 0.0f;

		smoke_trace.pParent = pCopyEnt;

		m_pStage->CreateEntity( smoke_trace );
	}
}


void CBE_GeneralEntity::Act(CCopyEntity* pCopyEnt)
{
	float frametime = m_pStage->GetFrameTime();
	if( m_sGEAttribute & GETYPE_LIFETIMER )
	{
		float& rfLifeTimer = pCopyEnt->f3;
		rfLifeTimer -= frametime;

		if( rfLifeTimer <= 0.0f )
		{
			// lifetime has expired - terminate the entity

			if( 0 < strlen(m_Explosion.GetBaseEntityName()) )
			{
				// create explosion animation
				m_pStage->CreateEntity( m_Explosion, pCopyEnt->GetWorldPosition(), Vector3(0,0,0), pCopyEnt->GetDirection() );
			}

			m_pStage->TerminateEntity( pCopyEnt );
			return;
		}
	}

	if( pCopyEnt->bNoClip
	 && !(pCopyEnt->GetEntityFlags() & BETYPE_RIGIDBODY) )
	{
		// TODO: pose update for collidable enitity
		pCopyEnt->SetWorldPosition( pCopyEnt->GetWorldPosition() + pCopyEnt->Velocity() * frametime );
	}

//	if( 0 < strlen(m_SmokeTrace.GetBaseEntityName()) )
	if( 0 < m_vecSmokeTrail.size() )
	{
		int i, num = (int)m_vecSmokeTrail.size();
		for( i=0; i<num; i++ )
		{
			CCopyEntity* pSmokeTrace = pCopyEnt->GetChild(i);
			if( IsValidEntity( pSmokeTrace ) )
				pSmokeTrace->Act();	// update smoke trace
		}
	}

	if( pCopyEnt->GetEntityFlags() & BETYPE_ENVMAPTARGET )
	{
	}

/**	if( pCopyEnt->GetChild(0) )
		pCopyEnt->GetChild(0)->Act();**/

}


void CBE_GeneralEntity::Draw(CCopyEntity* pCopyEnt)
{
	pCopyEnt->sState |= CESTATE_LIGHT_INFORMATION_INVALID;

	if( m_MeshProperty.m_MeshObjectHandle.IsLoaded()
	 || pCopyEnt->m_MeshHandle.IsLoaded() )
	{
		Draw3DModel(pCopyEnt);
	}
}


void CBE_GeneralEntity::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_DAMAGE:
		if( m_EntityFlag & BETYPE_INDESTRUCTIBLE )
			return;

		float& rfLife = pCopyEnt_Self->fLife;
		rfLife -= rGameMessage.fParam1;
		if( rfLife <= 0 )
		{
			if( 0 < strlen(m_Explosion.GetBaseEntityName()) )
			{
				// create explosion animation
				PrintLog( "creating an explosion entity: " + string(m_Explosion.GetBaseEntityName()) );
				m_pStage->CreateEntity( m_Explosion, pCopyEnt_Self->GetWorldPosition(), Vector3(0,0,0), pCopyEnt_Self->GetDirection() );
			}

			// terminate myself
			m_pStage->TerminateEntity( pCopyEnt_Self );
		}
		break;
	}
}

/*
void CBE_GeneralEntity::ReleaseGraphicsResources()
{
}

void CBE_GeneralEntity::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}
*/

bool CBE_GeneralEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner );

	string explosion, smoke_trace, child_name;
	Vector3 vSmokeEmitPos = Vector3(0,0,0);

	if( scanner.TryScanLine( "LIFE_TIME", m_fLifeTime ) )
	{	
		m_sGEAttribute |= GETYPE_LIFETIMER;
		return true;
	}

	if( scanner.TryScanLine( "EXPLOSION", explosion ) )
	{
		m_Explosion.SetBaseEntityName( explosion.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "SMOKE_TRACE", smoke_trace, vSmokeEmitPos ) )
	{
//		m_SmokeTrace.SetBaseEntityName( smoke_trace.c_str() );
//		m_SmokeTrace.SetBaseEntityName( smoke_trace.c_str() );
		m_vecSmokeTrail.push_back( SmokeTrail() );
		m_vecSmokeTrail.back().BaseEntity.SetBaseEntityName( smoke_trace.c_str() );
		m_vecSmokeTrail.back().vLocalPosition = vSmokeEmitPos;
		vSmokeEmitPos = Vector3(0,0,0);
		return true;
	}

	if( scanner.TryScanLine( "CHILD", child_name ) )
	{
		m_Child.SetBaseEntityName( child_name.c_str() );
		return true;
	}

	return false;
}


void CBE_GeneralEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );

	ar & m_sGEAttribute;
	ar & m_fLifeTime;

//	ar & m_SmokeTrace;
	ar & m_vecSmokeTrail;
	ar & m_Explosion;
	ar & m_Child;
}
