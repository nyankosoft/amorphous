#include "GI_MissileLauncher.hpp"
#include "GI_Ammunition.hpp"
#include "WeaponSystem.hpp"

#include "amorphous/3DMath/MathMisc.hpp"
#include "amorphous/3DMath/3DGameMath.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include "amorphous/Input/ForceFeedback/ForceFeedbackEffect.hpp"
#include "amorphous/Sound/SoundManager.hpp"
#include "amorphous/XML/XMLNode.hpp"

#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/CopyEntity.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"
#include "amorphous/Stage/BE_HomingMissile.hpp"
#include "amorphous/Stage/PlayerInfo.hpp"

#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{



CMissileHolder::CMissileHolder()
:
ReleaseLocalPose(Matrix34Identity()),
ReleaseWorldPose(Matrix34Identity()),
pLoadedAmmo(NULL),
LastFireTimeMS(0),
vVelocityAtReleasePos(Vector3(0,0,0))
{
}


void CMissileHolder::InitStates()
{
	pLoadedAmmo = NULL;
	LastFireTimeMS = 0;
	vVelocityAtReleasePos = Vector3(0,0,0);
}


void CMissileHolder::Serialize( IArchive& ar, const unsigned int version )
{
	ar & ReleaseLocalPose;

	if( ar.GetMode() == IArchive::MODE_INPUT )
		InitStates();
}


void CMissileHolder::LoadFromXMLNode( XMLNode& reader )
{
	reader.GetChildElementTextContent( "LocalReleasePos", ReleaseLocalPose.vPosition );

	InitStates();
}



/*
class CMissileEntityTracker : public CCopyEntityCallbackBase
{
public:

	CMissileEntityTracker() {}

	void OnCopyEntityDestroyed( CCopyEntity* pEntity )
	{
	}
}
*/


MissileLauncher::MissileLauncher()
{
	m_TypeFlag |= (TYPE_WEAPON);

	m_fValidSensorAngle	 = 1.5f;
	m_fMaxSensorRange	 = 200000.0f;

//	m_NumMaxSimulTargets = 1;
	SetNumMaxSimultaneousTargets( 1 );

	SetNumReleasePositions( 1 );

	m_CurrentReleasePoseIndex = 0;

	m_FireTargetIndex = 0;

	m_LauncherType = TYPE_LOAD_AND_RELEASE;
//	m_LauncherType = TYPE_FIRE_IMMEDIATE;

	m_fTargetSensoringInterval = 0.12f;

	m_fFrameTimeAccumulation = 0.0f;

	m_pFFEffect = shared_ptr<CForceFeedbackEffect>( new CForceFeedbackEffect );

	CConstantForceFeedbackEffectDesc desc;
	desc.duration  = 100;
	desc.gain      = 100;
	desc.magnitude = 100;

//	CForceFeedbackEffect m_FFEffect;
	m_pFFEffect->Init( desc );
}


void MissileLauncher::InitStates()
{
	// initialize states
	m_CurrentReleasePoseIndex = 0;
	m_FocusedEntity          = EntityHandle<>();
	m_vecCurrentTarget.resize( 0 );
	m_FireTargetIndex         = 0;

	for( size_t i=0; i<m_vecMissileHolder.size(); i++ )
		m_vecMissileHolder[i].InitStates();

	m_fFrameTimeAccumulation = 0.0f;

	m_dLastFireTime          = 0;
}


void MissileLauncher::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Weapon::Serialize( ar, version );

	ar & m_fValidSensorAngle;
	ar & m_fMaxSensorRange;

	ar & m_NumMaxSimulTargets;

	ar & m_NumReleasePositions;

	ar & m_vecMissileHolder;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// initialize states
		InitStates();
	}

	ar & m_LauncherType;

	ar & m_fTargetSensoringInterval;
}


void MissileLauncher::LoadFromXMLNode( XMLNode& reader )
{
	InitStates();

	CGI_Weapon::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "ValidSensorAngle",    m_fValidSensorAngle );
	reader.GetChildElementTextContent( "MaxSensorRange",      m_fMaxSensorRange );

	reader.GetChildElementTextContent( "NumMaxSimulTargets",  m_NumMaxSimulTargets );

//	reader.GetChildNodeTextContent( "NumReleasePositions", m_NumReleasePositions );

	int num_release_positions = 0;
	reader.GetChildElementTextContent( "NumReleasePositions", num_release_positions );
	SetNumReleasePositions( num_release_positions );
/*
	vector<XMLNode> = reader.GetImmediateChildren( "Holder" );
	for(;;)
	{
		child_reader = [i];
		int index = to_int( child_reader.GetAttributeText( "index" ) );

		if( index < 0 )
			continue;

		child_reader.GetChildNodeTextContent( "LocalPos", m_ReleaseLocalPos[index] );
	}

	reader.GetChildNodeTextContent( "LauncherType",        m_LauncherType );

	reader.GetChildNodeTextContent( "TargetSensoringInterval", m_fTargetSensoringInterval );*/
}


void MissileLauncher::SetNumMaxSimultaneousTargets( int num_targets )
{
	m_NumMaxSimulTargets = num_targets;

//	m_vecCurrentTarget.resize( m_NumMaxSimulTargets, NULL );
//	m_ReleaseLocalPose.resize( num_targets );
//	m_ReleaseWorldPose.resize( num_targets );
}


void MissileLauncher::SetLocalReleasePose( int index, const Matrix34& pose )
{
//	if( index < 0 || m_ReleaseLocalPose.size() <= index )
//		return;

	m_vecMissileHolder[index].ReleaseLocalPose = pose;
}


/**
 only collidable(!= noclip) entities are picked up as target candidates
  - changed: no clip entities are ignored
    - smoke trace creates too many false positives when missile is launched

*/
void MissileLauncher::UpdateTargets()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( !pStage )
		return;

	if( !m_pWeaponSlot || !m_pWeaponSlot->pChargedAmmo )
		return;		// stage is not linked / missile is not loaded

	float range_sq = m_pWeaponSlot->pChargedAmmo->GetRangeSq();

	m_vecCurrentTarget.resize( 0 );

	shared_ptr<CCopyEntity> pFocusedEntity = m_FocusedEntity.Get();
	if( pFocusedEntity )
	{
		// lock on the focused entity only if it is in the effective zone of the sensor
		// and in the range of the missile
		Vector3 vToTarget = pFocusedEntity->GetWorldPosition() - m_SensorCamera.GetPosition();
		float dist_to_target_sq = Vec3LengthSq(vToTarget);
		float dp = Vec3Dot( vToTarget / sqrtf(dist_to_target_sq), m_SensorCamera.GetFrontDirection() );

		if( acos(dp) < m_fValidSensorAngle && dist_to_target_sq < range_sq )
            m_vecCurrentTarget.push_back( m_FocusedEntity ); // still locking on it
		else
			m_vecCurrentTarget.push_back( EntityHandle<>() ); // lost it
	}

	if( m_NumMaxSimulTargets <= (int)m_vecCurrentTarget.size() )
		return;	// cannot lock-on any more than one target

	m_SensorCamera.SetFarClip( m_fMaxSensorRange );
	m_SensorCamera.SetFOV( m_fValidSensorAngle );

	// check visible entities
	// For launchers that can lock on only one target at a time,
	// this is almost always unnecessary, since the focused entity
	// is selected by the player
	ViewFrustumTest vf_test;
	vf_test.m_Flag = VFT_IGNORE_NOCLIP_ENTITIES;
	vf_test.SetCamera( &m_SensorCamera );
	vf_test.SetBuffer( m_vecpVisibleEntity );
	vf_test.ClearEntities();	// clear any previous data

	// collect entities that are in the view frustum volume of the sensor camera
	pStage->GetVisibleEntities( vf_test );

	int i, iNumVisibleEntities = vf_test.GetNumVisibleEntities();
	for( i=0;
	     i<iNumVisibleEntities && m_vecCurrentTarget.size() < m_NumMaxSimulTargets;
		 i++ )
	{
		CCopyEntity *pTarget = vf_test.GetEntity(i);
		if( pTarget->bNoClip )
			continue;	// don't target a no-clip entity

		if( pTarget == pFocusedEntity.get() )
			continue;	// already regsitered

		if( !IsValidEntity(pTarget) )
			continue;	// this should not really happen

//		if( pTarget->sGroupID == CE_GROUP_INVALID )
//			continue;	// don't target an entity that has an invalid group ID

		if( pTarget->GroupIndex == m_pWeaponSlot->ProjectileGroup )
			continue;	// don't target an entity(missile) fired from this missile launcher

		// check if the target is in the range of the missile
		Vector3 vToTarget = pTarget->GetWorldPosition() - m_SensorCamera.GetPosition();
		float dist_to_target_sq = Vec3LengthSq(vToTarget);
		if( range_sq < dist_to_target_sq )
			continue;

		m_vecCurrentTarget.push_back( pTarget->Self() );
//		pCopyEnt->pTarget = vf_test.GetEntity(i);

	}
}


bool MissileLauncher::SetPrimaryTarget( EntityHandle<>& target_entity )
{
	if( !target_entity.Get() )
		return false;

	m_FocusedEntity = target_entity;

	return true;
}


void MissileLauncher::UpdateWorldProperties( const Matrix34& rShooterWorldPose,
									    const Vector3& rvShooterVelocity,
										const Vector3& rvShooterAngVelocity )
{
	if( !m_pWeaponSlot )
		return;

	m_MuzzleEndWorldPose = m_pWeaponSlot->WorldPose * m_MuzzleEndLocalPose;

	m_vMuzzleEndVelocity
		= rvShooterVelocity;// + Vec3Cross( rvShooterAngVelocity, m_MuzzleEndWorldPose.vPosition - rShooterWorldPose.vPosition );

	// update the position & orientation of the sensor camera
	m_SensorCamera.SetPose( m_MuzzleEndWorldPose );

	size_t i, num_release_positions = m_NumReleasePositions;
	for( i=0; i<num_release_positions; i++ )
	{
		m_vecMissileHolder[i].ReleaseWorldPose = m_pWeaponSlot->WorldPose * m_vecMissileHolder[i].ReleaseLocalPose;

		m_vecMissileHolder[i].vVelocityAtReleasePos = rvShooterVelocity;
	}
}


static bool SafetyOff() { return true; }


void MissileLauncher::UpdateAmmunitions( CStage *pStage )
{
	// update ammunitions poses and load them if necessary
	// - note that these routines are executed whether the launcher is selected or not
	// - i.e. executed regardless of the result of IsWeaonSelected()
	// - also note that loading an ammo means creating a new entity in the stage

	// update the world poses of the loaded ammos
	size_t i, num_release_positions = m_vecMissileHolder.size();
	for( i=0; i<num_release_positions; i++ )
	{
		if( m_vecMissileHolder[i].pLoadedAmmo )
			m_vecMissileHolder[i].pLoadedAmmo->SetWorldPose( m_vecMissileHolder[i].ReleaseWorldPose );
	}

	const Vector3& rvMuzzleEndPosition	= m_MuzzleEndWorldPose.vPosition;

	// return if no ammo is charged
	if( !m_pWeaponSlot->pChargedAmmo )
		return;

	// reload ammo if certain time has passed since the last ammo was launched
	// from an ammo release position

	CGI_Ammunition& rCurrentAmmo = *(m_pWeaponSlot->pChargedAmmo);

	MeshHandle mesh_handle;
	if( 0 < rCurrentAmmo.GetMeshContainerRootNode().GetNumMeshContainers() )
		mesh_handle = rCurrentAmmo.GetMeshContainerRootNode().GetMeshContainer( 0 )->m_MeshObjectHandle;

	for( i=0; i<num_release_positions; i++ )
	{
		if( !m_vecMissileHolder[i].pLoadedAmmo
		 && 4000 < pStage->GetElapsedTimeMS() - m_vecMissileHolder[i].LastFireTimeMS )
		{
			if( rCurrentAmmo.GetCurrentQuantity() == 0 )
				break;	// no ammo

			CCopyEntityDesc missile_entity;
			CCopyEntity *pMissile = NULL;

			// set pointer to the base entity which serves as the bullet of this weapon
			missile_entity.pBaseEntityHandle = &rCurrentAmmo.GetBaseEntityHandle();

//				m_ReloadPosIndex = ( m_ReloadPosIndex + 1 ) % m_NumReleasePositions;

			missile_entity.SetWorldPose( m_vecMissileHolder[i].ReleaseWorldPose );
			missile_entity.vVelocity = Vector3(0,0,0);

			missile_entity.s1 = CBE_HomingMissile::MS_LOADED;

			// projectile fired by the player is marked as 'CE_GROUP_ID_PLAYERSIDE'
			// this is used when the bullet is a homing missile to differentiate targets
			missile_entity.sGroupID = m_pWeaponSlot->ProjectileGroup;

			// 3D mesh object for the entity
//			missile_entity.MeshObjectHandle = rCurrentAmmo.GetMeshObjectContainer().m_MeshObjectHandle;
			missile_entity.MeshObjectHandle = mesh_handle;

			pMissile = pStage->CreateEntity( missile_entity );

			ONCE( LOG_PRINT( "MissileLauncher::Update() - loaded a missile" ) );

			if( pMissile )
			{
				m_vecMissileHolder[i].pLoadedAmmo = pMissile;
			}
		}
	}
}


/// standard update - fire if both triggers are pulled
void MissileLauncher::Update( float dt )
{
	if( IsWeaponSelected() )
	{
		m_fFrameTimeAccumulation += dt;
		if( m_fTargetSensoringInterval <= m_fFrameTimeAccumulation )
		{
			m_fFrameTimeAccumulation -= m_fTargetSensoringInterval;
			UpdateTargets();
		}
	}

	CStageSharedPtr pStage = m_pStage.lock();
	if( !pStage )
		return;

	if( m_LauncherType == TYPE_FIRE_IMMEDIATE )
	{
		if( IsWeaponSelected() && m_aTriggerState[0] == 1 && SafetyOff() /*m_aTriggerState[1] == 1*/ )
		{
			if( 0 < m_iCurrentBurstCount && m_iCurrentBurstCount < m_iNumBursts )
			{
				Fire();
			}
		}
	}
	else if( m_LauncherType == TYPE_LOAD_AND_RELEASE )
	{
		UpdateAmmunitions( pStage.get() );
	}
	else
		ONCE( LOG_PRINT( "MissileLauncher::Update() - invalid launcher type" ) );

}


/// releases the ammo to fire a missile / drop a bomb
/// \attention does not release any memory.
bool MissileLauncher::ReleaseAmmo()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( !pStage )
		return false;

	ONCE( LOG_PRINT( "MissileLauncher::ReleaseAmmo() - releasing the ammo" ) );

	const size_t num_release_positions = m_vecMissileHolder.size();
	for( size_t i=0; i<num_release_positions; i++ )
	{
		CCopyEntity *pAmmoEntity = m_vecMissileHolder[i].pLoadedAmmo;
		if( IsValidEntity(pAmmoEntity) )
		{
			// ready for release

			pAmmoEntity->SetVelocity( m_vecMissileHolder[i].vVelocityAtReleasePos );

			// ignite the booster and set the target if it's a missile
			if( pAmmoEntity->pBaseEntity->GetArchiveObjectID() == BaseEntity::BE_HOMINGMISSILE )
			{
				MissileState(pAmmoEntity) = CBE_HomingMissile::MS_WAITING_IGNITION;
				MissileIgnitionTimer(pAmmoEntity) = 0.0f;
				SetTargetForMissile( pAmmoEntity );
			}

			// release the ammo
			m_vecMissileHolder[i].pLoadedAmmo = NULL;

			m_vecMissileHolder[i].LastFireTimeMS = pStage->GetElapsedTimeMS();

			CGI_Ammunition& rCurrentAmmo = *(m_pWeaponSlot->pChargedAmmo);
			rCurrentAmmo.ReduceQuantity( 1 );

			return true;
		}
	}

	return false;
}


bool MissileLauncher::HandleInput( int input_code, int input_type, float fParam )
{
	switch( input_code )
	{
	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[0] = 1;

			// the first shot is fired by trigger pull event
			if( SafetyOff()/*m_aTriggerState[1] == 1*/
				&& m_iCurrentBurstCount == 0 )
			{
				if( m_LauncherType == TYPE_FIRE_IMMEDIATE )
					Fire();
				else if( m_LauncherType == TYPE_LOAD_AND_RELEASE )
					ReleaseAmmo();

				return true;
			}
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[0] = 0;
			m_iCurrentBurstCount = 0;
			return true;
		}
		break;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[1] = 1;
			return true;
		}
		else if( input_type == ITYPE_KEY_RELEASED )
		{
			m_aTriggerState[1] = 0;
			return true;
		}
		break;

//	case GIC_MOUSE_M:
//		break;
	}

	return false;
}


inline void MissileLauncher::SetTargetForMissile( CCopyEntity *pMissileEntity )
{
	if( m_FireTargetIndex < m_vecCurrentTarget.size()
	 && IsValidEntity(m_vecCurrentTarget[m_FireTargetIndex].GetRawPtr()) )
	{
		GameMessage msg;
		msg.effect = GM_SET_TARGET;
//		msg.pUserData = m_vecCurrentTarget[i];
        msg.pUserData = m_vecCurrentTarget[m_FireTargetIndex].GetRawPtr();

		SendGameMessageTo( msg, pMissileEntity );
	}
}


void MissileLauncher::ApplyForceFeedback()
{
	m_pFFEffect->Start( 1, 0 );

//	CForceFeedbackTargetDevice fft;
//	fft.m_Type = InputDevice::TYPE_GAMEPAD;

//	m_FFEffect.Init( desc, group, fft );
}


void MissileLauncher::Fire()
{
	CStageSharedPtr pStage = m_pStage.lock();
	if( !pStage )
		return;

	if( !m_pWeaponSlot )
		return;

	if( !m_pWeaponSlot->pChargedAmmo )
		return;		// ammo is not loaded

	CGI_Ammunition& rCurrentAmmo = *(m_pWeaponSlot->pChargedAmmo);

	if( rCurrentAmmo.GetCurrentQuantity() == 0 )
		return;	// no ammo

	// TODO: use a large integer for current time
	double dCurrentTime = pStage->GetElapsedTime();
	double dTimeSinceLastFire = dCurrentTime - m_dLastFireTime;

	// return if enough time has not elapsed since the last fire
	if( dTimeSinceLastFire < (double)m_fFireInterval )
		return;

	m_dLastFireTime = dCurrentTime;
//	m_dLastFireTime += m_fFireInterval;

	// save burst shot count
	m_iCurrentBurstCount++;


	// --------------------------------------------------------------------------------


	// ------------------ new fire ------------------

	const Vector3& rvMuzzleEndPosition	= m_MuzzleEndWorldPose.vPosition;
	const Matrix33& matMuzzleOrient		= m_MuzzleEndWorldPose.matOrient;
	const Vector3& rvMuzzleDir_Right	= matMuzzleOrient.GetColumn(0);
	const Vector3& rvMuzzleDir_Up		= matMuzzleOrient.GetColumn(1);
	const Vector3& rvMuzzleDirection	= matMuzzleOrient.GetColumn(2);

	// play sound for the shot
//	pStage->PlaySound3D( m_FireSound, rvMuzzleEndPosition );
	GetSoundManager().PlayAt( m_FireSound, rvMuzzleEndPosition );

	if( m_vecCurrentTarget.size() <= m_FireTargetIndex )
		m_FireTargetIndex = 0;

	Vector3 vFireDirection;
	Vector3 vFireDir_Right, vUp = Vector3(0,1,0);
	CCopyEntityDesc missile_entity;

	missile_entity.SetWorldPosition( rvMuzzleEndPosition );

	CCopyEntity *pMissile = NULL;
	size_t i, num_current_targets = m_vecCurrentTarget.size();
//	for( i=0; i<num_current_targets; i++ )
	for( i=0; i<1; i++ )
	{
		// set pointer to the base entity which serves as the bullet of this weapon
		missile_entity.pBaseEntityHandle = &rCurrentAmmo.GetBaseEntityHandle();

/*		vFireDirection = rvMuzzleDirection
					   + rvMuzzleDir_Right * m_fGrouping / 10.0f * RangedRand( -0.5f, 0.5f )
					   + rvMuzzleDir_Up    * m_fGrouping / 10.0f * RangedRand( -0.5f, 0.5f );
*/
		float x,y;
		GaussianRand( x, y );
		x /= 2.718f;
		y /= 2.718f;
		Limit( x, -1.0f, 1.0f );
		Limit( y, -1.0f, 1.0f );
		vFireDirection = rvMuzzleDirection * 10.0f;
//					   + rvMuzzleDir_Right * m_fGrouping * x
//					   + rvMuzzleDir_Up    * m_fGrouping * y;

		Vec3Normalize( vFireDirection, vFireDirection );
		missile_entity.SetWorldOrient( CreateOrientFromFwdDir(vFireDirection) );

		missile_entity.vVelocity
			= vFireDirection * rCurrentAmmo.GetMuzzleSpeed() * m_fMuzzleSpeedFactor
			+ m_vMuzzleEndVelocity;

		// projectile fired by the player is marked as 'CE_GROUP_ID_PLAYERSIDE'
		// this is used when the bullet is a homing missile to differentiate targets
		missile_entity.sGroupID = m_pWeaponSlot->ProjectileGroup;

		pMissile = pStage->CreateEntity( missile_entity );

		if( !IsValidEntity(pMissile) )
			continue;

		// provide target info
		SetTargetForMissile( pMissile );

		if( pMissile->pBaseEntity->GetArchiveObjectID() == BaseEntity::BE_HOMINGMISSILE )
		{
			MissileState(pMissile) = CBE_HomingMissile::MS_IGNITED;
		}

		// create muzzle flash
		CCopyEntityDesc& rMuzzleFlashDesc = missile_entity;	// reuse the desc object 
		rMuzzleFlashDesc.pBaseEntityHandle = &rCurrentAmmo.GetMuzzleFlashHandle();
		rMuzzleFlashDesc.vVelocity = m_vMuzzleEndVelocity * 0.8f; // Vector3(0,0,0);
		pStage->CreateEntity( rMuzzleFlashDesc );
	}

	rCurrentAmmo.ReduceQuantity( 1 );

	m_FireTargetIndex++;

	// recoil effect
	GameMessage msg;
	msg.effect = GM_IMPACT;
	msg.vParam = - vFireDirection * 0.0f/*m_RecoilForce*/;

/*	Vector3 vImpact = m_vLocalRecoilForce;
	msg.vParam = vImpact.z * rvMuzzleDirection
		       + vImpact.y * rvMuzzleDir_Up
			   + vImpact.x * rvMuzzleDir_Right;
*/

	SendGameMessageTo( msg, SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity() );

	// Need to call in UpdateAmmunitions()
//	ApplyForceFeedback();

//	if( m_pOwnerEntity )
//		SendGameMessageTo( msg, m_pOwnerEntity );

}


bool MissileLauncher::IsLockingOn( CCopyEntity *pEntity )
{
	size_t i, num_current_targets = m_vecCurrentTarget.size();
	for( i=0; i<num_current_targets; i++ )
	{
		if( m_vecCurrentTarget[i].GetRawPtr() == pEntity )
			return true;
	}
	return false;
}


bool MissileLauncher::IsLockingOn( U32 entity_id )
{
	size_t i, num_current_targets = m_vecCurrentTarget.size();
	for( i=0; i<num_current_targets; i++ )
	{
		shared_ptr<CCopyEntity> pEntity = m_vecCurrentTarget[i].Get();
		if( pEntity && pEntity->GetID() == entity_id )
		{
			return true;
		}
	}
	return false;
}


void MissileLauncher::Disarm()
{
	size_t i, num_release_positions = m_vecMissileHolder.size();
	for( i=0; i<num_release_positions; i++ )
	{
		m_vecMissileHolder[i].pLoadedAmmo = NULL;
	}
}


} // namespace amorphous
