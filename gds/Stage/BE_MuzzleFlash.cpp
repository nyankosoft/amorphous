#include "BE_MuzzleFlash.hpp"

#include "Support/MTRand.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "Serialization_BaseEntityHandle.hpp"


CBE_MuzzleFlash::CBE_MuzzleFlash()
{
	m_fMuzzleFlash_MaxRadius = 0.12f;
	m_MaxNumParticlesPerSet = 16;
}


void CBE_MuzzleFlash::Init()
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
}


void CBE_MuzzleFlash::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime = 0.0f;

//	pCopyEnt->bUseZSort = true;
	pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );
	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	// set the number of particles for this particle set
	rParticleSet.iNumParticles = m_MaxNumParticlesPerSet;

	float fMaxRadius = m_fMuzzleFlash_MaxRadius;

	Vector3 vPos;
	Vector3	vBasePos = pCopyEnt->GetWorldPosition();
	Vector3 vDir = pCopyEnt->GetDirection();
	Vector3 vRight = pCopyEnt->GetRightDirection();
	Vector3	vUp = pCopyEnt->GetUpDirection();
	Vector3	vVelocity = pCopyEnt->Velocity();
	Vector3 vRandDir;

	int i, num_particles = rParticleSet.iNumParticles;
	int iRandVal;
	for( i=0; i<num_particles; i++ )
	{
		rParticleSet.pafAnimationTime[i] = 0;

		iRandVal = RangedRand( 0, CBE_MUZZLEFLASH_NUM_RAND_POSITIONS - 1 );
		if( iRandVal < 0 ) iRandVal = 0;
		vPos = vBasePos + vDir * ( 0.02f + m_afRandPosisionTable[iRandVal] * fMaxRadius * 2.2f );

		// add variation for right & up direction (technique 1)
		iRandVal = RangedRand( 0, CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS - 1 );
		if( iRandVal < 0 ) iRandVal = 0;
		vRandDir = m_avRandDir_Circle[iRandVal];
		vPos += ( vRight * vRandDir.x + vUp * vRandDir.y) * fMaxRadius * 0.2f;

		// add variation for right & up direction (technique 0)
/*		iRandVal = rand() * CBE_MUZZLEFLASH_NUM_RAND_POSITIONS / RAND_MAX - 1;
		if( iRandVal < 0 ) iRandVal = 0;
		vPos += vRight * m_afRandPosisionTable[iRandVal] * fMaxRadius;

		iRandVal = rand() * CBE_MUZZLEFLASH_NUM_RAND_POSITIONS / RAND_MAX - 1;
		if( iRandVal < 0 ) iRandVal = 0;
		vPos += vUp * m_afRandPosisionTable[iRandVal] * fMaxRadius;*/

		rParticleSet.pavPosition[i] = vPos;
		rParticleSet.pavVelocity[i] = vVelocity; // Vector3(0,0,0);
		rParticleSet.pafAnimationTime[i] = 0;
		rParticleSet.pasPattern[i] = RangedRand( m_iNumTextureSegments * m_iNumTextureSegments * 4 - 1 );
	}

	if( 0 < strlen(m_MuzzleFlashLight.GetBaseEntityName()) )
	{
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_MuzzleFlashLight;
		light.SetWorldPosition( pCopyEnt->GetWorldPosition() );
		m_pStage->CreateEntity( light );
	}

}


void CBE_MuzzleFlash::Draw( CCopyEntity* pCopyEnt )
{
	DrawParticles(pCopyEnt);
}


bool CBE_MuzzleFlash::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
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


void CBE_MuzzleFlash::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );

	ar & m_fMuzzleFlash_MaxRadius;
	ar & m_MuzzleFlashLight;
}
