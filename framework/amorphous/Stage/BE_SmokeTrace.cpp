#include "BE_SmokeTrace.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Support/VectorRand.hpp"
#include "amorphous/Support/Profile.hpp"


namespace amorphous
{

using namespace std;


CBE_SmokeTrace::CBE_SmokeTrace()
{
	m_iParticleSetCurrentIndex = 0;
	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	SetLighting( false );

	m_SmokeType = ST_PARTICLES_PER_DISTANCE;
	m_iNumParticlesPerSec = 1;
	m_fGravityInfluenceFactor = 0.0f;

	m_fSmokeTraceParticleRand = 0.07f;

	// use thread by default
	m_bCreateParticleThread = false;
//	m_bCreateParticleThread = false;

	m_bMinimumParticleUpdates = false;
}


CBE_SmokeTrace::~CBE_SmokeTrace()
{
//	if( m_bCreateParticleThread )
//	m_pThreadPooler->ClearAllThread();

	ReleaseGraphicsResources();
}


void CBE_SmokeTrace::Init()
{
	CBE_ParticleSet::Init();

	float fMaxSpan, fMaxRadius;
	fMaxSpan = m_fParticleRadius * 2.0f + m_fParticleInterval * (float)(m_MaxNumParticlesPerSet - 1);
	fMaxRadius = fMaxSpan / 2.0f + 0.1f;

	m_aabb.SetMaxAndMin( Vector3(fMaxRadius,fMaxRadius,fMaxRadius), Vector3(-fMaxRadius,-fMaxRadius,-fMaxRadius) );

	// set my own my name so that we can create same smoke object one after another
	m_SmokeTrace.SetBaseEntityName( GetName() );
}


void CBE_SmokeTrace::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	InitParticleSetEntity( *pCopyEnt );

	ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );
//	ParticleSetExtraData& rParticleSet = pParticleSetEntity->m_ParticleSet;

	rParticleSet.iNumParticles = 0;

	int i;
	for( i=0; i<m_MaxNumParticlesPerSet; i++ )
	{
		rParticleSet.pasPattern[i] = RangedRand( m_iNumTextureSegments * m_iNumTextureSegments * 4 - 1 );
		rParticleSet.pafAnimationTime[i] = 0.0f;
	}

	Vector3& rvEmitterPrevPos = pCopyEnt->v1;	// this is supposed to be set by emitter entity
	Vector3& rvLastUpdatePos  = pCopyEnt->v2;
	rvLastUpdatePos = rvEmitterPrevPos;

	if( m_SmokeType == ST_PARTICLES_PER_DISTANCE )
        MakeDistanceBasedTrailParticles(pCopyEnt);
	else // i.e. m_SmokeType == ST_PARTICLES_PER_TIME
		MakeTimeBasedTrailParticles(pCopyEnt);
}


void CBE_SmokeTrace::Act( CCopyEntity* pCopyEnt )
{
	ProfileBegin( "trail particle" );

	if( m_SmokeType == ST_PARTICLES_PER_DISTANCE )
        MakeDistanceBasedTrailParticles(pCopyEnt);
	else // i.e. m_SmokeType == ST_PARTICLES_PER_TIME
		MakeTimeBasedTrailParticles(pCopyEnt);

	ProfileEnd( "trail particle" );

	if( !IsValidEntity(pCopyEnt) )
	{
		// terminated in MakeDistanceBasedTrailParticles() or MakeTimeBasedTrailParticles()
		// - calling UpdateVertexBuffer() will cause error since extra data index is now -1
		return;
	}


//	ProfileBegin( "trail particle (r)" );

	CCopyEntity *pChild = pCopyEnt->GetChild(0);
	if( pChild )
		pChild->pBaseEntity->Act( pChild );

//	ProfileEnd( "trail particle (r)" );

/*	if( m_Type == CBEC_Billboard::TYPE_BILLBOARDARRAYMESH )
	{
		// each copy entity (i.e. particle group) has its own paticles vertices
		UpdateVertexBuffer( pCopyEnt );
	}*/

	UpdateVertices( *pCopyEnt );
}


void CBE_SmokeTrace::MakeDistanceBasedTrailParticles( CCopyEntity* pCopyEnt )
{
	float fFrameTime = m_pStage->GetFrameTime();
	float& rfCurrentTime = CurrentTime(*pCopyEnt);
	float& rfTotalFrameTime = pCopyEnt->f4;
	CCopyEntity *pSmokeEmitter = pCopyEnt->GetParent();
	Vector3& rvLastUpdatePos = pCopyEnt->v2;
	float fParticleInterval = m_fParticleInterval;

	rfCurrentTime += fFrameTime;

	// get the external particle object for 'pCopyEnt'
	ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	int i, iNumCurrentParticles = rParticleSet.iNumParticles;

	if( 0 < iNumCurrentParticles && m_fDuration < rParticleSet.pafAnimationTime[iNumCurrentParticles-1]
	 || m_fDuration * 20.0f < rfCurrentTime )
	{
		rParticleSet.iNumParticles = 0;
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	if( m_bCreateParticleThread )
	{
		pCopyEnt->world_aabb.MergeAABB( rParticleSet.aabb );
	}
	else
	{
		if( m_bMinimumParticleUpdates )
		{
//			pCopyEnt->world_aabb.MergeAABB( aabb );
		}
		else
		{
			ProfileBegin( "UpdateParticles()" );
			// update the animation time of the currently displayed particles
//			UpdateParticles( pCopyEnt, fFrameTime );

			AABB3 aabb;
			aabb.Nullify();
			UpdateParticles( rParticleSet, fFrameTime, aabb );
			pCopyEnt->world_aabb.MergeAABB( aabb );
			ProfileEnd( "UpdateParticles()" );
		}
	}


	// update only if the accumulated frame time exceeds certain amount
	rfTotalFrameTime += fFrameTime;

	if( rfTotalFrameTime < 0.016f )
		return;
/*
	// changing to this makes smoke trail short. why?
	ProfileBegin( "UpdateParticles()" );
	// update the animation time of the currently displayed particles
	UpdateParticles( pCopyEnt, rfTotalFrameTime );
	ProfileEnd( "UpdateParticles()" );
*/

	if( !IsValidEntity( pSmokeEmitter ) || m_MaxNumParticlesPerSet <= iNumCurrentParticles )
	{
		return;	// no need to make any more particles
	}


	if( Vec3LengthSq( pSmokeEmitter->GetWorldPosition() - rvLastUpdatePos ) < fParticleInterval * fParticleInterval )
		return;	// emitter has not moved enough to generate new smoke particles
	else
		rvLastUpdatePos = pSmokeEmitter->GetWorldPosition();


//	ProfileBegin( "adding trail" );

	// calculate positions of new particles
	Vector3& rvEmitterPrevPos = pCopyEnt->v1;
	Vector3 vLinearTraceDir = pSmokeEmitter->GetWorldPosition() - rvEmitterPrevPos;
	float fTraceLength = Vec3Length( vLinearTraceDir ); // how far the emitter move during this frame
	Vec3Normalize( vLinearTraceDir, vLinearTraceDir );

	if( fTraceLength < 0.001f )
		return; // smoke emitter entity has not traveled enough distance to create smoke trail

	i = iNumCurrentParticles;
	int iNumNewParticles = (int)(fTraceLength / fParticleInterval) + 1;
	float& rfLeftTraceLength = pCopyEnt->f3;	// length of the uncovered part of the trace left since the last frame
	float fLastParticlePosition = rfLeftTraceLength;
	
	float fNewParticleAnimationTime = rfTotalFrameTime; //fFrameTime;
	float fAnimTimePerParticle = rfTotalFrameTime/*fFrameTime*/ / (float)iNumNewParticles;

	const float r = m_fSmokeTraceParticleRand;
	while(1)
	{
		// create new smoke particles

//		PROFILE_SCOPE( "create new trail particles" );

		if( m_MaxNumParticlesPerSet <= iNumCurrentParticles )
			break;

		rParticleSet.pavPosition[iNumCurrentParticles] = rvEmitterPrevPos + vLinearTraceDir * fLastParticlePosition;

		rParticleSet.pavPosition[iNumCurrentParticles]
			+= Vector3( RangedRand(-r, r), RangedRand(-r, r), RangedRand(-r, r) );

//		rParticleSet.pavVelocity[iNumCurrentParticles] = D3DXVECTOR3(0,0,0);
		rParticleSet.pavVelocity[iNumCurrentParticles] = Vec3RandDir() * 0.12f;

		fNewParticleAnimationTime -= fAnimTimePerParticle;
		rParticleSet.pafAnimationTime[iNumCurrentParticles] = fNewParticleAnimationTime;

		iNumCurrentParticles++;

		fLastParticlePosition += fParticleInterval;

		if( fTraceLength < fLastParticlePosition )
		{
			rfLeftTraceLength = fLastParticlePosition - fTraceLength;
			break;
		}
	}

//	ProfileEnd( "adding trail" );

	// update the number of the particles
	rParticleSet.iNumParticles = iNumCurrentParticles;


//	ProfileBegin( "creating a new trail entity" );

	if( m_MaxNumParticlesPerSet <= iNumCurrentParticles )
	{
		// release this entity from the emitter
		pCopyEnt->DisconnectFromParentAndChildren();

		// create a new smoke trace and connect it to the emitter
		CCopyEntityDesc next_trace;
		next_trace.pBaseEntityHandle = &m_SmokeTrace;
		next_trace.pParent = pSmokeEmitter;	// connect to the parent (smoke emitter)
		next_trace.SetWorldPosition( rParticleSet.pavPosition[m_MaxNumParticlesPerSet - 1] );
		next_trace.v1      = rParticleSet.pavPosition[m_MaxNumParticlesPerSet - 1];
		next_trace.f3      = rfLeftTraceLength;

		m_pStage->CreateEntity( next_trace );
	}

//	ProfileEnd( "creating a new trail entity" );

	rvEmitterPrevPos = pSmokeEmitter->GetWorldPosition();

	// update position - set the center position in the smoke trace
	pCopyEnt->SetWorldPosition( (rParticleSet.pavPosition[0] + rParticleSet.pavPosition[iNumCurrentParticles-1]) / 2.0f );

	// reset the accumulated frametime
	rfTotalFrameTime = 0;
}


void CBE_SmokeTrace::MakeTimeBasedTrailParticles( CCopyEntity* pCopyEnt )
{
	float fFrameTime = m_pStage->GetFrameTime();

	float& rfCurrentTime = CurrentTime(*pCopyEnt);
	float& rfTotalFrameTime = pCopyEnt->f4;
	CCopyEntity *pSmokeEmitter = pCopyEnt->GetParent();
//	float num_particles_per_sec = m_iNumParticlesPerSec;
	float particle_gen_rate = 1.0f / (float)m_iNumParticlesPerSec;

	rfCurrentTime += fFrameTime;

	// get the external particle object for 'pCopyEnt'
	ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );
//	ParticleSetExtraData& rParticleSet = pParticleSetEntity->m_ParticleSet;

	int iNumCurrentParticles = rParticleSet.iNumParticles;

	if( 0 < iNumCurrentParticles && m_fDuration < rParticleSet.pafAnimationTime[iNumCurrentParticles-1] ||
		iNumCurrentParticles == 0 && !IsValidEntity( pSmokeEmitter ) )
	{
///		pCopyEnt-Terminate();
		rParticleSet.iNumParticles = 0;
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	// update the animation time of the currently displayed particles
//	UpdateParticles( pCopyEnt, fFrameTime );
	AABB3 aabb;
	aabb.Nullify();
	UpdateParticles( rParticleSet, fFrameTime, aabb );
	pCopyEnt->world_aabb.MergeAABB( aabb );

	// update only if the accumulated frame time exceeds certain amount
	rfTotalFrameTime += fFrameTime;
	if( rfTotalFrameTime < 0.016f )
		return;

	if( 0 < iNumCurrentParticles )
	{
		// update position - set the center position in the smoke trace

		// results in wrong visibility state
		// not rendered at certain locations
//		pCopyEnt->GetWorldPosition() = (rParticleSet.pavPosition[0] + rParticleSet.pavPosition[iNumCurrentParticles-1]) / 2.0f;

		// not a correct position update
		// used to cope with the visibility problem caused by the above update code
		if( pSmokeEmitter )
            pCopyEnt->SetWorldPosition( pSmokeEmitter->GetWorldPosition() );

		Vector3 vSpan = (rParticleSet.pavPosition[0] - rParticleSet.pavPosition[iNumCurrentParticles-1]) / 2.0f;
		const float r = m_fParticleRadius;
		vSpan.x = fabsf( vSpan.x ) + r;
		vSpan.y = fabsf( vSpan.y ) + r;
		vSpan.z = fabsf( vSpan.z ) + r;
		pCopyEnt->local_aabb.vMax =  vSpan * 1.0f;
		pCopyEnt->local_aabb.vMin = -vSpan * 1.0f;
		pCopyEnt->fRadius = Vec3Length( vSpan ) + r;
	}

	if( !IsValidEntity( pSmokeEmitter ) || m_MaxNumParticlesPerSet <= iNumCurrentParticles )
	{
		return;	// no need to make any more particles
	}

	Vector3 vSourcePos = pSmokeEmitter->GetWorldPosition();

	float frametime_left = rfTotalFrameTime;

	while( particle_gen_rate < frametime_left )
	{
		if( m_MaxNumParticlesPerSet <= iNumCurrentParticles )
			break;

		frametime_left -= particle_gen_rate;

		rParticleSet.pavPosition[iNumCurrentParticles] = vSourcePos;

		rParticleSet.pavPosition[iNumCurrentParticles]
			+= Vector3( RangedRand(-0.06f, 0.06f), RangedRand(-0.06f, 0.06f), RangedRand(-0.06f, 0.06f) );

		rParticleSet.pavVelocity[iNumCurrentParticles] = Vector3(0,0,0);

//		fNewParticleAnimationTime -= fAnimTimePerParticle;
//		rParticleSet.pafAnimationTime[iNumCurrentParticles] = fNewParticleAnimationTime;
		rParticleSet.pafAnimationTime[iNumCurrentParticles] = 0;


		iNumCurrentParticles++;
	}

	rfTotalFrameTime = frametime_left;


	// update the number of the particles
	rParticleSet.iNumParticles = iNumCurrentParticles;

	if( m_MaxNumParticlesPerSet <= iNumCurrentParticles )
	{
		// release this entity from the emitter
		pCopyEnt->DisconnectFromParentAndChildren();

		// create a new smoke trace and connect it to the emitter
		CCopyEntityDesc next_smoke;
		next_smoke.SetDefault();

		next_smoke.pBaseEntityHandle = &m_SmokeTrace;
		next_smoke.pParent = pSmokeEmitter;	// connect to the parent (smoke emitter)
//		next_smoke.vPosition = rParticleSet.pavPosition[m_MaxNumParticlesPerSet - 1];
		next_smoke.SetWorldPosition( pSmokeEmitter->GetWorldPosition() );
		next_smoke.vVelocity = Vector3(0,0,0);
		next_smoke.v1 = rParticleSet.pavPosition[m_MaxNumParticlesPerSet - 1];
		next_smoke.f4 = rfTotalFrameTime;
		m_pStage->CreateEntity( next_smoke );
	}

}


void CBE_SmokeTrace::Draw(CCopyEntity* pCopyEnt)
{
	DrawParticles( pCopyEnt );
	return;
}


bool CBE_SmokeTrace::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_ParticleSet::LoadSpecificPropertiesFromFile(scanner);

	string smoke_type;

	if( scanner.TryScanLine( "PTCL_INTVL",		m_fParticleInterval ) ) return true;
	if( scanner.TryScanLine( "PTCLS_PER_SEC",	m_iNumParticlesPerSec ) ) return true;
	if( scanner.TryScanLine( "RAND_POS_SHIFT",	m_fSmokeTraceParticleRand ) ) return true;

	if( scanner.TryScanLine( "SMOKE_TYPE", smoke_type ) )
	{
		if( smoke_type == "PARTICLES_PER_TIME" )			m_SmokeType = ST_PARTICLES_PER_TIME;
		else if( smoke_type == "PARTICLES_PER_DISTANCE" )	m_SmokeType = ST_PARTICLES_PER_DISTANCE;

		return true;
	}

	return false;
}


void CBE_SmokeTrace::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );

	ar & m_fParticleInterval;
	ar & m_iNumParticlesPerSec;
	ar & m_SmokeType;
	ar & m_fSmokeTraceParticleRand;
}


} // namespace amorphous
