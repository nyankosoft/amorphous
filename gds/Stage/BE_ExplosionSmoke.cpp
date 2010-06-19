#include "BE_ExplosionSmoke.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "Support/MTRand.hpp"
#include "Support/VectorRand.hpp"


CBE_ExplosionSmoke::CBE_ExplosionSmoke()
{
	m_fRandPositionMin = m_fRandPositionMax = 0;
	m_fRandVelocityMin = m_fRandVelocityMax = 0;
}


void CBE_ExplosionSmoke::Init()
{
	CBE_ParticleSet::Init();
}


void CBE_ExplosionSmoke::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime = 0.0f;

	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	Vector3& rvDir = pCopyEnt->GetDirection();
	Vector3 vRandDir;
	float fDist, fRandSpeed;
	float fRandVelMin = m_fRandVelocityMin;
	float fRandVelMax = m_fRandVelocityMax;
	float fRandPositionMin = m_fRandPositionMin;
	float fRandPositionMax = m_fRandPositionMax;
	int i, iNumParticles = m_MaxNumParticlesPerSet;
	rParticleSet.iNumParticles = iNumParticles;
	int iNumPatterns = m_iNumTextureSegments * m_iNumTextureSegments * 4 - 1;

	if( m_ParticleType == TYPE_SMOKE_SHOOTING)
	{
		for(i=0; i<iNumParticles; i++)
		{
			rParticleSet.pavOrigDirection[i] = rvDir;

			vRandDir = Vec3RandDir();

			rParticleSet.pavPosition[i] = pCopyEnt->GetWorldPosition();

			rParticleSet.pavVelocity[i] = vRandDir * RangedRand( 0.05f, 0.50f) + rvDir * RangedRand( 0.1f, 12.0f);

			rParticleSet.pafAnimationTime[i] = RangedRand( m_fAnimTimeOffsetMin, m_fAnimTimeOffsetMax );

			rParticleSet.pasPattern[i] = RangedRand( iNumPatterns );

			rParticleSet.pafFadeVel[i] = RangedRand( 1.6f, 2.0f );
		}
	}
	else // i.e. TYPE_SMOKE_NORMAL or TYPE_SMOKE_DIRECTIONAL
	{
		for(i=0; i<iNumParticles; i++)
		{
			vRandDir = Vec3RandDir();

			fRandSpeed = RangedRand( 0.8f );

			fDist = RangedRand( fRandPositionMin, fRandPositionMax );

			rParticleSet.pavPosition[i] = pCopyEnt->GetWorldPosition() + vRandDir * fDist + rvDir * (0.8f - fRandSpeed) * 0.3f;

			rParticleSet.pavVelocity[i] = vRandDir * RangedRand( fRandVelMin, fRandVelMax ) + rvDir * fRandSpeed;

			rParticleSet.pafAnimationTime[i] = RangedRand( m_fAnimTimeOffsetMin, m_fAnimTimeOffsetMax );

			rParticleSet.pasPattern[i] = RangedRand( iNumPatterns );
		}
	}
}


void CBE_ExplosionSmoke::Draw(CCopyEntity* pCopyEnt)
{
	DrawParticles( pCopyEnt );
}


bool CBE_ExplosionSmoke::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_ParticleSet::LoadSpecificPropertiesFromFile( scanner );

	if( scanner.TryScanLine( "RAND_POS", m_fRandPositionMin, m_fRandPositionMax ) ) return true;

	if( scanner.TryScanLine( "RAND_VEL", m_fRandVelocityMin, m_fRandVelocityMax ) ) return true;

	return false;
}


void CBE_ExplosionSmoke::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );

	ar & m_fRandPositionMin & m_fRandPositionMax;
	ar & m_fRandVelocityMin & m_fRandVelocityMax;
}
