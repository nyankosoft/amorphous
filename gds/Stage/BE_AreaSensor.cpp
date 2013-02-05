#include "BE_AreaSensor.hpp"


namespace amorphous
{
///#include "../GameTextSystem/GameTextSet.hpp"
///#include "../Script/GameEventTypes.hpp"


CBE_AreaSensor::CBE_AreaSensor()
{
///	this->m_BoundingVolumeType = BVTYPE_AABB;
///	this->m_bNoClip = true;
///	this->m_bNoClipAgainstMap = true;
}


void CBE_AreaSensor::InitCopyEntity( CCopyEntity* pCopyEnt )
{
/*
	// copy entity of CBE_AreaSensor must have min and max of the world aabb in 'v1' and 'v2' 
//	Vector3 vMin = pCopyEnt->v1;
//	Vector3 vMax = pCopyEnt->v2;
//	pCopyEnt->world_aabb.SetMaxAndMin( vMax, vMin );
//	vMin = pCopyEnt->v1 - pCopyEnt->Position();	// change to local coordinate
//	vMax = pCopyEnt->v2 - pCopyEnt->Position();	// change to local coordinate
//	pCopyEnt->local_aabb.SetMaxAndMin( vMax, vMin );

	pCopyEnt->bNoClip = true;

	int& rTimePlayerLeft = pCopyEnt->iExtraDataIndex;
//	rTimePlayerLeft = (int)timeGetTime();
	rTimePlayerLeft = (int)m_pStage->GetElapsedTimeMS();

//	pCopyEnt->SetOrientation( Matrix33Identity() );
*/
}


void CBE_AreaSensor::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{/*
	if( pCopyEnt_Other->pBaseEntity != SinglePlayerInfo().GetCurrentPlayerBaseEntity() )
		return;	// only the player can trigger events


	int& rTimePlayerLeft = pCopyEnt_Self->iExtraDataIndex;
	int iCurrentTime = (int)m_pStage->GetElapsedTimeMS();

	if( iCurrentTime - rTimePlayerLeft < 3000 )
	{	// constantly update 'rdwTimePlayerLeft' while the player is intersecting with this event box
		rTimePlayerLeft = iCurrentTime;
		return;
	}

	// more than 3 seconds have passed since the player left this event box
	rTimePlayerLeft = iCurrentTime;

	// notify the parent entity that the player has been detected by the sensor
	GameMessage message;
	message.iEffect = GM_ENTITY_DETECTED_BY_SENSOR;
	message.pSenderEntity = pCopyEnt_Self;
//	message.s1 = (short)pCopyEnt_Self->s1;	// set the sensor id

	if( pCopyEnt_Self->pParent )
		SendGameMessageTo( message, pCopyEnt_Self->pParent );
	else
		return;	// has no platform/door to send the detection notice to.
*/
}

void CBE_AreaSensor::Serialize( IArchive& ar, const unsigned int version )
{
///	BaseEntity::Serialize( ar, version );

}


} // namespace amorphous
