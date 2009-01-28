
#include "JL_PhysicsActor.h"
#include "JL_PhysicsManager.h"

#include "JL_Shape_Box.h"

#include "3DMath/MathMisc.h"

#include "Support/MsgBox.h"



bool CJL_PhysicsActor::m_indicate_frozen_objects = true;

inline Scalar safe_inv_num(Scalar num) 
{
  if (num > 0.0f)
    return (1.0f / num);
  else
    return (1.0e-30);
}

//==============================================================
// SetPosition
// MSVC6.0 completely screws up if this is made inline (as it
// really should be). 
//==============================================================
void CJL_PhysicsActor::SetPosition(const Vector3 & pos)
{
  m_vPosition = pos;
}

/*
//==============================================================
// CJL_PhysicsActor
//==============================================================
CJL_PhysicsActor::CJL_PhysicsActor(CJL_PhysicsManager * pPhysicsManager)
:
m_pPhysicsManager(pPhysicsManager)
{}*/

//==============================================================
// set_mass
//==============================================================
void CJL_PhysicsActor::SetMass(Scalar mass)
{
  m_fMass = mass;
  m_fInvMass = safe_inv_num(m_fMass);
}

//==============================================================
// set_inv_mass
//==============================================================
void CJL_PhysicsActor::SetInvMass(Scalar inv_mass)
{
  m_fInvMass = inv_mass;
  m_fMass = safe_inv_num(m_fInvMass);
}

//==============================================================
// setLocal_inertia
//==============================================================
void CJL_PhysicsActor::SetLocalInertia(Scalar Ixx, Scalar Iyy, Scalar Izz)
{
  m_matLocalInertia.SetTo(0.0f);
  m_matLocalInertia(0, 0) = Ixx;
  m_matLocalInertia(1, 1) = Iyy;
  m_matLocalInertia(2, 2) = Izz;

  m_matLocalInvInertia.SetTo(0.0);
  m_matLocalInvInertia(0, 0) = safe_inv_num(Ixx);
  m_matLocalInvInertia(1, 1) = safe_inv_num(Iyy);
  m_matLocalInvInertia(2, 2) = safe_inv_num(Izz);
}

//==============================================================
// setLocal_inv_inertia
//==============================================================
void CJL_PhysicsActor::SetLocalInvInertia(Scalar inv_Ixx, Scalar inv_Iyy, Scalar inv_Izz)
{
  m_matLocalInvInertia.SetTo(0.0f);
  m_matLocalInvInertia(0, 0) = inv_Ixx;
  m_matLocalInvInertia(1, 1) = inv_Iyy;
  m_matLocalInvInertia(2, 2) = inv_Izz;

  m_matLocalInertia.SetTo(0.0);
  m_matLocalInertia(0, 0) = safe_inv_num(inv_Ixx);
  m_matLocalInertia(1, 1) = safe_inv_num(inv_Iyy);
  m_matLocalInertia(2, 2) = safe_inv_num(inv_Izz);
}

//==============================================================
// addWorld_force
//==============================================================
void CJL_PhysicsActor::AddWorldForce(const Vector3 & force, const Vector3 & pos)
{
  m_vForce += force ;
  Vector3 v = Vec3Cross( pos - m_vPosition, force );
  m_vTorque += v;
	m_bVelocityChanged = true;
}

//==============================================================
// addWorld_torque
//==============================================================
void CJL_PhysicsActor::AddWorldTorque(const Vector3 & torque, const Vector3 & pos)
{
  m_vTorque += torque;
  Vector3 v = Vec3Cross( pos - m_vPosition, torque );
  m_vForce += v;
	m_bVelocityChanged = true;
}

//==============================================================
// addLocal_force
//==============================================================
void CJL_PhysicsActor::AddLocalForce(const Vector3 & force)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocal_force
//==============================================================
void CJL_PhysicsActor::AddLocalForce(const Vector3 & force, const Vector3 & pos)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocal_torque
//==============================================================
void CJL_PhysicsActor::AddLocalTorque(const Vector3 & torque)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocal_torque
//==============================================================
void CJL_PhysicsActor::AddLocalTorque(const Vector3 & torque, const Vector3 & pos)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocalImpulse
//==============================================================
void CJL_PhysicsActor::ApplyLocalImpulse(const Vector3 & impulse)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocalImpulse
//==============================================================
void CJL_PhysicsActor::ApplyLocalImpulse(const Vector3 & impulse, const Vector3 & pos)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocalAngularImpulse
//==============================================================
void CJL_PhysicsActor::ApplyLocalAngularImpulse(const Vector3 & ang_impule)
{
//  assert1(!"not implemented!");
}

//==============================================================
// addLocalAngularImpulse
//==============================================================
void CJL_PhysicsActor::ApplyLocalAngularImpulse(const Vector3 & angImpulse, const Vector3 & pos)
{
//  assert1(!"not implemented!");
}

//==============================================================
// clear_all_forces
//==============================================================
void CJL_PhysicsActor::ClearForces()
{
  m_vForce = Vector3(0,0,0);
  m_vTorque = Vector3(0,0,0);
}

//==============================================================
// update_velocity
//==============================================================
void CJL_PhysicsActor::UpdateVelocities(Scalar dt)
{
  if ( (m_ActorFlag & JL_ACTOR_STATIC) || (m_ActivityState == FROZEN) )
    return;

#ifdef CHECK_RIGID_BODY
  Vector3 orig_velocity = m_vVelocity;
  Vector3 orig_rotation = m_vAngularVelocity;
#endif

	m_vVelocity += (dt * m_fInvMass) * m_vForce ;
	m_vAngularVelocity += m_matWorldInvInertia * (dt * m_vTorque);
	m_bVelocityChanged = true;

#ifdef CHECK_RIGID_BODY
  // check the result, and roll-back if needed
  if ( !IsSensible(m_vVelocity) )
  {
/*    TRACE("Velocity is not sensible: this = %p\n", this);
    orig_velocity.show("orig vel");
    m_vForce.show("force");
    m_vVelocity.show("velocity");
    while (1) {}*/
    m_vVelocity = orig_velocity;
  }

  if ( !IsSensible(m_vAngularVelocity) )
  {
/*    TRACE("rotation is not sensible: this = %p\n-", this);
    m_vAngularVelocity.show("rotation");
    orig_rotation.show("orig");
    m_vTorque.show("torque");
    m_matWorldInvInertia.show("inv world inertia");
    while (1) {}*/
    m_vAngularVelocity = orig_rotation;
  }
#endif
}

/// returns false if the orientation is a zero matrix
inline bool IsOrientationValid( const Matrix33& orient )
{
	int i;
	for( i=0; i<9; i++ )
	{
		if( orient.GetData()[i] != 0.0f )
			break;
	}
	if( i == 9 )
		return false;
	else
		return true;
}


//==============================================================
// update_position
//==============================================================
void CJL_PhysicsActor::UpdatePositions(Scalar dt)
{
  // in case something goes wrong...
  Vector3 orig_position = m_vPosition;
  Matrix33 orig_orientation = m_matOrientation;

  // position is easy
  m_vPosition += dt * m_vVelocity;

  // rotation is easy too!
  const Vector3 & rot = m_vAngularVelocity;
  m_matOrientation += dt * Matrix33(     0, rot[2], -rot[1],
                                  -rot[2],      0,  rot[0],
                                   rot[1],-rot[0],       0 ) * orig_orientation;
  m_matOrientation.Orthonormalize();

  // check the result, and roll-back if needed
  // hmmm probably due to velocity/ration being screwed, so reset them
  if ( !IsSensible(m_vPosition) )
  {
/*    TRACE("Position is not sensible: this = %p", this);
    m_vPosition.show("position");
    orig_position.show("orig position");
    m_vVelocity.show("velocity");
    while (1) {}*/
    m_vPosition = orig_position;
    m_matOrientation = orig_orientation;
    m_vVelocity = Vector3(0,0,0);
    m_vAngularVelocity = Vector3(0,0,0);
  }
  if ( !m_matOrientation.IsSensible() || !IsOrientationValid(m_matOrientation) )
  {
/*    TRACE("Orientation is not sensible: this = %p", this);
    m_matOrientation.show("orientation");
    orig_orientation.show("orig orientation");
    rot.show("rot");
    while (1) {}*/
    m_matOrientation = orig_orientation;
    m_vPosition = orig_position;
    m_vVelocity = Vector3(0,0,0);
    m_vAngularVelocity = Vector3(0,0,0);
  }

  m_matInvOrientation = Matrix33Transpose(m_matOrientation);
 
  // recalculate the world inertia
  m_matWorldInvInertia = m_matOrientation * m_matLocalInvInertia * m_matInvOrientation;

}

//==============================================================
// state_to_str
//==============================================================
static const char * state_to_str(CJL_PhysicsActor::Activity state)
{
  switch (state)
  {
  case CJL_PhysicsActor::ACTIVE: return "ACTIVE";
  case CJL_PhysicsActor::FROZEN: return "FROZEN";
  }
  return "Invalid state";
}

//==============================================================
// set_deactivation_time
//==============================================================
void CJL_PhysicsActor::SetDeactivationTime(Scalar seconds)
{
	m_deactivation_time = seconds;
}

//==============================================================
// SetActivityThreshold
//==============================================================
void CJL_PhysicsActor::SetActivityThreshold(Scalar vel, Scalar rot)
{
  m_sqrVelocityActivityThreshold = vel * vel;
  m_sqrAngularVelocityActivityThreshold = deg_to_rad(rot) * deg_to_rad(rot);
}

//==============================================================
// SetActivityThreshold
//==============================================================
void CJL_PhysicsActor::SetDeactivationThreshold( Scalar fPosThreshold, Scalar fOrientTheshold )
{
  m_fSqDeltaPosThreshold = fPosThreshold * fPosThreshold;
  m_fSqDeltaQuatOrientThreshold = fOrientTheshold * fOrientTheshold;	//deg_to_rad(rot) * deg_to_rad(rot);
}

//==============================================================
// setActivationFactor
//==============================================================
void CJL_PhysicsActor::SetActivationFactor(Scalar factor)
{
  m_sqrActivationFactor = factor * factor;
}

//==============================================================
// SetActivityState
//==============================================================
void CJL_PhysicsActor::SetActivityState(Activity state, Scalar activityFactor)
{
//  TRACE_FILE_IF(FRAME_1)
//    TRACE("this = %p: old state = %s, new state = %s\n", 
//      this, state_to_str(m_ActivityState), state_to_str(state));

  if (m_allow_freezing)
    m_ActivityState = state;
  else
    m_ActivityState = ACTIVE;

  if (m_ActivityState == ACTIVE)
  {
    m_inactive_time = (1.0f - activityFactor) * m_deactivation_time;
  }
#if 1
  Scalar frac = m_inactive_time / m_deactivation_time;
  Scalar r = 0.5f;
  Scalar scale = 1.0f - frac / r;
  if (scale < 0.0f) scale = 0.0f;
  if (scale > 1.0f) scale = 1.0f;

  m_vVelocity *= scale;
  m_vAngularVelocity *= scale;
#endif
}


//==============================================================
// SetAllowFreezing
//==============================================================
void CJL_PhysicsActor::SetAllowFreezing(bool allow)
{
  m_allow_freezing = allow;
  SetActivityState(ACTIVE);
}

//==============================================================
// MoveTo
//==============================================================
void CJL_PhysicsActor::MoveTo(const Vector3 & pos)
{
  if (GetActivityState() == FROZEN)
  {
    m_pPhysicsManager->ActivateObject(this);
  }
  SetPosition(pos);
}


/// the following functions were inline in the original jiglib

//==============================================================
// doMovementActivations
//==============================================================
void CJL_PhysicsActor::DoMovementActivations()
{
  if (m_vecpActorsToBeActivatedOnMovement.empty())
    return;
  // TODO don't use hard-coded distance
//if ( (m_vPosition - m_vStoredPositionForActivation).mag2() < 0.05f)
  Vector3 v = m_vPosition - m_vStoredPositionForActivation;
  if( Vec3LengthSq( v )  < 0.05f )
    return;

  const size_t num_bodies = m_vecpActorsToBeActivatedOnMovement.size();
  size_t i;
  for (i = 0 ; i < num_bodies ; ++i)
  {
    m_pPhysicsManager->ActivateObject(m_vecpActorsToBeActivatedOnMovement[i]);
  }
//  m_vecpActorsToBeActivatedOnMovement.clear();
  m_vecpActorsToBeActivatedOnMovement.resize(0);
}

//==============================================================
// LimitVel
//==============================================================
void CJL_PhysicsActor::LimitVel()
{
	Scalar VelMax = 100.0f;

	Limit( m_vVelocity.x, -VelMax, VelMax );
	Limit( m_vVelocity.y, -VelMax, VelMax );
	Limit( m_vVelocity.z, -VelMax, VelMax );
}


//==============================================================
// LimitAngVel
//==============================================================
void CJL_PhysicsActor::LimitAngVel()
{
	Scalar AngVelMax = 100.0f;

	Limit( m_vAngularVelocity.x, -AngVelMax, AngVelMax );
	Limit( m_vAngularVelocity.y, -AngVelMax, AngVelMax );
	Limit( m_vAngularVelocity.z, -AngVelMax, AngVelMax );
}


//==============================================================
// SetConstraintsAndCollisionsUnsatisfied
//==============================================================
void CJL_PhysicsActor::SetConstraintsAndCollisionsUnsatisfied()
{
//	for( size_t constraint = m_constraints.size(); constraint-- != 0 )
//		m_constraints[constraint]->SetUnsatisfied();

#ifndef USE_COMBINED_COLLISION_POINTS_INFO
//	vector<CJL_ContactInfo> *pvecCollInfo = &m_pPhysicsManager->m_vecCollision;
#else
	vector<CJL_CollisionInfo> *pvecCollInfo = &m_pPhysicsManager->m_vecCollision;
#endif

	if( !(m_ActorFlag & JL_ACTOR_KINEMATIC) )
	{
		for( size_t coll = m_veciCollisionIndex.size(); coll-- != 0 ; )
			(*pvecCollInfo)[m_veciCollisionIndex[coll]].m_Satisfied = false;
	}
}


//==============================================================
// AddGravity
//==============================================================
void CJL_PhysicsActor::AddGravity()
{
  if ( !(m_ActorFlag & JL_ACTOR_STATIC) && (m_ActivityState == ACTIVE) )
	  return;
//    AddWorldForce(m_fMass * m_pPhysicsManager->GetGravity());
 //   AddWorldForce(m_fMass * Physics::GetGravity());
}


//==============================================================
// Init
//==============================================================

void CJL_PhysicsActor::Init( CJL_PhysicsActorDesc &rActorDesc )
{
	// since we are reusing objects to avoid dynamic memory allocations, we need to set default values explicitly
	SetDefault();

	if( m_pPhysicsManager->m_FreezingEnabled )
	{
		if( rActorDesc.bAllowFreezing )
			m_allow_freezing = true;
		else
			m_allow_freezing = false;
	}
	else
		m_allow_freezing = false;

	// global pose
	m_vPosition = rActorDesc.vPosition;
	m_matOrientation = rActorDesc.matOrient;

	// velocity
	m_vVelocity = rActorDesc.vVelocity;
	m_vAngularVelocity = rActorDesc.vAngularVelocity;

	m_vForce  = Vector3(0,0,0);
	m_vTorque = Vector3(0,0,0);

	// mass
	m_fMass = rActorDesc.fMass;
	m_fInvMass = 1.0f / m_fMass;

	m_ActorFlag = rActorDesc.ActorFlag;

	m_iCollisionGroup = rActorDesc.iCollisionGroup;

	// TODO: fix the bug caused by invalid actor desc
	// descs for fragment entities apparently caused the problem, but not for sure
	size_t num_shapes = rActorDesc.vecpShapeDesc.size();
	if( num_shapes < 0 || 100 < num_shapes )
	{
		MsgBoxFmt( "CJL_PhysicsActor::Init() - invalid actor desc: %d shapes" );
		return;
	}

	// set shapes
	size_t i;
	CJL_ShapeBase *pShape;
	for( i=0; i<rActorDesc.vecpShapeDesc.size(); i++ )
	{
		rActorDesc.vecpShapeDesc[i]->pPhysicsActor = this;

		switch( rActorDesc.vecpShapeDesc[i]->sShape )
		{
		case JL_SHAPE_BOX:
			pShape = m_pPhysicsManager->m_BoxShape.GetNewObject();
			break;
		case JL_SHAPE_CAPSULE:
			pShape = m_pPhysicsManager->m_CapsuleShape.GetNewObject();
			break;
		case JL_SHAPE_TRIANGLEMESH:
			pShape = m_pPhysicsManager->m_TriangleMesh.GetNewObject();
			break;
		default:
			pShape = NULL;
		}

		if( pShape )
		{	// initialize the shape
			pShape->Init( *rActorDesc.vecpShapeDesc[i] );
			m_vecpShape.push_back( pShape );
		}
	}

	// compute inertia tensor
	memset( &m_matLocalInertia, 0, sizeof(Matrix33) );
	Matrix33 matInertia;
	for( i=0; i<rActorDesc.vecpShapeDesc.size(); i++ )
	{	// actor is made of one shape
		memset( &matInertia, 0, sizeof(Matrix33) );

		rActorDesc.vecpShapeDesc[i]->GetLocalInertiaTensor( matInertia, m_fMass );

		m_matLocalInertia += matInertia;

		m_matLocalInvInertia = Matrix33Transpose( m_matLocalInertia );
	}

	if( m_ActorFlag & JL_ACTOR_STATIC )
		SetStatic();

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		SetKinematic();
}


void CJL_PhysicsActor::ReleaseShapes()
{
	size_t i;
	for( i=0; i<m_vecpShape.size(); i++ )
	{
		switch( m_vecpShape[i]->m_sShape )
		{
		case JL_SHAPE_BOX:
			CJL_Shape_Box *pBox;
			pBox = (CJL_Shape_Box *)m_vecpShape[i];
			m_pPhysicsManager->m_BoxShape.Release( pBox );
			break;
		case JL_SHAPE_TRIANGLEMESH:	// right now, static mesh only
			CTriangleMesh *pTriMesh;
			pTriMesh = (CTriangleMesh *)m_vecpShape[i];
			m_pPhysicsManager->m_TriangleMesh.Release( pTriMesh );
			break;
		}
	}
	m_vecpShape.resize( 0 );
}

/*
bool CJL_PhysicsActor::ClipTrace( const Vector3& vStartPos,	// [in]
								  const Vector3& vGoalPos,	// [in]
								  Vector3& vEndPos,			// [out]
								  float& fFraction			// [out]
								  )
{
	Vector3 vCurrentGoal = vGoalPos;
	Vector3 vCurrentEnd = vGoalPos;
	float frac = 1.0f;
	float fEndFrac = 1.0f;

	int i, num_shapes = m_vecpShape.size();
	for( i=0; i<num_shapes; i++ )
	{
		if( m_vecpShape[i]->ClipTrace( vStartPos, vCurrentGoal, vCurrentEnd, frac ) )
		{
			// trace has hit the shape
			vCurrentGoal = vCurrentEnd;
			fEndFrac *= frac;
			frac = 1.0f;
		}
	}

	vEndPos = vCurrentEnd;
	fFraction = fEndFrac;

	if( fEndFrac < 1.0f )
		return true;
	else
		return false;
}*/
