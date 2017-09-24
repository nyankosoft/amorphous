#include "HumanoidMotionSynthesizer.hpp"
#include "MotionBlender.hpp"
#include "MotionPrimitiveBlender.hpp"
#include "MotionDatabase.hpp"
#include "HumanoidMotionTable.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/memory_helpers.hpp"

#include <boost/foreach.hpp>


namespace amorphous
{

using namespace boost;

using namespace msynth;


CHumanoidMotionSynthesizer::CHumanoidMotionSynthesizer()
{
	m_pMotionPrimitiveBlender
		= std::shared_ptr<MotionPrimitiveBlender>( new MotionPrimitiveBlender() );

	m_pSteeringMotionBlender
		= std::shared_ptr<SteeringMotionBlender>( new SteeringMotionBlender( m_pMotionPrimitiveBlender ) );

	m_vecpMotionBlender.push_back( m_pSteeringMotionBlender );
	m_vecpMotionBlender.push_back( m_pMotionPrimitiveBlender );


	m_pMotionPrimitiveBlenderStatics
		= std::shared_ptr<MotionPrimitiveBlenderStatistics>( new MotionPrimitiveBlenderStatistics( m_pMotionPrimitiveBlender.get() ) );
}


CHumanoidMotionSynthesizer::~CHumanoidMotionSynthesizer()
{
}


void CHumanoidMotionSynthesizer::ProcessMotionTransitionRequest( HumanoidMotion::Type motion_type )
{
	// get walk motion
	CHumanoidMotionHolder & motion_holder = m_aMotion[motion_type];
	if( motion_holder.m_vecpMotion.size() == 0 )
		return;

	shared_ptr<MotionPrimitive> pTargetMotion = motion_holder.m_vecpMotion[0];

	if( pTargetMotion )
	{
		// peek the current motion
		shared_ptr<MotionPrimitive> pCurrentMotion
			= m_pMotionPrimitiveBlender->GetCurrentMotionPrimitive();

		if( !pCurrentMotion->IsEmpty() &&
			pCurrentMotion->HasAnnotation( GetAnnotation(motion_type) ) )
		{
			// already using the target motion
			// - no need to add motion primitive as long as the current walk motion
			//   is looped
			int keep_current_motion = 1;
		}
		else
		{
			m_pMotionPrimitiveBlender->StartNewMotionPrimitive( pTargetMotion );
		}
	}
}


void CHumanoidMotionSynthesizer::Forward()
{
}


void CHumanoidMotionSynthesizer::Walk()
{
	ProcessMotionTransitionRequest( HumanoidMotion::Walk );
}


void CHumanoidMotionSynthesizer::Stop()
{
	ProcessMotionTransitionRequest( HumanoidMotion::Stop );
}


void CHumanoidMotionSynthesizer::TurnTo( const Vector3 vDir )
{
	m_pSteeringMotionBlender->SetDestDirection( vDir );
}


void CHumanoidMotionSynthesizer::Idle()
{
}

void CHumanoidMotionSynthesizer::TurnRight()
{
}

void CHumanoidMotionSynthesizer::TurnLeft()
{
}


void CHumanoidMotionSynthesizer::Turn( Scalar turn_speed )
{
	m_pSteeringMotionBlender->SetSteeringSpeed( turn_speed );
}


void CHumanoidMotionSynthesizer::LoadMotions( MotionDatabase& mdb, const std::string& motion_table_key_name )
{
	HumanoidMotionTable tbl;

	bool retrieved = mdb.GetHumanoidMotionTable( motion_table_key_name, tbl );

	if( !retrieved )
	{
		LOG_PRINT_ERROR( "Cannot find the motion table: " + motion_table_key_name );
	}

	std::map<std::string,int> motion_type_to_id;
	motion_type_to_id["Walk"]   = HumanoidMotion::Walk;
	motion_type_to_id["Run"]    = HumanoidMotion::Run;
	motion_type_to_id["Stop"]   = HumanoidMotion::Stop;
	motion_type_to_id["Jump"]   = HumanoidMotion::Jump;
	motion_type_to_id["Crawl"]  = HumanoidMotion::Crawl;
	motion_type_to_id["Crouch"] = HumanoidMotion::Crouch;

	BOOST_FOREACH( const CHumanoidMotionEntry& entry, tbl.m_vecEntry )
	{
		// get array index from motion type name
		std::map<std::string,int>::iterator itr = motion_type_to_id.find(entry.m_MotionType);

		if( itr != motion_type_to_id.end() )
		{
			int type_index = itr->second;
			
			BOOST_FOREACH( const std::string& motion_primitive_name, entry.m_vecMotionPrimitiveName )
			{
				shared_ptr<MotionPrimitive> pMotion = mdb.GetMotionPrimitive( motion_primitive_name );
				if( pMotion )
					m_aMotion[type_index].m_vecpMotion.push_back( pMotion );
			}
		}
	}

	// initialize annotation objects
	m_aAnnotation[HumanoidMotion::Walk]      = mdb.GetAnnotation( "Walk" );
	m_aAnnotation[HumanoidMotion::Run]       = mdb.GetAnnotation( "Run" );
	m_aAnnotation[HumanoidMotion::Turn]      = mdb.GetAnnotation( "Turn" );
	m_aAnnotation[HumanoidMotion::Stop]      = mdb.GetAnnotation( "Stop" );
	m_aAnnotation[HumanoidMotion::Jump]      = mdb.GetAnnotation( "Jump" );
	m_aAnnotation[HumanoidMotion::Crawl]     = mdb.GetAnnotation( "Crawl" );
	m_aAnnotation[HumanoidMotion::Crouch]    = mdb.GetAnnotation( "Crouch" );
	m_aAnnotation[HumanoidMotion::TurnRight] = mdb.GetAnnotation( "TurnRight" );
	m_aAnnotation[HumanoidMotion::TurnLeft]  = mdb.GetAnnotation( "TurnLeft" );

	// copy transform node tree structure to m_CurrentKeyframe
	// - This has to be a motion primitive that holds the complete tree of skeleton structure
	Keyframe default_keyframe;
	size_t i;
	for( i=0; i<HumanoidMotion::NumActions; i++ )
	{
		if( 0 < m_aMotion[i].m_vecpMotion.size() )
		{
			m_aMotion[HumanoidMotion::Walk].m_vecpMotion[0]->GetFirstKeyframe( default_keyframe );
			
			m_CurrentKeyframe = default_keyframe;

			m_pMotionPrimitiveBlender->SetDefaultKeyframe( default_keyframe );

			break;
		}
	}

	if( i == HumanoidMotion::NumActions )
		LOG_PRINT_WARNING( " - No motion primitive to create base skeleton tree" );

//	BOOST_FOREACH( const CHumanoidMotionEntry& entry, m_aAnnotation )
//	{
//	}
}


void CHumanoidMotionSynthesizer::Update( float dt )
{
	BOOST_FOREACH( std::shared_ptr<MotionBlender>& pMotionBlender, m_vecpMotionBlender )
	{
		pMotionBlender->Update( dt );
//		pMotionBlender->CalculateKeyframe( dt, m_CurrentKeyframe );
	}

	const std::shared_ptr<MotionPrimitive>& pCurrentMotion = m_pMotionPrimitiveBlender->GetCurrentMotionPrimitive();

	if( pCurrentMotion->IsEmpty() )
	{
		// add stop motion
		Stop();
	}


	m_pMotionPrimitiveBlenderStatics->Update();
}


/*
void CHumanoidMotionSynthesizer::CalculateKeyframe( Keyframe& dest_keyframe )
{
	BOOST_FOREACH( std::shared_ptr<MotionBlender>& pMotionBlender, m_vecpMotionBlender )
	{
		pMotionBlender->CalculateKeyframe( dest_keyframe );
	}

}
*/


void CHumanoidMotionSynthesizer::UpdateKeyframe()
{
	BOOST_FOREACH( std::shared_ptr<MotionBlender>& pMotionBlender, m_vecpMotionBlender )
	{
		pMotionBlender->CalculateKeyframe( m_CurrentKeyframe );
	}
}


// const Skeleton& CHumanoidMotionSynthesizer::GetSkeleton()
const std::shared_ptr<Skeleton> CHumanoidMotionSynthesizer::GetSkeleton()
{
	shared_ptr<MotionPrimitive> pCurrentMotion
		= m_pMotionPrimitiveBlender->GetCurrentMotionPrimitive();

	// GetCurrentMotionPrimitive() returns an empty motion primitive
	// if no motion primitives are in the queue
	// - no null pointer check

	return pCurrentMotion->GetSkeleton();
}


void CHumanoidMotionSynthesizer::SetRootPose( const Matrix34& pose )
{
	m_pMotionPrimitiveBlender->SetRootPose( pose );
}


/*



void CHumanoidMotionSynthesizer::Walk()
{
	// get walk motion
	CHumanoidMotionHolder & motion_holder = m_aMotion[HumanoidMotion::Walk];
	if( motion_holder.m_vecpMotion.size() == 0 )
		return;

	shared_ptr<MotionPrimitive> pWalkMotion = motion_holder.m_vecpMotion[0];

	if( pWalkMotion )
	{
		// peek the current motion
		shared_ptr<MotionPrimitive> pCurrentMotion
			= m_pMotionPrimitiveBlender->GetCurrentMotionPrimitive();

		if( !pCurrentMotion->IsEmpty() &&
			pCurrentMotion->HasAnnotation( GetAnnotation(HumanoidMotion::Walk) ) )
		{
			// already walking
			// - no need to add motion primitive as long as the current walk motion
			//   is looped
		}
		else
		{
			float dt = 0.033333f;
			m_pMotionPrimitiveBlender->StartNewMotionPrimitive( dt, pWalkMotion );
		}
	}
}

*/


} // namespace amorphous
