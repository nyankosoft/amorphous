
#include "JL_PhysicsManager.hpp"

#include "JL_CollisionFunctor.hpp"
#include "JL_CollisionDetect.hpp"

#include "JL_ShapeDesc_TriangleMesh.hpp"
#include "JL_PhysicsActorDesc.hpp"

#include "GravityFunction.hpp"

using namespace std;

// limit the extra velocity during collision/penetration calculations
static const Scalar max_vel_mag = 30.0f;
//static const Scalar maxVelMag = 0.5f;	// jiglib-0.70
static const Scalar MaxShockVelMag = 0.05f;
static const Scalar minVelForProcessing = 0.01f;
static const Scalar penetrationRelaxationTimesteps = 3;
static const Scalar penetrationShockRelaxationTimesteps = 20;


#define DO_FRICTION


//==============================================================
// Physics
//==============================================================
CJL_PhysicsManager::CJL_PhysicsManager()
{
///	m_timestep = 1.0f / 120;
//	m_timestep = 1.0f / 80;
//	m_timestep = 1.0f / 100;

//	m_overlap_time = 0.0f;

//	m_NumCollisionIterations = 5;		// original
//	m_NumCollisionIterations = 8;		// larger boxes need more iterations
	m_NumCollisionIterations = 4;		// jiglib 0.70 default

//	m_NumContactIterations = 10;		// original
	m_NumContactIterations = 12;		// jiglib 0.70 default

//	m_num_penetration_iterations = 10;	// original
//	m_num_penetration_iterations = 5;
//	m_penetration_resolve_fraction = 0.008f;
//	m_reuse_collisions = false;

	m_CollTolerance = 0.05f;

	m_traverse_dir = TRAVERSE_FORWARD;

//	m_allow_smaller_timesteps = true;

	m_FreezingEnabled = true;

//	m_time_scale = 0.8f;
	m_time_scale = 1.0f;

//	m_DoShockStep = true;
	m_DoShockStep = false;

//	m_ProcessCollisionFn = &CJL_PhysicsManager::ProcessCollision;
	m_ProcessCollisionFn = &CJL_PhysicsManager::ProcessCollisionCombined;


	m_vGravity = Vector3( 0.0f, -9.8f, 0.0f );

	// set a default material
	m_vecMaterial.push_back( CJL_SurfaceMaterial() );


	m_ActorList.Init(128);

	m_BoxShape.Init(64);
	m_CapsuleShape.Init(64);
	m_TriangleMesh.Init(2);

	// enable collisions between all the collision groups (default)
	int i, j;
	for( i=0; i<NUM_MAX_COLLISION_GROUPS; i++ )
	{
		for( j=0; j<NUM_MAX_COLLISION_GROUPS; j++ )
		{
			m_CollisionGroupTable[i][j] = 1;
		}
	}
}


void CJL_PhysicsManager::Init()
{
}


//==============================================================
// add_constraint
//==============================================================
void CJL_PhysicsManager::AddConstraint(Constraint * constraint)
{
//  m_constraints.push_back(constraint);
}


//==============================================================
// add_controller
//==============================================================
void CJL_PhysicsManager::AddController(Physics_controller * controller)
{
//  m_controllers.push_back(controller);
}


//==============================================================
// ActivateObject
// activate object and bring it into the collision list/do collision detection
//==============================================================
void CJL_PhysicsManager::ActivateObject(CJL_PhysicsActor *pActor)
{
	assert( pActor != NULL );

	if ( (pActor->GetActivityState() == CJL_PhysicsActor::ACTIVE) || (pActor->GetActorFlag() & JL_ACTOR_STATIC) )
	{
		return;
	}

	pActor->SetActivityState(CJL_PhysicsActor::ACTIVE, 1.0f);

  int orig_num = m_vecCollision.size();

//  if (!body->collision_body()) return;

//  ::detect_frozen_collisions_with_body(body->collision_body(), m_collision_bodies, m_heightmaps, m_vecCollision);
//  m_collision_bodies.push_back(body->collision_body());

  CJL_CollisionFunctor collision_functor;
  collision_functor.SetCollisionBuffer( &m_vecCollision );

  // collision tolerance - make sure we get things above us?
  collision_functor.m_fCollTolerance = m_CollTolerance + 0.01f;

  JL_CollisionDetect_FrozenActor( pActor, m_ActorList, collision_functor );
//  m_collision_bodies.push_back(body->collision_body());


  // now check that any adjacent touching bodies wouldn't accelerate towards us if we moved away
  int new_num = m_vecCollision.size();
  int i;
  for (i = orig_num ; i < new_num ; ++i)
  {
    // must be a body-body interaction to be interesting
    if (m_vecCollision[i].pBody1)
    {
      CJL_PhysicsActor *pOtherActor = m_vecCollision[i].pBody0->GetPhysicsActor();

      // the collision normal pointing from 'pActor' to 'pOtherActor'
      Vector3 vThisBodyNormal = m_vecCollision[i].m_vDirToBody0;
//      Vector3 vThisBodyNormal = m_vecCollision[i].vNormal;
      if (pOtherActor == pActor)
      {
        pOtherActor = m_vecCollision[i].pBody1->GetPhysicsActor();
        vThisBodyNormal *= -1;
      }
      if (pOtherActor->GetActivityState() == CJL_PhysicsActor::FROZEN)
      {
        // remember that the Rigid_body doesn't apply gravity to sleeping bodies
        Vector3 force_on_other = pOtherActor->GetMass() * /*get_gravity()*/ Vector3(0,-9.8f,0) + pOtherActor->GetForce();
        if ( Vec3Dot(force_on_other, vThisBodyNormal) <= 0.0f )
        {
          // wake it up recursively. after this, the contents of our m_vecCollision may have been relocated
          ActivateObject(pOtherActor);
        }
      }
    }
  }
}


static const Scalar MaxVelMag = 20.0f;
static const Scalar PenetrationRelaxationTimesteps = 3;

//==============================================================
// PreProcessCollision
//==============================================================
inline void CJL_PhysicsManager::PreProcessCollision(CJL_CollisionInfo & collision, Scalar dt)
//inline void CJL_PhysicsManager::PreProcessCollision(CJL_ContactInfo & collision, Scalar dt)
{
	CJL_ShapeBase *pShape0 = collision.pBody0;
	CJL_ShapeBase *pShape1 = collision.pBody1;

	CJL_PhysicsActor *pActor0 = pShape0->GetPhysicsActor();
	CJL_PhysicsActor *pActor1 = NULL;
	CJL_SurfaceMaterial& rMaterial0 = m_vecMaterial[ pShape0->GetMaterialIndex() ];

	if( pShape1 )
		pActor1 = pShape1->GetPhysicsActor();

	// mark as not satisfied
	collision.m_Satisfied = false;

	// always calc the following
	const Vector3 & N = collision.m_vDirToBody0;
//	const Vector3 & N = collision.vNormal; // dir_to_0
	const Scalar timescale = PenetrationRelaxationTimesteps * dt;


	for( int i=0; i<collision.m_vecPointInfo.size(); i++ )
//	for( int i=0; i<1; i++ )
	{
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[i];

		Vector3 v;
		// some things we only calculate if there are bodies, and they are movable
		if( pActor0->GetActorFlag() & JL_ACTOR_STATIC )
		{
			pt_info.fDenominator = 0.0f;
		}
		else
		{
			v = pActor0->GetWorldInvInertia() * Vec3Cross( pt_info.vR0, N );

			pt_info.fDenominator = pActor0->GetInvMass()
				                 + Vec3Dot( N, Vec3Cross( v, pt_info.vR0 ) );
		}

		if( pActor1 && !(pActor1->GetActorFlag() & JL_ACTOR_STATIC) )
		{
			v = pActor1->GetWorldInvInertia() * Vec3Cross( pt_info.vR1, N );

			pt_info.fDenominator += pActor1->GetInvMass()
				                  + Vec3Dot( N, Vec3Cross( v, pt_info.vR1 ) );
		}
		else
			int collision_against_static_actor = 1;

		if( pt_info.fDenominator < SCALAR_TINY )
			pt_info.fDenominator = SCALAR_TINY;

		pt_info.vContactPosition = pActor0->GetOldPosition() + pt_info.vR0;
//		pt_info.vContactPoint = pActor0->GetOldPosition() + pt_info.vR0;

		// per-point penetration resolution
		if( 0.0f < pt_info.InitialPenetration )
			pt_info.fMinSeparationVel = pt_info.InitialPenetration / timescale;
		else
			pt_info.fMinSeparationVel = pt_info.InitialPenetration / dt;

		if( MaxVelMag < pt_info.fMinSeparationVel )
			pt_info.fMinSeparationVel = MaxVelMag;
	}

	// surface materials
	if( pShape1 )
	{
		CJL_SurfaceMaterial& rMaterial1 = m_vecMaterial[ pShape1->GetMaterialIndex() ];

		collision.fElasticity      = rMaterial0.fElasticity * rMaterial1.fElasticity;
		collision.fStaticFriction  = 0.5f * (rMaterial0.fStaticFriction + rMaterial1.fStaticFriction);
		collision.fDynamicFriction = 0.5f * (rMaterial0.fDynamicFriction + rMaterial1.fDynamicFriction);
	}
	else
	{	// collision with movable actor (pActor0) and static actor
		CJL_SurfaceMaterial& rMaterial1 = m_vecMaterial[ collision.Body1MaterialIndex ];
//		CJL_SurfaceMaterial& rMaterial1 = m_vecMaterial[ collision.sStaticGeometryMaterialIndex ];
		collision.fElasticity      = rMaterial0.fElasticity * rMaterial1.fElasticity;
		collision.fStaticFriction  = 0.5f * ( rMaterial0.fStaticFriction + rMaterial1.fStaticFriction );
		collision.fDynamicFriction = 0.5f * ( rMaterial0.fDynamicFriction + rMaterial1.fDynamicFriction );
	}
}


static const Scalar MinVelForProcessing = 0.01f;

//==============================================================
// ProcessCollision
//==============================================================
bool CJL_PhysicsManager::ProcessCollision( CJL_CollisionInfo & collision, Scalar dt )
//bool CJL_PhysicsManager::ProcessCollision( CJL_ContactInfo & collision, Scalar dt )
{
	collision.m_Satisfied = true;

//	if (collision.fDenominator < 0.0001f)
//		return false;

	CJL_ShapeBase *pShape0 = collision.pBody0;
	CJL_ShapeBase *pShape1 = collision.pBody1;

	CJL_PhysicsActor *pActor0 = NULL, *pActor1 = NULL;
	if( pShape0 )
		pActor0 = pShape0->GetPhysicsActor();
	if( pShape1 )
		pActor1 = pShape1->GetPhysicsActor();

	bool gotOne = false;

	const Vector3& N = collision.m_vDirToBody0; //dir_to_0
//	const Vector3& N = collision.vNormal; //dir_to_0

	Vector3 vPointVelocity0, vPointVelocity1;
	Scalar normal_vel;

	for( int i=0; i<collision.m_vecPointInfo.size(); i++ )
//	for( int i=0; i<1; i++ )
	{
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[i];

		assert( SCALAR_TINY <= pt_info.fDenominator );

		// point velocity of body 0
		pActor0->GetLocalPointVelocity( vPointVelocity0, pt_info.vR0 ); 

		if( pActor1 )
		{
			pActor1->GetLocalPointVelocity( vPointVelocity1, pt_info.vR1 ); 
			normal_vel = Vec3Dot( vPointVelocity0 - vPointVelocity1, N );
		}
		else
		{
			normal_vel = Vec3Dot( vPointVelocity0, N );
		}

		if( pt_info.fMinSeparationVel < normal_vel )
			continue;

		Scalar final_normal_vel = - collision.fElasticity * normal_vel;

		if( final_normal_vel < MinVelForProcessing )
		{
			// could be zero elasticity in collision, or could be zero
			// elasticity in contact - don't care. relax towards 0
			// penetration
			final_normal_vel = pt_info.fMinSeparationVel;
		}

		Scalar delta_vel = final_normal_vel - normal_vel;

		if( delta_vel <= MinVelForProcessing )
			continue;

		Scalar normal_impulse = delta_vel / pt_info.fDenominator;

		// prepare our return value
		gotOne = true;
		Vector3 vImpulse = normal_impulse * N;

		if( !(pActor0->GetActorFlag() & JL_ACTOR_APPLY_NO_IMPULSE) )
			pActor0->ApplyBodyWorldImpulse( vImpulse, pt_info.vR0 );

		if( pActor1 && !(pActor1->GetActorFlag() & JL_ACTOR_APPLY_NO_IMPULSE) )
			pActor1->ApplyBodyWorldImpulse( -vImpulse, pt_info.vR1 );


		// For friction, work out the impulse in the opposite direction to the tangential
		// velocity that would be required to bring this point to a halt. Apply the minimum 
		// of this impulse magnitude, and the one obtained from the normal impulse. This 
		// prevents reversing the velocity direction.
		//
		// However, recalculate the velocity since it's changed. 

		Vector3 v;
//		Vec3Cross( &v, &pActor0->GetAngularVelocity(), &collision.vR0 );
//		Vector3 Vr_new = pActor0->GetVelocity() + v;
		Vector3 Vr_new;
		pActor0->GetLocalPointVelocity( Vr_new, pt_info.vR0 );

		if( pActor1 )
		{
			pActor1->GetLocalPointVelocity( v, pt_info.vR1 );
			Vr_new -= v;
//			Velocity Vr_av = 0.5f * (Vr + Vr_new);
		}

		Vector3 tangent_vel = Vr_new - Vec3Dot(Vr_new,N) * N;
		Scalar tangent_speed = Vec3Length( tangent_vel );

		if( tangent_speed > SCALAR_TINY )
		{
			Vector3 T = -tangent_vel / tangent_speed;

			// epsilon = 0
			Scalar numerator = tangent_speed;

			Scalar denominator;

			denominator = pActor0->GetInvMass();
			v = Vec3Cross( pt_info.vR0, T );
			v = pActor0->GetWorldInvInertia() * v;
			v = Vec3Cross( v, pt_info.vR0 );
			denominator += Vec3Dot( T,  v );
			if( pActor1 )
			{
				denominator += pActor1->GetInvMass();
				v = Vec3Cross( pt_info.vR1, T);
				v = pActor1->GetWorldInvInertia() * v;
				v = Vec3Cross( v, pt_info.vR1 );
				denominator += Vec3Dot( T,  v );
			}

			if( denominator > SCALAR_TINY )
			{
				Scalar impulse_to_reverse = numerator / denominator;
				Scalar static_friction = collision.fStaticFriction;
				Scalar dynamic_friction = collision.fDynamicFriction;

				Scalar impulse_from_normal_impulse = static_friction * normal_impulse;
				Scalar friction_impulse;

				if (impulse_to_reverse < impulse_from_normal_impulse)
					friction_impulse = impulse_to_reverse;
				else
					friction_impulse = dynamic_friction * normal_impulse;

				pActor0->ApplyBodyWorldImpulse(friction_impulse * T, pt_info.vR0);
				
				if (pActor1)
					pActor1->ApplyBodyWorldImpulse((-friction_impulse) * T, pt_info.vR1);
			}
		} // end of friction
	}

	if( gotOne )
	{
		pActor0->SetConstraintsAndCollisionsUnsatisfied();
		if( pActor1 )
			pActor1->SetConstraintsAndCollisionsUnsatisfied();
	}

	return gotOne;
}


//==============================================================
// ProcessCollisionCombined
//==============================================================
bool CJL_PhysicsManager::ProcessCollisionCombined( CJL_CollisionInfo& collision,
												   Scalar dt )
{
	collision.m_Satisfied = true;

	CJL_ShapeBase *pShape0 = collision.pBody0;
	CJL_ShapeBase *pShape1 = collision.pBody1;
	CJL_PhysicsActor *pActor0 = NULL, *pActor1 = NULL;

	if( pShape0 )
		pActor0 = pShape0->GetPhysicsActor();

	if( pShape1 )
		pActor1 = pShape1->GetPhysicsActor();

	assert( pActor0 != NULL );

	const Vector3& N = collision.m_vDirToBody0;

	// the individual impulses in the same order as
	// collision->m_vecPointInfo - for friction
	Scalar total_impulse = 0.0f;
	unsigned iPos;

	Vector3 vAvePos = Vector3(0,0,0);
	Scalar AveMinSeparationVel = 0.0f;

	Scalar impulses[CJL_CollisionInfo::MAX_COLLISION_POINTS];

	for( iPos=collision.m_vecPointInfo.size(); iPos-- != 0 ; )
	{
		assert( SCALAR_TINY <= collision.m_vecPointInfo[iPos].fDenominator );
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];
		impulses[iPos] = 0.0f;

		Scalar normal_vel;
		Vector3 vPointVel0 = Vector3(0,0,0), vPointVel1 = Vector3(0,0,0);

		pActor0->GetLocalPointVelocity( vPointVel0, pt_info.vR0 );
		if( pActor1 )
			pActor1->GetLocalPointVelocity( vPointVel1, pt_info.vR1 );

		normal_vel = Vec3Dot( vPointVel0 - vPointVel1, N );

		if( pt_info.fMinSeparationVel < normal_vel )
			continue;

		Scalar final_normal_vel = - collision.fElasticity * normal_vel;

		if( final_normal_vel < MinVelForProcessing )
		{
			// could be zero elasticity in collision, or could be zero
			// elasticity in contact - don't care. relax towards 0
			// penetration
			final_normal_vel = pt_info.fMinSeparationVel;
		}

		Scalar delta_vel = final_normal_vel - normal_vel;

		if( delta_vel <= MinVelForProcessing )
			continue;

		Scalar normal_impulse = delta_vel / pt_info.fDenominator;

		impulses[iPos] = normal_impulse;
		total_impulse += normal_impulse;

		vAvePos += normal_impulse * pt_info.vContactPosition;
		AveMinSeparationVel += pt_info.fMinSeparationVel * normal_impulse;
	}

	if( total_impulse < SCALAR_TINY )
		return false;

	const Scalar scale = 1.0f / total_impulse;

	// apply all these impulses (as well as subsequently applying an impulse
	// at an averaged position)
	for( iPos=collision.m_vecPointInfo.size(); iPos-- != 0 ; )
	{
		if( SCALAR_TINY < impulses[iPos] )
		{
			CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];
			const Scalar sc = impulses[iPos] * scale;
			const Vector3 vImpulse = impulses[iPos] * sc * N;

			pActor0->ApplyBodyWorldImpulse( vImpulse, pt_info.vR0 );
			if( pActor1 )
				pActor1->ApplyBodyWorldImpulse( -vImpulse, pt_info.vR1 );
		}
	}

	vAvePos *= scale;
	AveMinSeparationVel *= scale;

	// now calculate the single impulse to be applied at vAvePos
	Vector3 vR0, vR1, Vr;
	vR0 = vAvePos - pActor0->GetPosition();

	pActor0->GetLocalPointVelocity( Vr, vR0 );
	if( pActor1 )
	{
		vR1 = vAvePos - pActor1->GetPosition();
		Vector3 vPointVel1;
		pActor1->GetLocalPointVelocity( vPointVel1, vR1 );
		Vr -= vPointVel1;
	}

	const Scalar normal_vel = Vec3Dot( Vr, N );

	Scalar normal_impulse = 0.0f;
	if( normal_vel < AveMinSeparationVel )
	{
		// coefficient of restitution
		Scalar final_normal_vel = - collision.fElasticity * normal_vel;

		if( final_normal_vel < MinVelForProcessing )
		{
			// must be a contact could be zero elasticity in collision,
			// or could be zero elasticity in contact - don't care. relax
			// towards 0 penetration
			final_normal_vel = AveMinSeparationVel;
		}

		const Scalar delta_vel = final_normal_vel - normal_vel;

		if( MinVelForProcessing < delta_vel )
		{
			Scalar denominator = 0.0f;
			if( !(pActor0->GetActorFlag() & JL_ACTOR_STATIC) )
				denominator = pActor0->GetInvMass()
					        + Vec3Dot( N, Vec3Cross( pActor0->GetWorldInvInertia() * (Vec3Cross(vR0,N)), vR0 ) );
			if( pActor1 && !(pActor1->GetActorFlag() & JL_ACTOR_STATIC) )
				denominator = pActor1->GetInvMass()
					        + Vec3Dot( N, Vec3Cross( pActor1->GetWorldInvInertia() * (Vec3Cross(vR1,N)), vR1 ) );

			if( denominator < SCALAR_TINY )
				denominator = SCALAR_TINY;

			normal_impulse = delta_vel / denominator;
			const Vector3 vImpulse = normal_impulse * N;

			pActor0->ApplyWorldImpulse( vImpulse, vAvePos );
			if( pActor1 )
				pActor1->ApplyWorldImpulse( -vImpulse, vAvePos );

		}
	}

#ifdef DO_FRICTION

	// now do friction point by point
	for( iPos=collision.m_vecPointInfo.size(); iPos-- != 0 ; )
	{
		// For friction, work out the impulse in the opposite direction to the tangential
		// velocity that would be required to bring this point to a halt. Apply the minimum 
		// of this impulse magnitude, and the one obtained from the normal impulse. This 
		// prevents reversing the velocity direction.
		//
		// However, recalculate the velocity since it's changed. 
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];

		Vector3 v;
		Vector3 Vr_new;
		pActor0->GetLocalPointVelocity( Vr_new, pt_info.vR0 );

		if( pActor1 )
		{
			pActor1->GetLocalPointVelocity( v, pt_info.vR1 );
			Vr_new -= v;
		}

		Vector3 T = Vr_new - Vec3Dot(Vr_new,N) * N;
		const Scalar tangent_speed = Vec3Length( T );

		if( tangent_speed > SCALAR_TINY )
		{
			T /= -tangent_speed;

			const Scalar sc = impulses[iPos] * scale;
			const Scalar pt_normal_impulse
				= sc * (normal_impulse + impulses[iPos]);

			// calculate an "inelastic collision" to zero the relative vel
			Scalar denominator = 0.0f;
			if( !(pActor0->GetActorFlag() & JL_ACTOR_STATIC) )
			{
				denominator = pActor0->GetInvMass();
				v = pActor0->GetWorldInvInertia() * Vec3Cross( pt_info.vR0, T );
				denominator += Vec3Dot( T, Vec3Cross(v, pt_info.vR0) );
			}
			if( pActor1 && !(pActor1->GetActorFlag() & JL_ACTOR_STATIC) )
			{
				denominator += pActor1->GetInvMass();
				v = pActor1->GetWorldInvInertia() * Vec3Cross( pt_info.vR1, T );
				denominator += Vec3Dot( T, Vec3Cross(v, pt_info.vR1) );
			}

			if( denominator > SCALAR_TINY )
			{
				const Scalar impulse_to_reverse = tangent_speed / denominator;
				Scalar static_friction = collision.fStaticFriction;
				Scalar dynamic_friction = collision.fDynamicFriction;

				Scalar impulse_from_normal_impulse
					= static_friction * pt_normal_impulse;
				Scalar friction_impulse;

				if (impulse_to_reverse < impulse_from_normal_impulse)
					friction_impulse = impulse_to_reverse;
				else
					friction_impulse = dynamic_friction * pt_normal_impulse;

				T *= friction_impulse;
				pActor0->ApplyBodyWorldImpulse( T, pt_info.vR0 );
				
				if (pActor1)
					pActor1->ApplyBodyWorldImpulse( -T, pt_info.vR1 );
			}
		} // end of friction
	}
#endif

	pActor0->SetConstraintsAndCollisionsUnsatisfied();
	if( pActor1 )
		pActor1->SetConstraintsAndCollisionsUnsatisfied();

	return true;
}


//==============================================================
// ProcessCollisionForShock
//==============================================================
bool CJL_PhysicsManager::ProcessCollisionForShock( CJL_CollisionInfo& collision, Scalar dt )
//bool CJL_PhysicsManager::ProcessCollisionForShock( CJL_ContactInfo& collision, Scalar dt )
{
	collision.m_Satisfied = true;

	const Vector3& N = collision.m_vDirToBody0;
//	const Vector3& N = collision.vNormal;

	Vector3 vGravityDir = Vec3NormalizeSafe( Vector3( 0, -9.8f, 0 ) );	// m_vGravity

	if( fabsf( Vec3Dot(N, vGravityDir) ) < 0.9f )
		return false;

	unsigned iPos;
	const Scalar timescale = penetrationShockRelaxationTimesteps * dt;

	for( iPos = 0; iPos < collision.m_vecPointInfo.size(); iPos++ )
//	for( iPos = 0; iPos < 1; iPos++ )
	{
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];
//		CJL_ContactInfo& pt_info = collision;

		// per-point penetration resolution - for shock is much weaker
		if( pt_info.InitialPenetration > 0.0f )
			pt_info.fMinSeparationVel = pt_info.InitialPenetration / timescale;
		else
			pt_info.fMinSeparationVel = pt_info.InitialPenetration / dt;

		if( pt_info.fMinSeparationVel > MaxShockVelMag )
			pt_info.fMinSeparationVel = MaxShockVelMag;
	}

	// since this is shock, body 0 OR body 1 can be immovable. Also, if immovable
	// make the constraint against a non-moving object
	CJL_PhysicsActor *pActor0, *pActor1;
	if( collision.pBody0 )
		pActor0 = collision.pBody0->GetPhysicsActor();
	else
		pActor0 = NULL;
	if( collision.pBody1 )
		pActor1 = collision.pBody1->GetPhysicsActor();
	else
		pActor1 = NULL;

	// non-static frozen actors have been temporarily made static
	if( pActor0->GetActorFlag() & JL_ACTOR_STATIC )
		pActor0 = NULL;
	if( pActor1 && pActor1->GetActorFlag() & JL_ACTOR_STATIC )
		pActor1 = NULL;

	if( !pActor0 && !pActor1 )
		return false;

	// the individual impulses in the same order as collision->m_PointInfo - for friction
	Scalar impulses[16];	// CJL_ContactInfo::MAX_COLLISION_POINTS
	Scalar total_impulse = 0.0f;

	Vector3 avPos = Vector3(0,0,0);
	Scalar avMinSeparationVel = 0.0f;

	for( iPos = collision.m_vecPointInfo.size(); iPos-- != 0  ; )
//	for( iPos=0; iPos<1 ;iPos++ )
	{
//		Assert();
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];
//		CJL_ContactInfo& pt_info = collision;

		impulses[iPos] = 0.0f;

		Scalar normal_vel = 0;
		if( pActor0 )
			normal_vel = Vec3Dot( pActor0->GetLocalPointVelocity(pt_info.vR0), N );
		if( pActor1 )
			normal_vel = Vec3Dot( pActor1->GetLocalPointVelocity(pt_info.vR1), N );

		if( pt_info.fMinSeparationVel < normal_vel )
			continue;

		Scalar final_normal_vel = pt_info.fMinSeparationVel;
		Scalar normal_impulse = (final_normal_vel - normal_vel) / pt_info.fDenominator;

		if( normal_impulse < 0.0f )
			continue;

		impulses[iPos] = normal_impulse;
		total_impulse += normal_impulse;

		avPos += pt_info.vContactPosition * normal_impulse;
//		avPos += pt_info.vContactPoint * normal_impulse;
		avMinSeparationVel += pt_info.fMinSeparationVel * normal_impulse;
	}

	if( total_impulse <= SCALAR_TINY )
		return false;

	Scalar scale = 1.0f / total_impulse;

	// apply all these impulses (as well as subsequently applying
	// an impulse at an averaged position)
	for( iPos = collision.m_vecPointInfo.size(); iPos-- != 0  ; )
//	for( iPos=0; iPos<1 ;iPos++ )
	{
//		Assert();
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];

		Scalar sc = impulses[iPos] * scale;
		Vector3 vImpulse = (impulses[iPos] * sc) * N;

		if( pActor0 )
			pActor0->ApplyBodyWorldImpulse( vImpulse, pt_info.vR0 );
		if( pActor1 )
			pActor1->ApplyBodyWorldImpulse( -vImpulse, pt_info.vR1 );
	}

	avPos *= scale;
	avMinSeparationVel *= scale;

	// now calculate the single impulse to be applied at avPos
	Vector3 vR0, vR1;
	Vector3 Vr = Vector3(0,0,0);

	if( pActor0 )
	{
		vR0 = avPos - pActor0->GetPosition();
		pActor0->GetLocalPointVelocity( Vr, vR0 );
	}

	if( pActor1 )
	{
		vR1 = avPos - pActor1->GetPosition();
		Vector3 v;
		pActor1->GetLocalPointVelocity( v, vR1 );
		Vr -= v;
	}

	Scalar normal_vel = Vec3Dot( Vr, N );

	Scalar normal_impulse = 0.0f;
	if( normal_vel < avMinSeparationVel )
	{
		// coefficient of restitution
		Scalar final_normal_vel = avMinSeparationVel;

		Scalar delta_vel = final_normal_vel - normal_vel;

		if( MinVelForProcessing < delta_vel )
		{
			Scalar denominator = 0.0f;
			if( pActor0 )
			{
				denominator = pActor0->GetInvMass()
					+ Vec3Dot( N, Vec3Cross( pActor0->GetWorldInvInertia() * Vec3Cross(vR0,N), vR0 ) );
			}

			if( pActor1 )
			{
				denominator += pActor1->GetInvMass()
					+ Vec3Dot( N, Vec3Cross( pActor1->GetWorldInvInertia() * Vec3Cross(vR1,N), vR1 ) );
			}

			if( denominator < SCALAR_TINY )
				denominator = SCALAR_TINY;

			normal_impulse = delta_vel / denominator;
			Vector3 vImpulse = normal_impulse * N;

			if( pActor0 )
//				pActor0->ApplyBodyWorldImpulse( vImpulse, avPos );
				pActor0->ApplyWorldImpulse( vImpulse, avPos );
			if( pActor1 )
//				pActor1->ApplyBodyWorldImpulse( -vImpulse, avPos );
				pActor1->ApplyWorldImpulse( -vImpulse, avPos );

		}
	}


//#undef DO_FRICTION

#ifdef DO_FRICTION

	// now do friction point by point
	for( iPos = collision.m_vecPointInfo.size(); iPos-- != 0  ; )
//	for( iPos=0; iPos<1 ;iPos++ )
	{
		CJL_CollPointInfo& pt_info = collision.m_vecPointInfo[iPos];
		Vector3 Vr_new = Vector3(0,0,0);
		Vector3 v;

        // are collition.vR0 & vR1 in world coord ?
		pActor0->GetLocalPointVelocity( Vr_new, pt_info.vR0 );
		if( pActor1 )
		{
			pActor1->GetLocalPointVelocity( v, pt_info.vR1 );
			Vr_new -= v;
		}

		Vector3 vTangentVel = Vr_new - Vec3Dot(Vr_new,N) * N;
		Scalar tangent_speed = Vec3Length( vTangentVel );

		if( SCALAR_TINY < tangent_speed )
		{
			vTangentVel /= -tangent_speed;

			Scalar sc = impulses[iPos] * scale;
			Scalar pt_normal_impulse = sc * ( normal_impulse + impulses[iPos] );

			// calculate the 'enelastic collision' to zero the relative vel
			Scalar denominator = 0.0f;

			v = Vec3Cross( pActor0->GetWorldInvInertia() * (Vec3Cross(pt_info.vR0,vTangentVel)), pt_info.vR0 );
			denominator = pActor0->GetInvMass() + Vec3Dot( vTangentVel, v );

			if( pActor1 )
			{
				v = Vec3Cross( pActor1->GetWorldInvInertia() * (Vec3Cross(pt_info.vR1,vTangentVel)), pt_info.vR1 );
				denominator += pActor1->GetInvMass() + Vec3Dot( vTangentVel, v );
			}

			if( SCALAR_TINY < denominator )
			{
				Scalar impulse_to_reverse = tangent_speed / denominator;

				Scalar impulse_from_normal_impulse = collision.fStaticFriction * pt_normal_impulse;
				Scalar friction_impulse;

				if( impulse_to_reverse < impulse_from_normal_impulse )
				{
					friction_impulse = impulse_to_reverse;
				}
				else
				{
					friction_impulse = collision.fDynamicFriction * pt_normal_impulse;
				}
				
				vTangentVel *= friction_impulse;

				// are collision.vR0 & vR1 is world space
				pActor0->ApplyBodyWorldImpulse( vTangentVel, pt_info.vR0 );
				if( pActor1 )
					pActor1->ApplyBodyWorldImpulse( -vTangentVel, pt_info.vR1 );
			}
		}
	}	// end of friction

#endif

	if( pActor0 )
		pActor0->SetConstraintsAndCollisionsUnsatisfied();
	if( pActor1 )
		pActor1->SetConstraintsAndCollisionsUnsatisfied();

	return true;
}


//==============================================================
// CopyAllCurrentStatesToOld
//==============================================================
void CJL_PhysicsManager::CopyAllCurrentStatesToOld()
{
/*  int i , numBodies = mBodies.size();
	for (i = 0 ; i < numBodies ; ++i)	{
		if (mBodies[i]->IsActive() || mBodies[i]->GetVelChanged())
			mBodies[i]->CopyCurrentStateToOld();
	}*/

	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		if( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE ||
			itrActor->VelocityChanged() )
		{
			itrActor->CopyCurrentStateToOld();
		}
	}
}


//==============================================================
// TryToActivateAllFrozenObjects
//==============================================================
void CJL_PhysicsManager::TryToActivateAllFrozenObjects()
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		if( itrActor->GetActivityState() == CJL_PhysicsActor::FROZEN )
		{
			if( itrActor->ShouldBeActive() )
			{
				ActivateObject( &(*itrActor) );
			}
			else
			{
				if( itrActor->VelocityChanged() )
				{
                    itrActor->SetVelocity( Vector3(0,0,0) );
					itrActor->SetAngularVelocity( Vector3(0,0,0) );
					itrActor->ClearVelocityChanged();
				}
			}
		}
		itrActor++;
	}
}


//==============================================================
// DetectAllCollisions
//==============================================================
void CJL_PhysicsManager::DetectAllCollisions(Scalar dt)
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		itrActor->StoreState();
	}

	// temporarily update velocities and positions?
	UpdateAllVelocities(dt);
	UpdateAllPositions(dt);


//	m_vecCollision.clear();
	m_vecCollision.resize(0);

	CJL_CollisionFunctor collision_functor;

	collision_functor.SetCollisionBuffer( &m_vecCollision );

	collision_functor.m_fCollTolerance = m_CollTolerance;

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		// clear all the collision indices of every actor
//		itrActor->GetCollisionIndexBuffer()->clear();
		itrActor->GetCollisionIndexBuffer().resize(0);

		if( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE )
			itrActor->UpdateWorldProperties();
	}

	JL_CollisionDetect_All( m_ActorList, collision_functor, this );

	std::random_shuffle( m_vecCollision.begin(), m_vecCollision.end() );

    for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		itrActor->RestoreState();
	}
}

/*
//==============================================================
// HandleAllCollisions
//==============================================================
void CJL_PhysicsManager::HandleAllCollisions(Scalar dt)
{
//  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  // handle collisions by:
  // 1. store the original position/velocity/force info.
  // 2. predict a new velocity using the external forces (and zero the forces)
  // 3. use the new velocity to predict a new position
  // 4. detect contacts
  // 5. reset the velocity to its original, zero forces
  // 6. classify the contacts based on the "current" velocity, and add/apply impulses
  // 7. repeat step 6 a few times...
  // 8. reset the position/forces to the original

  // then in the next functions
  // 9. apply forces

  // 10.0 detect collisions and handle using inelastic collisions
}*/

/*
//==============================================================
// SeparateObjects
//==============================================================
void CJL_PhysicsManager::SeparateObjects(CJL_ContactInfo& collision, Scalar factor)
{
    CJL_ShapeBase *pShape0 = collision.pBody0;
    CJL_ShapeBase *pShape1 = collision.pBody1;

	CJL_PhysicsActor *pActor0 = pShape0->GetPhysicsActor();

  // TODO wake up if separation is needed?
//  if ( (pShape0->GetActorFlag() & JL_ACTOR_STATIC) || (pShape0->get_activity_state() == Rigid_body::FROZEN) )
//    return;
    int i, iNumCollisions;

  if ( (pShape1 == 0) || 
       (pShape1->GetPhysicsActor()->GetActorFlag() & JL_ACTOR_STATIC) ||
       (pShape1->GetPhysicsActor()->GetActorFlag() & JL_ACTOR_KINEMATIC) ||
       (pShape1->GetPhysicsActor()->GetActivityState() == CJL_PhysicsActor::FROZEN) )
  {
    // ground - just update body 0
    Vector3 delta = (factor * collision.fPenetrationDepth) * collision.vNormal;
    Vector3 orig = pActor0->GetPosition();
    pActor0->SetPosition(orig + delta);

	vector<int>& rveciColIndex0 = pActor0->GetCollisionIndexBuffer();
	iNumCollisions = rveciColIndex0.size();
	for( i=0; i<iNumCollisions; i++ )
	{
		CJL_ContactInfo &collision_info = m_vecCollision[ rveciColIndex0[i] ];
		collision_info.fPenetrationDepth -= Vec3Dot( &delta, &collision_info.vNormal );
	}
  }

  else
  {	// collision between two movable actors
	  CJL_PhysicsActor *pActor1 = pShape1->GetPhysicsActor();

	  Vector3 delta = (factor * collision.fPenetrationDepth) * collision.vNormal;

    // how should we partition the separation? It's not physical, so just 50-50
    delta *= 0.5f;
    Vector3 orig0 = pActor0->GetPosition();
    Vector3 orig1 = pActor1->GetPosition();
    pActor0->SetPosition(orig0 + delta);
    pActor1->SetPosition(orig1 - delta);

    // now update all the collisions affected
	vector<int>& rveciColIndex0 = pActor0->GetCollisionIndexBuffer();
	iNumCollisions = rveciColIndex0.size();
	for( i=0; i<iNumCollisions; i++ )
	{
		CJL_ContactInfo &collision_info = m_vecCollision[ rveciColIndex0[i] ];
		if( collision_info.pBody0 == collision.pBody0 )
			collision_info.fPenetrationDepth -= Vec3Dot( &delta, &collision_info.vNormal );
		else
			collision_info.fPenetrationDepth += Vec3Dot( &delta, &collision_info.vNormal );
	}

	vector<int>& rveciColIndex1 = pActor1->GetCollisionIndexBuffer();
	iNumCollisions = rveciColIndex1.size();
	for( i=0; i<iNumCollisions; i++ )
	{
		CJL_ContactInfo &collision_info = m_vecCollision[ rveciColIndex1[i] ];
		if( collision_info.pBody0 == collision.pBody1 )
			collision_info.fPenetrationDepth += Vec3Dot( &delta, &collision_info.vNormal );
		else
			collision_info.fPenetrationDepth -= Vec3Dot( &delta, &collision_info.vNormal );
	}
  }
}*/


//==============================================================
// HandleAllConstraints
//==============================================================
void CJL_PhysicsManager::HandleAllConstraints(Scalar dt, int num_coll_iterations, bool force_inelastic )
{
	unsigned i;
	unsigned num_orig_collisions = m_vecCollision.size();
	const unsigned num_constraints = m_constraints.size();

	// prepare all the constraints
/*	for (i = 0 ; i < num_constraints ; ++i)
	{
//  	m_constraints[i]->PreApply(dt);
	}*/

	// prepare all the collisions
	if( force_inelastic )
	{
		for (i=0 ; i<num_orig_collisions ; ++i)
		{
			m_vecCollision[i].fElasticity = 0.0f;
			m_vecCollision[i].m_Satisfied = false;
		}
	}
	else
	{
		// prepare for the collisions
		for( i=0; i<num_orig_collisions; ++i )
            PreProcessCollision(m_vecCollision[i], dt);
	}

	// iterate over the collisions
	static int dir = 1;
	unsigned step;
	for (step=0; step<num_coll_iterations; ++step)
	{
		bool gotOne = false;

		// step 6
		unsigned num_collisions = m_vecCollision.size();
		dir = !dir;
		for( i = dir ? 0 : num_collisions - 1;
			 0 <= i && i < num_collisions;
			 dir ? ++ i : -- i )
		{
			if( !m_vecCollision[i].m_Satisfied )
			{
				gotOne |= (this->*m_ProcessCollisionFn)( m_vecCollision[i], dt );
//				gotOne |= ProcessCollision( m_vecCollision[i], dt );
			}
		}
/*		for( i=0; i<num_constraints; ++i )
		{
			if( m_constraints[i].GetSatisfied() )
			{
				gotOne |= m_constraints[i].Apply(dt);
			}
		}*/
	
		// wake up any previously stationary frozen objects that were frozen.
		if( m_FreezingEnabled )
			TryToActivateAllFrozenObjects();

		// number of collisions may have been increased.
		num_collisions = m_vecCollision.size();

		// preprocess any new collisions.
		if( force_inelastic )
		{
			for( i=num_orig_collisions; i<num_collisions; ++i )
			{
				m_vecCollision[i].fElasticity = 0.0f;
				m_vecCollision[i].m_Satisfied = false;
				PreProcessCollision( m_vecCollision[i], dt );
			}
		}
		else
		{
			for( i=num_orig_collisions; i<num_collisions; ++i )
			{
				PreProcessCollision( m_vecCollision[i], dt ); 
			}
		}

		num_orig_collisions = num_collisions;

		if( !gotOne )
			break;
	}
}


inline bool LessBodyY( CJL_PhysicsActor *pActor0, CJL_PhysicsActor *pActor1 )
{
	return pActor0->GetPosition().y < pActor1->GetPosition().y;
}


void CJL_PhysicsManager::DoShockStep(Scalar dt)
{
	m_ActorList.Sort( LessBodyY );

	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	vector<CJL_CollisionInfo>& vecCollision = m_vecCollision;
//	vector<CJL_ContactInfo>& vecCollision = m_vecCollision;

	bool gotOne = true;
	unsigned int nLoops = 0;
	while( gotOne )
	{
		gotOne = false;
		nLoops++;
		for( itrActor = m_ActorList.Begin();
			 itrActor != m_ActorList.End();
			 itrActor++ )
		{
			if( !(itrActor->GetActorFlag() & JL_ACTOR_STATIC) && itrActor->GetDoShockProcessing() )
			{
//			  if( skin )
//			  {
				vector<int>& vecCollIndex = itrActor->GetCollisionIndexBuffer();
				int num_colls = vecCollIndex.size();

				if( num_colls == 0 || itrActor->GetActivityState() != CJL_PhysicsActor::ACTIVE )
				{
					// no collision and sleeping - make it immovable (temporarily)
					itrActor->InternalSetImmovable();
				}
				else
				{
					bool bSetImmovable = false;
					// process every collision on body that is between it and
					// another immovable... then make it immovable (temporarily)
					for( int i=0; i<num_colls; i++ )
					{
						CJL_CollisionInfo& coll_info = vecCollision[ vecCollIndex[i] ];
//						CJL_ContactInfo& coll_info = vecCollision[ vecCollIndex[i] ];

						if( ( coll_info.pBody0->GetPhysicsActor() == &(*itrActor) &&
							( (coll_info.pBody1 == NULL) || (coll_info.pBody1->GetPhysicsActor()->GetActorFlag() & JL_ACTOR_STATIC) ) )
							||
							( coll_info.pBody1->GetPhysicsActor() == &(*itrActor) &&
							( (coll_info.pBody0 == NULL) || (coll_info.pBody0->GetPhysicsActor()->GetActorFlag() & JL_ACTOR_STATIC) ) ) )
						{
							// need to recalc denominator since immovable set
							PreProcessCollision( coll_info, dt );
							ProcessCollisionForShock( coll_info, dt );
							bSetImmovable = true;
						}

					}

					if( bSetImmovable )
					{
						itrActor->InternalSetImmovable();
						gotOne = true;
					}
				}
//			  }
//			  else
//			  {
//				// no skin - help early out next loop
//				itrActor->InternalSetImmovable();
//			  }
			}
		}
	}

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		itrActor->InternalRestoreImmovable();
	}

}


//==============================================================
// GetAllExternalForces
//==============================================================
void CJL_PhysicsManager::GetAllExternalForces(Scalar dt)
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		if( !(itrActor->GetActorFlag() & (JL_ACTOR_TAKE_NO_GRAVITY | JL_ACTOR_STATIC | JL_ACTOR_KINEMATIC)) )
		{	// simply add gravity for experiment
///			itrActor->AddWorldForce( itrActor->GetMass() * Vector3( 0, -9.8f, 0 ) );
			itrActor->AddWorldForce( itrActor->GetMass() * m_vGravity );
		}

	}

/*	int i;
  for (i = 0 ; i < iNumRigidBodyEntities ; ++i)
    m_rigid_bodies[i]->add_external_forces(dt);

  int num_controllers = m_controllers.size();
  for (i = 0 ; i < num_controllers ; ++i) {
    m_controllers[i]->update(dt);
  }*/
}

//==============================================================
// UpdateAllVelocities
//==============================================================
void CJL_PhysicsManager::UpdateAllVelocities(Scalar dt)
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		if( !(itrActor->GetActorFlag() & JL_ACTOR_STATIC) &&
			( itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE || itrActor->VelocityChanged() ) )
		{
			itrActor->UpdateVelocities(dt);
		}
		itrActor++;
	}
}


//==============================================================
// LimitAllVelocities
//==============================================================
void CJL_PhysicsManager::LimitAllVelocities()
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		itrActor->LimitVel();
		itrActor->LimitAngVel();
		itrActor++;
	}
}


//==============================================================
// ClearAllForces
//==============================================================
void CJL_PhysicsManager::ClearAllForces()
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		itrActor->ClearForces();
		itrActor++;
	}
}


//==============================================================
// UpdateAllPositions
//==============================================================
void CJL_PhysicsManager::UpdateAllPositions(Scalar dt)
{
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		if( !(itrActor->GetActorFlag() & JL_ACTOR_STATIC) &&
			itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE )
		{
			itrActor->UpdatePositions(dt);
		}
		itrActor++;
	}
}


//==============================================================
// TryToFreezeAllObjects
//==============================================================
void CJL_PhysicsManager::TryToFreezeAllObjects(Scalar dt)
{
//	TRACE_METHOD_ONLY(MULTI_FRAME_1);

  	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	itrActor = m_ActorList.Begin();

	while( itrActor != m_ActorList.End() )
	{
		if( !(itrActor->GetActorFlag() & JL_ACTOR_STATIC) )
		{
			itrActor->TryToFreeze(dt);
		}
		itrActor++;
	} 
}


//==============================================================
// ActivateAllFrozenObjectsLeftHanging
//==============================================================
void CJL_PhysicsManager::ActivateAllFrozenObjectsLeftHanging()
{
  	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;

	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		if( (itrActor->GetActivityState() == CJL_PhysicsActor::ACTIVE) &&
			!(itrActor->GetActorFlag() & JL_ACTOR_STATIC ) )
		{
			// first activate any bodies due to the movement of this body
			itrActor->DoMovementActivations();

			// now record any movement notifications that are needed
			vector<int>& veciCollisionIndex = itrActor->GetCollisionIndexBuffer();

			if( veciCollisionIndex.empty() )
				continue;

			// walk through the object's contact list
			int j;
			const int iNumCollisions = veciCollisionIndex.size();
			for (j = 0 ; j < iNumCollisions; ++j)
			{
				assert(veciCollisionIndex[j] < (int) m_vecCollision.size());
				const CJL_CollisionInfo& collision = m_vecCollision[ veciCollisionIndex[j] ];
//				const CJL_ContactInfo& collision = m_vecCollision[ veciCollisionIndex[j] ];

				// must be a body-body interaction to be interesting
				if( !collision.pBody1 )
					continue;

				CJL_PhysicsActor *pOtherActor = collision.pBody0->GetPhysicsActor();
				Vector3 vThisBodyNormal = collision.m_vDirToBody0;
//				Vector3 vThisBodyNormal = collision.vNormal;
				if( pOtherActor == &(*itrActor) )
				{
					pOtherActor = collision.pBody1->GetPhysicsActor();
					vThisBodyNormal *= -1;
				}
				if( pOtherActor->GetActivityState() == CJL_PhysicsActor::FROZEN )
				{
					itrActor->AddMovementActivation( itrActor->GetPosition(), pOtherActor );
				}

			}
		}
	}
}


//==============================================================
// Integrate
//==============================================================
void CJL_PhysicsManager::Integrate(Scalar dt)
{
	m_DoingIntegration = true;

//	m_OldTime = m_TargetTime;
//	m_TargetTime += dt;

//	SetCollisionFunctions();

//	FindAllActiveBodies();

	CopyAllCurrentStatesToOld();
	
	GetAllExternalForces(dt);

	DetectAllCollisions(dt);

	HandleAllConstraints(dt, m_NumCollisionIterations, false);

	UpdateAllVelocities(dt);

	HandleAllConstraints(dt, m_NumContactIterations, true);

	// do a shock step to help stacking
	if( m_DoShockStep )
		DoShockStep(dt);

//	DampActiveBodies();

	if( m_FreezingEnabled )
	{
		TryToFreezeAllObjects(dt);
		ActivateAllFrozenObjectsLeftHanging();
	}

	LimitAllVelocities();

	UpdateAllPositions(dt);

//	NotifyAllPostPhysics(dt);

	m_DoingIntegration = false;
}


//==============================================================
// enable_freezing
//==============================================================
/*void CJL_PhysicsManager::enable_freezing(bool freeze)
{
  m_FreezingEnabled = freeze;

  if (!m_FreezingEnabled)
  {
    int iNumRigidBodyEntities = m_rigid_bodies.size();
    int i;
    for (i = 0 ; i < iNumRigidBodyEntities ; ++i)
    {
      m_rigid_bodies[i]->set_activity_state(Rigid_body::ACTIVE);
    }
  }
}*/


void CJL_PhysicsManager::SetMaterial( short sMaterialIndex, CJL_SurfaceMaterial& rMaterial )
{
	if( m_vecMaterial.size() <= sMaterialIndex )
	{
		m_vecMaterial.resize( sMaterialIndex + 1 );
	}

	m_vecMaterial[sMaterialIndex] = rMaterial;
}


void CJL_PhysicsManager::SetCollisionGroupState( int group0, int group1, bool bEnableCollision )
{

	if( group0 < 0 || NUM_MAX_COLLISION_GROUPS <= group0 ||
		group1 < 0 || NUM_MAX_COLLISION_GROUPS <= group1 )
		return;

	if( bEnableCollision )
	{
		m_CollisionGroupTable[group0][group1] = 1;
		m_CollisionGroupTable[group1][group0] = 1;
	}
	else
	{	// disable collisions between actors in group0 and group1
		m_CollisionGroupTable[group0][group1] = 0;
		m_CollisionGroupTable[group1][group0] = 0;
	}
}


void CJL_PhysicsManager::SetCollisionGroupState( int group, bool bEnableCollision )
{
	for( int i=0; i<NUM_MAX_COLLISION_GROUPS; i++ )
	{
		SetCollisionGroupState( group, i, bEnableCollision );
	}
}


void CJL_PhysicsManager::ClipLineSegment( CJL_LineSegment& segment )
{
	// TODO: line segment check for every shape
	// only support triangle mesh for the moment
	TCPreAllocDynamicLinkList<CJL_PhysicsActor>::LinkListIterator itrActor;
	for( itrActor = m_ActorList.Begin();
		 itrActor != m_ActorList.End();
		 itrActor++ )
	{
		int i, num_shapes = itrActor->GetNumShapes();
		for( i=0; i<num_shapes; i++ )
		{
			if( itrActor->GetShape(i)->GetShapeType() == JL_SHAPE_TRIANGLEMESH )
			{
				((CTriangleMesh *)itrActor->GetShape(i))->ClipLineSegment( segment );
			}
		}
	}
}


/*
CJL_PhysicsActor *CJL_PhysicsManager::CreateStaticTriangleMesh( vector<Vector3>& rvecvVertex,
															   vector<int>& rveciTriangleIndex )
{
	CJL_ShapeDesc_TriangleMesh mesh_desc;
	CJL_PhysicsActorDesc actor_desc;

	mesh_desc.pveciIndex = &rveciTriangleIndex;
	mesh_desc.pvecvVertex = &rvecvVertex;
	actor_desc.vecpShapeDesc.push_back( &mesh_desc );
	actor_desc.ActorFlag = JL_ACTOR_STATIC;
	return CreateActor( actor_desc );
}*/