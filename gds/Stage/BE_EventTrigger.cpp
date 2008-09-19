#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "trace.h"
#include "Stage.h"
#include "../Stage/PlayerInfo.h"
#include "../Stage/HUD_PlayerBase.h"

#include "../GameTextSystem/GameTextSet.h"
#include "../GameEvent/GameEventTypes.h"

#include "BE_EventTrigger.h"


CBE_EventTrigger::CBE_EventTrigger()
{
	this->m_BoundingVolumeType = BVTYPE_AABB;
	this->m_bNoClip = true;
	this->m_bNoClipAgainstMap = true;
}


CBE_EventTrigger::~CBE_EventTrigger()
{
}


void CBE_EventTrigger::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	// clients who want to use EventTrigger entity must provide 'v1' and 'v2' with min and max of an aabb in world coordinate
	Vector3 vMin = pCopyEnt->v1;
	Vector3 vMax = pCopyEnt->v2;
	pCopyEnt->world_aabb.SetMaxAndMin( vMax, vMin );
	vMin = pCopyEnt->v1 - pCopyEnt->Position();	// change to local coordinate
	vMax = pCopyEnt->v2 - pCopyEnt->Position();	// change to local coordinate
	pCopyEnt->local_aabb.SetMaxAndMin( vMax, vMin );

	pCopyEnt->bNoClip = true;

	pCopyEnt->SetOrientation( Matrix33Identity() );

	// currently, only text events are supported
//	pCopyEnt->s1 = ET_SHOWTEXTMESSAGE;

}


void CBE_EventTrigger::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
	if( pCopyEnt_Other->pBaseEntity != SinglePlayerInfo().GetCurrentPlayerBaseEntity() )
		return;	// only the player can trigger events

	float& rfTimeSincePlayerLeft = pCopyEnt_Self->f3;
//	float fCurrentTime = (float)( timeGetTime() / 1000 );
	float fCurrentTime = (float)m_pStage->GetElapsedTime();

	if( fCurrentTime - rfTimeSincePlayerLeft < 3.0f )
	{	// constantly update 'rfTimeSincePlayerLeft' while the player is intersecting with this event box
		rfTimeSincePlayerLeft = fCurrentTime;
		return;
	}

	// more than 3 seconds have passed since the player left this event box
	rfTimeSincePlayerLeft = fCurrentTime;

	int i, num;

//	CGameEvent *pEvent = m_pStage->GetEvent( pCopyEnt_Self->iExtraDataIndex );
	CGameEvent *pEvent = NULL;

	if( !pEvent )
		return;

	// check if the text window is being used by other messages
	if( pEvent->GetType() == CGameEvent::TEXT_MESSAGE &&
		SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetHUD()->GetCurrentTextSetInTextWindow() != NULL )
		return;

	// check conditions
	bool bCond = true;
	vector<CGameEventCondition>& rvecCondition = pEvent->GetCondition();
	num = rvecCondition.size();
	for( i=0; i<num; i++ )
	{
		switch( rvecCondition[i].type )
		{
		case CGameEventCondition::HAS_ITEM:
			break;

		case CGameEventCondition::HAS_KEYCODE:
//			if( !SinglePlayerInfo().GetCurrentPlayerBaseEntity()->HasKeycode( rvecCondition[i].strData.c_str() ) )
			if( !SinglePlayerInfo().HasKeycode( rvecCondition[i].strData.c_str() ) )
				bCond = false;
			break;

		case CGameEventCondition::NOT_HAVE_KEYCODE:
			if( SinglePlayerInfo().HasKeycode( rvecCondition[i].strData.c_str() ) )
				bCond = false;
			break;
		}
	}
	if( !bCond )
		return;

	CGE_TextEvent *pTextEvent;
	CGE_SpawnEvent *pSpawnEvent;

	switch( pEvent->GetType() )
	{
	case CGameEvent::TEXT_MESSAGE:
		pTextEvent = (CGE_TextEvent *)pEvent;
//		if( SinglePlayerInfo().GetHUD()->GetCurrentTextSetInTextWindow() != NULL )
//			return;	// currently being used by another text set

		SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetHUD()->OpenTextWindow( &(pTextEvent->GetTextSet()) );
		break;

	case CGameEvent::ENTITY_SPAWN:
		{
			pSpawnEvent = (CGE_SpawnEvent *)pEvent;
			CCopyEntityDesc entity_desc;
			CBaseEntityHandle entity;
			entity_desc.pBaseEntityHandle = &entity;

			vector<CEventEntityDesc>& rvecEntity = pSpawnEvent->GetEntity();
//			rvecEntity = SpawnEvent->GetEntity();
			for( i=0; i<num; i++ )
			{
				entity.SetBaseEntityName( rvecEntity[i].strEntityName.c_str() );
				entity_desc.SetWorldPose( rvecEntity[i].matWorldPose );
				m_pStage->CreateEntity( entity_desc );
			}
		}
		break;
	}

	// decrement the life counter for finite events
	float& rfEventCounter = pCopyEnt_Self->f1;
	if( 0 < rfEventCounter )
	{
		rfEventCounter -= 1.0f;
		if( rfEventCounter <= 0.0f )
		{
			m_pStage->TerminateEntity( pCopyEnt_Self );
		}
	}

}

void CBE_EventTrigger::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

//	ar & 
//	ar & 
//	ar & 
}
