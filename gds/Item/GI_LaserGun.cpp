#include "GI_LaserGun.hpp"
#include "WeaponSystem.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Input/InputHandler.hpp"
#include "GameCommon/CriticalDamping.hpp"
#include "Stage/trace.hpp"
#include "Stage/Stage.hpp"
#include "XML/XMLNodeReader.hpp"

#include "Physics/Actor.hpp"

using namespace std;
using namespace physics;


CGI_LaserGun::CGI_LaserGun()
{
	m_fMaxRange = 20.0f;
	m_fRadius = 3.0f;

	m_fPower = 40.0f;
}


CGI_LaserGun::~CGI_LaserGun()
{
}


void CGI_LaserGun::Update( float dt )
{
//	if( !IsWeaponSelected() )
//		return;

	CStageSharedPtr pStage = m_pStage.lock();
	if( !pStage )
		return;

    Vector3 vOwnerMuzzlePos = m_MuzzleEndWorldPose.vPosition;
/*
	CCopyEntity *pTarget = m_Target.GetRawPtr();
	if( pTarget )
	{
		// calc the translation from the center of the target to the muzzle position
		Vector3 vDist = vOwnerMuzzlePos - pTarget->Position();

		float fDistSq = Vec3LengthSq(vDist);
		if( m_fMaxRange * m_fMaxRange < fDistSq )
		{
			m_iHoldingTargetToggle = 0;
//			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_Target.Reset();
			return;
		}

		STrace tr;
		tr.bvType = BVTYPE_DOT;
		tr.vStart = vOwnerMuzzlePos;
		tr.vGoal = pTarget->Position();
		tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

//		CTrace tr;
//		tr.BVType    = BVTYPE_DOT;
//		tr.vStart    = &vOwnerMuzzlePos;
//		tr.vGoal     = &pTarget->Position();
//		tr.TypeFlags = CTrace::FLAG_IGNORE_NOCLIP_ENTITIES;//TRACETYPE_IGNORE_NOCLIP_ENTITIES;

		// check trace
		CStageSharedPtr pStage = m_pStage.lock();
		if( pStage )
			pStage->ClipTrace( tr );

		if( tr.pTouchedEntity != pTarget )
		{
			// found an obstacle between the player and the target object
			// - unable to hold the target any more
			m_iHoldingTargetToggle = 0;
//			m_pTarget->pPhysicsActor->SetAllowFreezing( true );
			m_Target.Reset();
			return;
		}

		if( fDistSq < m_fGraspRange * m_fGraspRange || m_aTriggerState[1] == 1 )
		{
			// the gravity gun is holding the target object

			// account for the target object's size so that it does not bump into the shooter
			float fDist = sqrtf(fDistSq);
			Vector3 vDir = vDist / fDist;		// normalization
			fDist -= ( pTarget->fRadius + 0.2f );
			vDist = vDir * fDist;

			// calc relative velocity
			Vector3 vRVel = pTarget->Velocity() - m_vMuzzleEndVelocity;

			Vector3 vForce;
//			vForce = m_fPosGain * vDist - m_fSpeedGain * vRVel;
//			m_pTarget->ApplyWorldImpulse( vForce, m_pTarget->Position());

			return;
		}
		else
		{
			return;
		}
	}*/
}


bool CGI_LaserGun::HandleInput( int input_code, int input_type, float fParam )
{
	Vector3 vOwnerMuzzlePos = m_MuzzleEndWorldPose.vPosition;

	switch( input_code )
	{
	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[0] = 1;

			// create laser entity
			//
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[0] = 0;

			// tell the laser entity that the user of the laser gun decided to stop the laser
			// - entity will be released shortly after this.
			return true;
		}
		break;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[1] = 1;
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[1] = 0;
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}


void CGI_LaserGun::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Weapon::Serialize( ar, version );

	ar & m_fMaxRange;
	ar & m_fRadius;

	ar & m_fPower;
}


void CGI_LaserGun::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGI_Weapon::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "MaxRange",  m_fMaxRange );
	reader.GetChildElementTextContent( "Radius",    m_fRadius );
	reader.GetChildElementTextContent( "Power",     m_fPower );
}
