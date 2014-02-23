#include "GameCommon/RandomDirectionTable.hpp"
#include "Physics/Actor.hpp"
#include "Support/MTRand.hpp"
#include "Support/VectorRand.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"
#include "Stage/BEC_Destruction.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/Stage.hpp"


namespace amorphous
{

using namespace physics;


void CBEC_Destruction::CreateExplosion( CCopyEntity& entity, CStage *pStage )
{
	if( 0 < strlen(m_Explosion.GetBaseEntityName()) )
	{
		// create explosion - usu. done by texture animation
		CCopyEntityDesc desc;
		desc.pBaseEntityHandle = &m_Explosion;
		desc.SetWorldPose( entity.GetWorldPose() );
		desc.vVelocity         = Vector3(0,0,0);

		pStage->CreateEntity( desc );
	}
}


void CBEC_Destruction::CreateFragments( CCopyEntity& entity,
									    CStage *pStage,
										int num_max_fragments )
{
	CCopyEntityDesc desc;

	// avoid overlaps with fragment entities
	entity.bNoClip = true;

	Vector3 vWorldFragmentOffset;
	CCopyEntity* pFragEntity = NULL;

	// create fragments
	const size_t num_fragments = m_vecFragment.size();
	for( size_t i=0; i<num_fragments; i++ )
	{
		const Fragment& frag = m_vecFragment[i];

		desc.pBaseEntityHandle = &m_vecFragment[i].BaseEntity;

		entity.GetWorldPose().Transform( vWorldFragmentOffset, frag.vLocalPosition );
		desc.WorldPose.vPosition = vWorldFragmentOffset;
		desc.SetWorldOrient( entity.GetWorldPose().matOrient );
		desc.vVelocity  = Vec3RandDir() * RangedRand( frag.InitSpeed.min, frag.InitSpeed.max )
				         + entity.Velocity();

		pFragEntity = pStage->CreateEntity( desc );

		if( pFragEntity )//&& pFragEntity->pPhysicsActor )
		{
			physics::CActor *pPhysicsActor = pFragEntity->GetPrimaryPhysicsActor();
			if( pPhysicsActor )
				pPhysicsActor->SetAngularVelocity( Vec3RandDir() * RangedRand( 0.5f, 8.0f ) );
		}
	}
}


void CBEC_Destruction::CreateTrails( CCopyEntity& entity, CStage *pStage )
{
	CCopyEntityDesc desc;

	const size_t num_trails = m_vecTrail.size();
	for( size_t i=0; i<num_trails; i++ )
	{
		const Fragment& trail = m_vecTrail[i];

		desc.SetDefault();

		desc.pParent = &entity;

		desc.pBaseEntityHandle = &m_vecTrail[i].BaseEntity;
		desc.WorldPose.vPosition         = entity.GetWorldPose() * trail.vLocalPosition;

		Vector3& rvEmitterPrevPos = desc.v1;
		rvEmitterPrevPos = desc.WorldPose.vPosition;

		float& rfLeftTraceLength = desc.f3;
		rfLeftTraceLength = 0.0f;

		float& rfCurrentTime = desc.f2;
		rfCurrentTime = 0.0f;

		pStage->CreateEntity( desc );
	}
}


bool CBEC_Destruction::LoadFromFile( CTextFileScanner& scanner )
{
	std::string str_name;

	if( scanner.TryScanLine( "EXPLOSION", str_name ) )
	{
		m_Explosion.SetBaseEntityName( str_name.c_str() );
		return true;
	}

	Vector3 vLocalPosition = Vector3(0,0,0), vRange = Vector3( 15.5f, 35.5f, 0.0f );
	if( scanner.TryScanLine( "FRAGMENT", str_name, vLocalPosition, vRange ) )
	{
		Fragment frag;

		frag.BaseEntity.SetBaseEntityName( str_name.c_str() );
		frag.vLocalPosition          = vLocalPosition;
		frag.InitSpeed.min = vRange.x;
		frag.InitSpeed.max = vRange.y;

		m_vecFragment.push_back( frag );

		return true;
	}

	return false;
}


} // namespace amorphous
