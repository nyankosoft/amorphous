#include "BE_Turret.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"


CBE_Turret::CBE_Turret()
{
	m_BoundingVolumeType = BVTYPE_AABB;
}


void CBE_Turret::Init()
{
	CBE_Enemy::Init();
	Init3DModel();
}


inline Vector3& DesiredDirection( CCopyEntity* pCopyEnt ) { return pCopyEnt->v1; }
inline Vector3& DesiredDirection_H( CCopyEntity* pCopyEnt ) { return pCopyEnt->v2; }	// horizontal desired direction
inline Vector3& CurrentDirection_H( CCopyEntity* pCopyEnt ) { return pCopyEnt->v3; }	// horizontal current direction


void CBE_Turret::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	CBE_Enemy::InitCopyEntity(pCopyEnt);

	DesiredDeltaYaw(pCopyEnt) = 0;

	FireCycleTime(pCopyEnt) = 0;

	LastFireTime(pCopyEnt) = 0;

	DesiredDirection(pCopyEnt) = pCopyEnt->GetRightDirection();

	DesiredDirection_H(pCopyEnt) = pCopyEnt->GetRightDirection();

	CurrentDirection_H(pCopyEnt) = pCopyEnt->GetRightDirection();
}

/*
void CBE_Turret::Act(CCopyEntity* pCopyEnt)
{
	short& rsMode = pCopyEnt->s1;
	float& rfSensoringInterval	= pCopyEnt->f1;
	Vector3& rvDesiredDirection  = pCopyEnt->v1;

//	Vector3& rvDesiredDirection_H = pCopyEnt->v2;	//horizontal desired direction
//	Vector3& rvCurrentDirection_H = pCopyEnt->v3;	//horizontal current direction
//	float& rfDesiredDeltaYaw	= pCopyEnt->f2;	//ideal angle of yaw rotation (relative) 
//	float& rfDesiredPitch		= pCopyEnt->f3; //ideal angle of pitch rotation (absolute) 
//	float& rfFireCycleTime		= pCopyEnt->f4;
//	float& rfLastFireTime		= pCopyEnt->f5;

	rfSensoringInterval += m_pStage->GetFrameTime();


	if( 0.16f <= rfSensoringInterval )
	{
		rfSensoringInterval = 0;

		// check if the player in a visible position and update 'rvDesiredDirection' and 'rsMode'
		SearchPlayer( pCopyEnt, rsMode, rvDesiredDirection );

		if( rsMode == ES_ATTACK )
			UpdateDesiredYawAndPitch( pCopyEnt, rvDesiredDirection );
	}
	
	if( rsMode == ES_ATTACK )
	{
		AimAlong(pCopyEnt, rvDesiredDirection);
		FireAtPlayer(pCopyEnt);
	}
}*/



bool CBE_Turret::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_Enemy::LoadSpecificPropertiesFromFile(scanner) )
		return true;

	return false;
}


void CBE_Turret::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Enemy::Serialize( ar, version );
}
