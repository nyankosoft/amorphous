#include "Firearm.hpp"
#include "WeaponSystem.hpp"
#include "Cartridge.hpp"
#include "Magazine.hpp"

#include "3DMath/MathMisc.hpp"
#include "Graphics/3DGameMath.hpp"
#include "GameCommon/3DActionCode.hpp"
#include "Support/MTRand.hpp"
#include "Sound/SoundManager.hpp"
#include "XML/XMLNodeReader.hpp"
#include "Input/InputHandler.hpp"

#include "Stage/Stage.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/PlayerInfo.hpp"

using std::vector;
using boost::shared_ptr;


//======================================================================================
// CFirearm
//======================================================================================

CFirearm::CFirearm()
:
m_PrimaryCaliber( Caliber::OTHER ),
m_StandardMagazineCapacity( 1 ),
m_fGrouping( 0.0f ),
m_FirearmState( FS_SLIDE_FORWARD ),
m_IsSlideHeld(false),
m_IsSlideStopEngaged(false),
m_fSlidePosition(0.0f),
m_fSlideStrokeDistance(0.5f),
m_vLocalHammerPivot( Vector3(0,0,0) ),
m_NumTotalFiredRounds(0)
{
	m_TypeFlag |= (TYPE_WEAPON);

/*
//	m_fBurstInterval = 1.0f;

	m_MuzzleEndLocalPose.Identity();
	m_MuzzleEndLocalPose.vPosition = Vector3(0,0,10);
	m_MuzzleEndWorldPose = m_MuzzleEndLocalPose;

	m_vMuzzleEndVelocity = Vector3(0,0,0);

	m_pOwnerEntity = NULL;*/
}


void CFirearm::Update( float dt )
{
	shared_ptr<CItemEntity> pEntity = GetItemEntity().Get();
	if( !pEntity )
		return;

	CStage *pStage = pEntity->GetStage();
	if( !pStage )
		return;

	UpdateFirearmState( *pStage );
}


bool CFirearm::IsSlideAtItsMostRearwardPosition( float error ) const
{
	return ( fabs( (-m_fSlidePosition) - m_fSlideStrokeDistance ) < error );
}


void CFirearm::UpdateFirearmState( CStage& stage )
{
	const float dt = 0.02f;

	bool bIsSlideMovingBackwardAfterFiring = false;
	if( bIsSlideMovingBackwardAfterFiring )
	{
		float slide_bkwd_speed = 25.0f;
		float d = slide_bkwd_speed * dt;
		m_fSlidePosition = get_clamped( m_fSlidePosition - d, -m_fSlideStrokeDistance, 0.0f );

		if( IsSlideAtItsMostRearwardPosition() )
			bIsSlideMovingBackwardAfterFiring = false;

		if( m_pMagazine && m_pMagazine->IsEmpty() )
			m_IsSlideStopEngaged = true;
	}
	else
	{
		UpdateSlideMotionFromRearwardToForward( stage );
	}
}


void CFirearm::UpdateSlideMotionFromRearwardToForward( CStage& stage )
{
	const float dt = 0.02f;

	// Update the slide position
	if( m_IsSlideHeld )
	{
		// Slide is held at its current position by the user
		// - Do nothing.
	}
	else if( m_IsSlideStopEngaged )
	{
		if( IsSlideAtItsMostRearwardPosition() )
		{
			// The slide is held at its most rearward position by the slide stop
			m_fSlidePosition = -m_fSlideStrokeDistance;
		}
		else
		{
			// Slide is not at its most rearward position:
			// the slide stop cannot be engaged
			m_IsSlideStopEngaged = false;
//			SlideForwardMotion(  );
		}
	}

	float slide_fwd_speed = 20.0f;
	float d = slide_fwd_speed * dt;
	m_fSlidePosition = get_clamped( m_fSlidePosition + d, -m_fSlideStrokeDistance, 0.0f );

	switch( m_FirearmState )
	{
	case FS_SLIDE_FORWARD:
		break;

	case FS_SLIDE_MOVING_FORWARD:
		{
			const double slide_back_to_forward_time = 0.05;

			double dCurrentTime = stage.GetElapsedTime();
			double dTimeSinceLastFire = dCurrentTime - m_dLastFireTime;
			if( slide_back_to_forward_time < dTimeSinceLastFire )
				m_FirearmState = FS_SLIDE_FORWARD;

		}
		break;

	case FS_SLIDE_OPEN:
		break;

	default:
		break;
	}
}


void CFirearm::ChangeMagazine( boost::shared_ptr<CMagazine> pNewMagazine )
{
	if( !pNewMagazine )
		return;

	if( m_PrimaryCaliber != pNewMagazine->GetCaliber() )
		return;

	if( m_pMagazine )
		m_pMagazine->SetInserted( false );

	m_pMagazine = pNewMagazine;

	m_pMagazine->SetInserted( true );
}


bool CFirearm::IsSlideOpen() const
{
	if( m_FirearmState == FS_SLIDE_OPEN )
		return true;
	else
		return false;
}


/// If the firearm is in a hold open state, close the slide.
/// If the slide is in the forward position, pull the slide
void CFirearm::FeedNextCartridge()
{
//	if( IsSlideOpen() )
	if( false )
	{
//		CloseSlide();
		if( m_pMagazine )
		{
			if( m_pMagazine->IsEmpty() )
				return;
			else
				m_pChamberedCartridge = m_pMagazine->TakeNextCartridge();
		}
	}
	else
	{
		// Empty the chamber if the round is in it.
//		ExtractCartridge();
		if( m_pChamberedCartridge )
		{
			// TODO: create an entity to display the extracted cartridge

			m_pChamberedCartridge->ReduceLoadedQuantity( 1 );
			m_pChamberedCartridge.reset();
		}

		if( m_pMagazine )
		{
			if( m_pMagazine->IsEmpty() )
				return;
//				SlideHoldOpen();
			else
				m_pChamberedCartridge = m_pMagazine->TakeNextCartridge();
		}
	}
}


void CFirearm::DisengageSlideStop()
{
	m_IsSlideStopEngaged = false;
}


bool CFirearm::IsMagazineCompliant( const boost::shared_ptr<CMagazine>& pMagazine ) const
{
	for( size_t i=0; i<m_ComplientMagazineNames.size(); i++ )
	{
		if( m_ComplientMagazineNames[i] == pMagazine->GetName() )
			return true;
	}

	return false;
}


void CFirearm::PullSlide( float fraction, bool hold_slide )
{
	if( IsSlideAtItsMostRearwardPosition() )
		return; // Cannot pull any further

	m_IsSlideHeld = hold_slide;

	clamp( fraction, 0.0f, 1.0f );

	m_fSlidePosition = -m_fSlideStrokeDistance * fraction;

	if( IsSlideAtItsMostRearwardPosition() )
	{
		if( m_pMagazine && m_pMagazine->IsEmpty() )
			m_IsSlideStopEngaged = true;

		// Feed the cartridge when the slide is full open to simplify the mechanism
		if( !IsCartridgeChambered() )
			FeedNextCartridge();
	}

//	SetSlidePulledDistance( fraction );
}


void CFirearm::ReleaseSlide()
{
	m_IsSlideHeld = false;

	if( !m_pMagazine )
	{
		// the slide stop is activated
		m_FirearmState = FS_SLIDE_OPEN;
	}
}

/*
void CFirearm::PushDownSlideRelease()
{
	if( m_FirearmState == FS_SLIDE_FORWARD
	 || m_FirearmState == FS_SLIDE_MOVING_FORWARD )
	{
		// The slide is already closing/closed - do nothing
	}
	else if( m_FirearmState == FS_SLIDE_OPEN )
	{
		m_FirearmState = FS_SLIDE_MOVING_FORWARD;
	}
}
*/

void CFirearm::Decock()
{
}


void CFirearm::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Weapon::Serialize( ar, version );

	ar & (uint&)m_PrimaryCaliber;

	/* weapon data */
//	ar & m_fFireInterval;
	ar & m_StandardMagazineCapacity;
	ar & m_fGrouping;	// grouping in 10[m]
	ar & m_ComplientMagazineNames;
	ar & m_NumTotalFiredRounds;

//	ar & m_fMuzzleSpeedFactor;

//	ar & m_vLocalRecoilForce;

//	ar & m_FireSound;

//	ar & m_iNumBursts;

//	ar & m_fBurstInterval;

//	ar & m_MuzzleEndLocalPose;

//	ar & m_MuzzleEndWorldPose;

/*	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// reset state
		m_dLastFireTime = 0;
		m_iCurrentBurstCount = 0;
		m_aTriggerState[0] = m_aTriggerState[1] = 0;

		//m_MuzzleEndWorldPose = m_MuzzleEndLocalPose;
		//m_vMuzzleEndVelocity = Vector3(0,0,0);
		//m_pOwnerEntity = ???
	}*/
}


void CFirearm::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGI_Weapon::LoadFromXMLNode( reader );

	m_PrimaryCaliber = GetCaliberFromName( reader.GetChild( "Caliber" ).GetTextContent().c_str() );

//	reader.GetChildElementTextContent( "FireInterval",             m_fFireInterval );
	reader.GetChildElementTextContent( "StandardMagazineCapacity", m_StandardMagazineCapacity );
	reader.GetChildElementTextContent( "Grouping",                 m_fGrouping ); // grouping in 10[m]
//	reader.GetChildElementTextContent( "MuzzleSpeedFactor",        m_fMuzzleSpeedFactor );
//	reader.GetChildElementTextContent( "LocalRecoilForce",         m_vLocalRecoilForce );
	reader.GetChildElementTextContent( "LocalHammerPivot",         m_vLocalHammerPivot );

	vector<CXMLNodeReader> mags = reader.GetChild( "CompliantMagazines" ).GetImmediateChildren( "Magazine" );
	m_ComplientMagazineNames.resize( mags.size() );
	for( size_t i=0; i<mags.size(); i++ )
		m_ComplientMagazineNames[i] = mags[i].GetTextContent();


/*	string fire_sound_name;
	reader.GetChildElementTextContent( "FireSound",          fire_sound_name );
	m_FireSound.SetResourceName( fire_sound_name );

	reader.GetChildElementTextContent( "NumBursts",          m_iNumBursts );

//	reader.GetChildElementTextContent( "fBurstInterval;", m_fBurstInterval );

	reader.GetChildElementTextContent( "MuzzleEndLocalPos", m_MuzzleEndLocalPose.vPosition );
*/	
}

/*
void CFirearm::UpdateWorldProperties( const Matrix34& rShooterWorldPose,
									    const Vector3& rvShooterVelocity,
										const Vector3& rvShooterAngVelocity )
{
	if( !m_pWeaponSlot )
		return;

	m_MuzzleEndWorldPose = m_pWeaponSlot->WorldPose * m_MuzzleEndLocalPose;

	m_vMuzzleEndVelocity
		= rvShooterVelocity;// + Vec3Cross( rvShooterAngVelocity, m_MuzzleEndWorldPose.vPosition - rShooterWorldPose.vPosition );
}

static bool SafetyOff() { return true; }

// standard update - fire if both triggers are pulled
void CFirearm::Update( float dt )
{
	if( !IsWeaponSelected() )
		return;

	if( m_aTriggerState[0] == 1 && SafetyOff() )//m_aTriggerState[1] == 1 )
	{
		if( 0 < m_iCurrentBurstCount && m_iCurrentBurstCount < m_iNumBursts )
		{
			Fire();
		}
	}
}*/


static bool SafetyOff() { return true; }

bool CFirearm::HandleInput( int action_code, int input_type, float fParam )
{
	if( action_code == ACTION_NOT_ASSIGNED )
		return false;

	switch( action_code )
	{
	case ACTION_ATK_FIRE:
		if( input_type == ITYPE_KEY_PRESSED )
		{
			m_aTriggerState[0] = 1;

			// the first shot is fired by trigger pull event
			if( SafetyOff()/*m_aTriggerState[1] == 1*/
				&& m_iCurrentBurstCount == 0 )
			{
				Fire();
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


bool CFirearm::IsReadyToFire() const
{
	return ( fabs(m_fSlidePosition) < 0.001f );
}


void CFirearm::Fire()
{
//	CStageSharedPtr pStage = m_pStage.lock();

	if( !IsReadyToFire() )
		return;

	shared_ptr<CItemEntity> pEntity = GetItemEntity().Get();
	if( !pEntity )
		return;

	CStage *pStage = pEntity->GetStage();

	if( !pStage )
		return;

	UpdateFirearmState( *pStage );

//	if( m_FirearmState != FS_SLIDE_FORWARD )
//		return; // The slide is not in the forward position: not ready to fire.

	if( !m_pChamberedCartridge )
		return; // no round in the chamber

	CCartridge& cartridge = *m_pChamberedCartridge;

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

	// ------------------ new fire ------------------

	Matrix34 muzzle_end_world_pose( Matrix34Identity() );
//	boost::shared_ptr<CCopyEntity> pEntity = GetItemEntity().Get();
	if( pEntity )
		muzzle_end_world_pose = pEntity->GetWorldPose() * m_MuzzleEndLocalPose;
	else
		muzzle_end_world_pose = m_MuzzleEndWorldPose;

	const Vector3& rvMuzzleEndPosition	= muzzle_end_world_pose.vPosition;
	const Matrix33& matMuzzleOrient		= muzzle_end_world_pose.matOrient;
	const Vector3& rvMuzzleDir_Right	= matMuzzleOrient.GetColumn(0);
	const Vector3& rvMuzzleDir_Up		= matMuzzleOrient.GetColumn(1);
	const Vector3& rvMuzzleDirection	= matMuzzleOrient.GetColumn(2);

	// play sound for the shot
//	ms_pStage->PlaySoundAt( m_FireSound, rvMuzzleEndPosition );
	SoundManager().PlayAt( m_FireSound, rvMuzzleEndPosition );

	Vector3 vFireDirection;
	Vector3 vFireDir_Right, vUp = Vector3(0,1,0);
	CCopyEntityDesc bullet_entity;

	bullet_entity.SetWorldPosition( rvMuzzleEndPosition );

	// set group for the bullet
	// e.g.) projectile fired by the player is marked as 'CE_GROUP_PLAYER_PROJECTILE'
	// this is used to avoid hitting the shooter
	bullet_entity.sGroupID = m_pWeaponSlot ? m_pWeaponSlot->ProjectileGroup : 0;

	for( uint i=0; i<cartridge.GetNumPellets(); i++ )
	{
		// set pointer to the base entity which serves as the bullet of this weapon
		bullet_entity.pBaseEntityHandle = &cartridge.GetBaseEntityHandle();

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
		vFireDirection = rvMuzzleDirection * 10.0f
					   + rvMuzzleDir_Right * m_fGrouping * x
					   + rvMuzzleDir_Up    * m_fGrouping * y;

		// TODO: change vUp when vFireDirection is almost (0,1,0)
		Vec3Normalize( vFireDirection, vFireDirection );
		bullet_entity.SetWorldOrient( CreateOrientFromFwdDir(vFireDirection) );

		bullet_entity.vVelocity
			= vFireDirection * cartridge.GetMuzzleSpeed() * m_fMuzzleSpeedFactor
			+ m_vMuzzleEndVelocity;

		pStage->CreateEntity( bullet_entity );
	}

	// create muzzle flash
	if( cartridge.GetMuzzleFlashHandle().GetBaseEntityName()
	 && 0 < strlen(cartridge.GetMuzzleFlashHandle().GetBaseEntityName()) )
	{
		CCopyEntityDesc& rMuzzleFlashDesc = bullet_entity;	// reuse the desc object 
		rMuzzleFlashDesc.pBaseEntityHandle = &cartridge.GetMuzzleFlashHandle();
		rMuzzleFlashDesc.vVelocity = m_vMuzzleEndVelocity * 0.8f; // Vector3(0,0,0);
		pStage->CreateEntity( rMuzzleFlashDesc );
	}

	// A bullet has been fired: feed the next round

	m_pChamberedCartridge->ReduceLoadedQuantity( 1 );
	m_pChamberedCartridge.reset();

	// m_pChamberedCartridge has been reset. Do not touch 'cartridge' after this

	// For now, slide instantly goes to its full backward position
	m_fSlidePosition = -m_fSlideStrokeDistance;

	if( m_pMagazine )
	{
		if( m_pMagazine->IsEmpty() )
		{
			m_IsSlideStopEngaged = true;
			m_FirearmState = FS_SLIDE_OPEN;
		}
		else
		{
			m_pChamberedCartridge = m_pMagazine->TakeNextCartridge();
		}
	}
	else
	{ 
		m_FirearmState = FS_SLIDE_MOVING_FORWARD;
	}

	// recoil effect
	SGameMessage msg;
	msg.effect = GM_IMPACT;
	msg.vParam = - vFireDirection * 5.0f;

/*	Vector3 vImpact = m_vLocalRecoilForce;
	msg.vParam = vImpact.z * rvMuzzleDirection
		       + vImpact.y * rvMuzzleDir_Up
			   + vImpact.x * rvMuzzleDir_Right;
*/

//	SendGameMessageTo( msg, SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity() );

//	if( m_pOwnerEntity )
//		SendGameMessageTo( msg, m_pOwnerEntity );

}
