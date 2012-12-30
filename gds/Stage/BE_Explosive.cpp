#include "BE_Explosive.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Serialization_BaseEntityHandle.hpp"
#include "../Stage/ScreenEffectManager.hpp"

#include "3DMath/3DGameMath.hpp"
#include "Graphics/Camera.hpp"

#include "Support/MTRand.hpp"
#include "Support/VectorRand.hpp"

#include "Sound/SoundManager.hpp"
#include "Sound/Serialization_SoundHandle.hpp"


namespace amorphous
{

using namespace std;


CBE_Explosive::CBE_Explosive()
{
	m_ExplosiveTypeFlag = 0;
	m_iNumFragments = 0;
	m_fTimer = 0.0f;

	m_fExplosionAnimTimeOffset = 0;
	m_fExplosionAnimRange = 0.4f;

	SetLighting( true );

	m_ExplosionSound.SetResourceName( "bom30" );

	m_fMinimumExplodeImpactSpeed = 1000000.0f;
}


void CBE_Explosive::Init()
{
	CBE_PhysicsBaseEntity::Init();

	Init3DModel();

	if( strlen(m_aExplosionAnimation[AT_EXPLOSION].GetBaseEntityName()) == 0 )
		m_aExplosionAnimation[AT_EXPLOSION].SetBaseEntityName( "Bang" );

	if( strlen(m_aExplosionAnimation[AT_SMOKE].GetBaseEntityName()) == 0 )
		m_aExplosionAnimation[AT_SMOKE].SetBaseEntityName( "xpbsmk" );

	if( strlen(m_aExplosionAnimation[AT_SPARK].GetBaseEntityName()) == 0 )
		m_aExplosionAnimation[AT_SPARK].SetBaseEntityName( "Lspk" );

	if( strlen(m_aExplosionAnimation[AT_SHOCKWAVE].GetBaseEntityName()) == 0 )
		m_aExplosionAnimation[AT_SHOCKWAVE].SetBaseEntityName( "ShockWave" );

	int i;
	for( i=0; i<NUM_EXPLOSION_ANIMATIONS; i++ )
		LoadBaseEntity( m_aExplosionAnimation[i] );

    LoadBaseEntity( m_Blast );
	LoadBaseEntity( m_Fragment );
	LoadBaseEntity( m_ExplosionLight );
}


void CBE_Explosive::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->fLife= m_fLife;

	if( m_ExplosiveTypeFlag & TYPE_TIMER )
	{	// set the timer
		float& rfTimer = pCopyEnt->f3;
		rfTimer = m_fTimer;
	}

	pCopyEnt->m_MeshHandle = m_MeshProperty.m_MeshObjectHandle;
}


void CBE_Explosive::Explode(CCopyEntity* pCopyEnt)
{
	// show explosion animation - single
	CCopyEntityDesc explosion;
	explosion.pBaseEntityHandle = &m_aExplosionAnimation[AT_EXPLOSION];
	explosion.SetWorldPosition( pCopyEnt->GetWorldPosition() + pCopyEnt->touch_plane.normal * 0.40f );	// make an explosion at the position a little ahead from the entity to prevent a texture animation from getting in the wall
	explosion.SetWorldOrient( Matrix33Identity() );
	explosion.vVelocity  = Vector3(0,0,0);
	m_pStage->CreateEntity( explosion );

	// show explosion animation - multiple explosions with variations in positions and animation times 
	float& rfOffset = m_fExplosionAnimTimeOffset;
	float& rfAnimRange = m_fExplosionAnimRange;
	Vector3 vPos;
	int i;
	for(i=0; i<2; i++)
	{
		vPos = pCopyEnt->GetWorldPosition()
			+ pCopyEnt->touch_plane.normal * 0.40f
			+ pCopyEnt->GetRightDirection() * RangedRand( -rfAnimRange, rfAnimRange )
			+ pCopyEnt->GetUpDirection()    * RangedRand( -rfAnimRange, rfAnimRange );

		explosion.SetWorldPosition( vPos );
		explosion.vVelocity = Vector3(0,0,0);
//		explosion.f1 = - 0.08f - 0.16f * (float)rand() / (float)RAND_MAX;	// set animation time offset
		explosion.f1 = rfOffset + rfOffset * 2.0f * (float)rand() / (float)RAND_MAX;	// set animation time offset
		m_pStage->CreateEntity( explosion );
	}

	// create shock wave
	CCopyEntityDesc& shock_wave = explosion;	// re-use the desc object
	shock_wave.pBaseEntityHandle = &m_aExplosionAnimation[AT_SHOCKWAVE];
	if( pCopyEnt->touch_plane.normal != Vector3(0,0,0) )
	{
		// for missiles that hit a surface
		shock_wave.SetWorldPosition( pCopyEnt->GetWorldPosition() + pCopyEnt->touch_plane.normal * 0.01f );
		shock_wave.SetWorldOrient( CreateOrientFromFwdDir( - pCopyEnt->touch_plane.normal ) );
	}
	else 
	{	// for explosives placed on the ground
		shock_wave.SetWorldPosition( pCopyEnt->GetWorldPosition() - Vector3( 0, m_aabb.vMin.y * 0.8f,0) );
		shock_wave.SetWorldOrient( CreateOrientFromFwdDir( Vector3(0,-1,0) ) );
	}

	shock_wave.f1		  = 0.05f;	// animation time offset
	this->m_pStage->CreateEntity( shock_wave );

	// show black smoke
	this->m_pStage->CreateEntity( m_aExplosionAnimation[AT_SMOKE],
	      pCopyEnt->GetWorldPosition() + pCopyEnt->touch_plane.normal * 0.36f,	// make an explosion at the position a little ahead from the entity to prevent a texture animation from getting in the wall
		  Vector3(0,0,0), Vector3(0,0,0) );

	// show particle animation
	Vector3 vParticleVelocity;
	if( pCopyEnt->touch_plane.normal != Vector3(0,0,0) )
		vParticleVelocity = pCopyEnt->touch_plane.normal * 2.0f;	// hit something - 'pCopyEnt' is probably a grenade
	else
		vParticleVelocity = Vector3(0, 2.5f, 0);	// not hitting anything - 'pCopyEnt' is probably a static explosive

	this->m_pStage->CreateEntity( m_aExplosionAnimation[AT_SPARK], pCopyEnt->GetWorldPosition(), vParticleVelocity, Vector3(0,0,0) );


	// play explosion sound
	SoundManager().PlayAt( m_ExplosionSound, pCopyEnt->GetWorldPosition() );

	// if 'pCopyEnt' is a homing missile, target entity has to be cleared
	pCopyEnt->m_Target.Reset();

	// flash the screen if explosion occured near the camera
	FlashScreen( pCopyEnt );

	if( m_ExplosiveTypeFlag & TYPE_BLAST )
	{
		// create blast entity
		// the direction of the explosive entity is inherited by the blast entity
		m_pStage->CreateEntity( m_Blast, pCopyEnt->GetWorldPosition(),
						 		 Vector3(0,0,0), pCopyEnt->GetDirection() );
	}

	// set dynamic light
	if( 0 < strlen(m_ExplosionLight.GetBaseEntityName()) )
	{
		explosion.pBaseEntityHandle = &m_ExplosionLight;
		explosion.SetWorldPosition( pCopyEnt->GetWorldPosition() );
		explosion.vVelocity = Vector3(0,0,0);
		m_pStage->CreateEntity( explosion );
	}

	if( m_ExplosiveTypeFlag & TYPE_FRAGMENTS )
	{
		// create fragment entities
		// directions of fragment entities are generated randomly
		CCopyEntityDesc fragment;
		fragment.pBaseEntityHandle = &m_Fragment; //m_pStage->FindBaseEntity( m_acFragmentEntityName );
		Vector3 vDir;

		for( i=0; i<m_iNumFragments; i++ )
		{
			fragment.SetWorldPosition( pCopyEnt->GetWorldPosition() + vDir * 0.05f );

			vDir = Vec3RandDir();
			fragment.SetWorldOrient( CreateOrientFromFwdDir(vDir) );
			fragment.vVelocity = vDir * 80.0f;
			fragment.fSpeed = 80.0; //m_fFragmentSpeed;

			m_pStage->CreateEntity( fragment );
		}
	}

	// terminate myself
	m_pStage->TerminateEntity( pCopyEnt );
}


void CBE_Explosive::FlashScreen(CCopyEntity* pCopyEnt)
{
	CCamera *pCamera = m_pStage->GetEntitySet()->GetCurrentCamera();

	if( !pCamera )
		return;

	Vector3 vCameraToExp = pCopyEnt->GetWorldPosition() - pCamera->GetPosition();

	// check if there is any obstacle between explosion and camera
/*	STrace tr;
	tr.vStart = pCamera->GetPosition();
	tr.vGoal = pCopyEnt->GetWorldPosition();
	tr.bvType = BVTYPE_DOT;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES
	tr.pSourceEntity = pCopyEnt;

	this->m_pStage->ClipTrace( tr );*/

	float fDist = Vec3Length(vCameraToExp);	// distance from camera to the center of the explosion

	const float fFlashEffectiveDist = 18.0f;
	
	if( fFlashEffectiveDist < fDist )
		return;

	// 0 <= alpha <= 1
	// alpha = 1 : explosion is close to the camera
	// alpha = 0 : explosion is far from camera
	float fAlpha = (fFlashEffectiveDist - fDist) / fFlashEffectiveDist;
	SFloatRGBAColor flash_color( 1.0f, 1.0f, 1.0f, fAlpha * 180.0f / 256.0f );

	m_pStage->GetScreenEffectManager()->FadeInFrom( flash_color, 0.6f + fAlpha, AlphaBlend::One );

}


void CBE_Explosive::Act( CCopyEntity* pCopyEnt )
{
	if( m_ExplosiveTypeFlag & TYPE_TIMER )
	{
		float& rfTimer = pCopyEnt->f3;
		rfTimer -= m_pStage->GetFrameTime();
		if( rfTimer <= 0 )
			Explode( pCopyEnt );
	}

	if( pCopyEnt->vVelocity == Vector3(0,0,0) )
		return;

	pCopyEnt->vVelocity += m_pStage->GetGravityAccel() * m_pStage->GetFrameTime();
	pCopyEnt->f1 = Vec3LengthSq( pCopyEnt->vVelocity );
//	char blocked = SlideMove( pCopyEnt );
	GrenadeMove( pCopyEnt );

	if( pCopyEnt->bInSolid && false )//m_ExplosiveFlags & EF_EXPLODE_WHEN_STUCK )
		Explode( pCopyEnt );
}

void CBE_Explosive::GrenadeMove(CCopyEntity* pCopyEnt)
{
	STrace tr;
	float fFrametime = m_pStage->GetFrameTime();

	// change velocity according to gravity
//	Vector3 vGravityAccel = this->m_pStage->GetGravityAccel();
//	pCopyEnt->vVelocity += vGravityAccel * fFrametime * 0.5f;

	Vector3 vStart = pCopyEnt->GetWorldPosition();
	tr.vStart = vStart;
	Vector3 vGoal = pCopyEnt->GetWorldPosition() + pCopyEnt->vVelocity * fFrametime;
	tr.vGoal = vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;

	this->m_pStage->ClipTrace( tr );

	pCopyEnt->SetWorldPosition( tr.vEnd );
	pCopyEnt->touch_plane = tr.plane;
	pCopyEnt->bInSolid = tr.in_solid;

//	if( tr.in_solid )
//		NudgePosition( pCopyEnt );

	if( tr.fFraction < 1.0f )	// hit something
	{
		pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );
		pCopyEnt->fSpeed = 0;
//		pCopyEnt->Direction( Vector3(0,0,0) );
		pCopyEnt->vVelocity = Vector3(0, 0, 0);
		pCopyEnt->sState |= CESTATE_ATREST;
		return;
	}
}

void CBE_Explosive::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.effect )
	{
	case GM_DAMAGE:
		float& rfLife = pCopyEnt_Self->fLife;
		rfLife -= rGameMessage.fParam1;

		SoundManager().PlayAt( "bosu21", pCopyEnt_Self->GetWorldPosition() );

		if( rfLife <= 0 )
			Explode( pCopyEnt_Self );
		break;
	}
}

// if the entity is an impact grenade, it explodes at the moment of impact
void CBE_Explosive::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
	if( pCopyEnt_Other && pCopyEnt_Other->bNoClip )
		return;

	// check the energy of motion recorded before the velocity is clipped
//	if( 3.0f * 3.0f < Vec3LengthSq( pCopyEnt_Self->vVelocity ) )
	if( square( m_fMinimumExplodeImpactSpeed ) < Vec3LengthSq( pCopyEnt_Self->vVelocity ) )
	{
		Explode( pCopyEnt_Self );
	}
}


void CBE_Explosive::Draw(CCopyEntity* pCopyEnt)
{
	Draw3DModel(pCopyEnt);
	return;
}


bool CBE_Explosive::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile(scanner);

	string str;

	if( scanner.TryScanLine( "BLASTNAME", str ) )
	{
		m_Blast.SetBaseEntityName( str.c_str() );
		m_ExplosiveTypeFlag |= TYPE_BLAST;
		return true;
	}

	if( scanner.TryScanLine( "FRAGMENTNAME", str ) )
	{
		m_Fragment.SetBaseEntityName( str.c_str() );
		m_ExplosiveTypeFlag |= TYPE_FRAGMENTS;
		return true;
	}

	if( scanner.TryScanLine( "TIMER", m_fTimer ) )
	{
		m_ExplosiveTypeFlag |= TYPE_TIMER;
		return true;
	}

	if( scanner.TryScanLine( "NUM_FRAGMENTS", m_iNumFragments ) ) return true;

	if( scanner.TryScanLine( "EXPLOSION_ANIM_TIME_OFFSET", m_fExplosionAnimTimeOffset ) ) return true;

	if( scanner.TryScanLine( "EXPLOSION_ANIMRANGE", m_fExplosionAnimRange ) ) return true;

	if( scanner.TryScanLine( "EXPLOSION_ANIM", str ) )
	{
		m_aExplosionAnimation[AT_EXPLOSION].SetBaseEntityName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "EXPLOSION_SMOKE", str ) )
	{
		m_aExplosionAnimation[AT_SMOKE].SetBaseEntityName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "EXPLOSION_SPARK", str ) )
	{
		m_aExplosionAnimation[AT_SPARK].SetBaseEntityName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "EXPLOSION_LIGHT", str ) )
	{
		m_ExplosionLight.SetBaseEntityName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "EXPLOSION_SOUND", str ) )
	{
		m_ExplosionSound.SetResourceName( str.c_str() );
		return true;
	}

	return false;
}


void CBE_Explosive::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );

	ar & m_ExplosiveTypeFlag;
	ar & m_fTimer;
	ar & m_fExplosionAnimTimeOffset;
	ar & m_fExplosionAnimRange;

	ar & m_Blast;
	ar & m_Fragment;
	ar & m_iNumFragments;
	ar & m_ExplosionLight;

	for( int i=0; i<NUM_EXPLOSION_ANIMATIONS; i++ )
        ar & m_aExplosionAnimation[i];

	ar & m_ExplosionSound;
}


} // namespace amorphous
