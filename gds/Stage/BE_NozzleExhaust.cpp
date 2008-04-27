
#include "3DCommon/Direct3D9.h"
#include "GameCommon/BasicGameMath.h"
#include "GameCommon/MTRand.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "Stage.h"
#include "Serialization_BaseEntityHandle.h"

#include "BE_NozzleExhaust.h"


CBE_NozzleExhaust::CBE_NozzleExhaust()
{
//	m_fMuzzleFlash_MaxRadius = 0.12f;
	m_MaxNumParticlesPerSet = 16;

	m_bWorldOffset = true;
}


void CBE_NozzleExhaust::Init()
{
	CBE_ParticleSet::Init();

	int i;
	float t, fDenominator = (float)( CBE_MUZZLEFLASH_NUM_RAND_POSITIONS * CBE_MUZZLEFLASH_NUM_RAND_POSITIONS );
	for( i=0; i<CBE_MUZZLEFLASH_NUM_RAND_POSITIONS; i++ )
	{
		t = (float)i;
		m_afRandPosisionTable[i] = t * t / fDenominator;
	}

	// suppose a circle on xy-plane
	for( i=0; i<CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS; i++ )
	{
		t = D3DX_PI * (float)i / (float)CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS;
		m_avRandDir_Circle[i] = Vector3( (float)cos(t), (float)sin(t), 0 );
	}

	LoadBaseEntity( m_MuzzleFlashLight );

	int j;
	for(i=0; i<m_MaxNumParticleSets; i++)
	{
		for(j=0; j<m_MaxNumParticlesPerSet; j++)
		{
			m_paParticleSet[i].pafAnimDuration[j] = m_fDuration;
		}
	}
}


void CBE_NozzleExhaust::CreateNewParticle( CCopyEntity* pCopyEnt, SBE_ParticleSetExtraData& rParticleSet, int index )
{
	const Vector3 vDir	= Vector3(0,0,1);
	Vector3 vRight	= Vector3(1,0,0);
	Vector3	vUp		= Vector3(0,1,0);
	Vector3	vBasePos= vDir * NozzleFlameStartPos(pCopyEnt);
	Vector3 vDestPos= vBasePos + vDir * NozzleFlameLength(pCopyEnt);
	Vector3 vVelOffset = Vector3(0,0,0);

//	Vector3 vPos = vBasePos + vRight * RangedRand( -0.4f, 0.4f ) + vUp * RangedRand( -0.4f, 0.4f );
	float r = NozzleFlameRadius(pCopyEnt) - m_fParticleRadius;
	Vector3 vPos = vBasePos + vRight * RangedRand( -r, r ) + vUp * RangedRand( -r, r );

	rParticleSet.pavPosition[index] = vPos;
//	float dist = Vec3Length(vDestPos - vPos);
	rParticleSet.pavVelocity[index] = vVelOffset + (vDestPos - vPos) / m_fDuration * RangedRand( 0.8f, 1.2f );
	rParticleSet.pasPattern[index] = RangedRand( m_iNumTextureSegments * m_iNumTextureSegments * 4 - 1 );

	rParticleSet.pafAnimationTime[index] = RangedRand( -m_fDuration, 0.0f );

}


void CBE_NozzleExhaust::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	float& rfCurrentTime = CurrentTime(pCopyEnt);
	rfCurrentTime = 0.0f;

//	pCopyEnt->bUseZSort = true;
	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;
	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	// set the number of particles for this particle set
	rParticleSet.iNumParticles = m_MaxNumParticlesPerSet;

//	float fMaxRadius = m_fMuzzleFlash_MaxRadius;

	Vector3 vPos;
/*	Vector3	vBasePos = Vector3(0,0,0);//pCopyEnt->Position();// + pCopyEnt->GetDirection() * pCopyEnt->f1;
	Vector3 vDir	= Vector3(0,0,1);//pCopyEnt->GetDirection();
	Vector3 vRight	= Vector3(1,0,0);//pCopyEnt->GetRightDirection();
	Vector3	vUp		= Vector3(0,1,0);//pCopyEnt->GetUpDirection();
	Vector3	vVelocity = pCopyEnt->Velocity();
	Vector3	vVelOffset;
	Vector3 vDestPos = vBasePos + vDir * 3.0f;// * CurrentTime(pCopyEnt);
*/
//	if( IsValidEntity(pCopyEnt->pParent) )
//		vVelOffset = pCopyEnt->pParent->Velocity();
//	else
///		vVelOffset = Vector3(0,0,0);

	int i, num_particles = rParticleSet.iNumParticles;
//	int iRandVal;
	for( i=0; i<num_particles; i++ )
	{
		CreateNewParticle( pCopyEnt, rParticleSet, i );

/*		vPos = vBasePos + vRight * RangedRand( -0.5f, 0.5f ) + vUp * RangedRand( -0.5f, 0.5f );

		rParticleSet.pavPosition[i] = vPos;
		float dist = Vec3Length(vDestPos - vPos);
		rParticleSet.pavVelocity[i] = vVelOffset + (vDestPos - vPos) / dist;
		rParticleSet.pasPattern[i] = RangedRand( m_iNumTextureSegments * m_iNumTextureSegments * 4 - 1 );

		rParticleSet.pafAnimationTime[i] = RangedRand( -m_fDuration, 0.0f );*/
	}

/*	if( 0 < strlen(m_MuzzleFlashLight.GetBaseEntityName()) )
	{
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_MuzzleFlashLight;
		light.vPosition = pCopyEnt->Position();
		m_pStage->CreateEntity( light );
	}
*/
}


void CBE_NozzleExhaust::Act( CCopyEntity* pCopyEnt )
{
	float fFrameTime = m_pStage->GetFrameTime();

	float& rfCurrentTime = CurrentTime(pCopyEnt);
	rfCurrentTime += fFrameTime;

	// update the positions of particles

//	Vector3 vGravityAccel = m_pStage->GetGravityAccel() * m_fGravityInfluenceFactor;

//	DWORD dwColor = D3DCOLOR_ARGB( ((int)((m_fDuration - rfCurrentTime) / m_fDuration * 255.0f)), 255, 255, 255 );

	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	const float dt = 0.008f;
	const float fTotalFrameTime = fFrameTime + OverlapTime(pCopyEnt);
	int num_loops = (int)(fTotalFrameTime / dt);
	OverlapTime(pCopyEnt) = fTotalFrameTime - dt * (float)num_loops;
	for( int i=0; i<num_loops; i++ )
	{
		UpdateNozzleExhaust( pCopyEnt, rParticleSet, dt );
	}

	if( m_Type == CBEC_Billboard::TYPE_BILLBOARDARRAYMESH )
		UpdateVertexBuffer( pCopyEnt );
}


void CBE_NozzleExhaust::UpdateNozzleExhaust( CCopyEntity* pCopyEnt, SBE_ParticleSetExtraData& rParticleSet, float dt )
{
	int i, num_particles = m_MaxNumParticlesPerSet;

//	float dt = fFrameTime;
	AABB3 aabb;
	aabb.Nullify();
//	UpdateParticles( rParticleSet, fFrameTime, aabb );
	float radius = m_fParticleRadius;
	for(i=0; i<num_particles; i++)
	{
		// update animation time
		rParticleSet.pafAnimationTime[i] += dt;

		if( rParticleSet.pafAnimationTime[i] < 0 )
			continue;

		// update velocity
//		rParticleSet.pavVelocity[i] += vGravityAccel * dt;

		// update position
		rParticleSet.pavPosition[i] += rParticleSet.pavVelocity[i] * dt;

		aabb.AddSphere( Sphere(rParticleSet.pavPosition[i],radius) );
	}

	pCopyEnt->world_aabb.MergeAABB( aabb );

	Vector3	vBasePos = Vector3(0,0,0);//pCopyEnt->Position();// + pCopyEnt->GetDirection() * pCopyEnt->f1;
	Vector3 vDir	= Vector3(0,0,1);//pCopyEnt->GetDirection();
	Vector3 vRight	= Vector3(1,0,0);//pCopyEnt->GetRightDirection();
	Vector3	vUp		= Vector3(0,1,0);//pCopyEnt->GetUpDirection();
	float cycle_time = m_fDuration;
	for( i=0; i<num_particles; i++ )
	{
		if( cycle_time <= rParticleSet.pafAnimationTime[i] )
		{
			// set new position & velocity

			CreateNewParticle( pCopyEnt, rParticleSet, i );
//			rParticleSet.pafAnimationTime[i] = 0;
		}
	}
}


void CBE_NozzleExhaust::Draw( CCopyEntity* pCopyEnt )
{
	DrawParticles(pCopyEnt);
}


bool CBE_NozzleExhaust::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_ParticleSet::LoadSpecificPropertiesFromFile( scanner );

	string light_entity;

	if( scanner.TryScanLine( "LIGHT", light_entity ) )
	{
		m_MuzzleFlashLight.SetBaseEntityName( light_entity.c_str() );
		return true;
	}

	return false;
}


void CBE_NozzleExhaust::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );

//	ar & m_fMuzzleFlash_MaxRadius;
	ar & m_MuzzleFlashLight;
}
