
#include "Actor.hpp"
#include "3DMath/MathMisc.hpp"


using namespace physics;


//bool CActor::m_indicate_frozen_objects = true;

/*
void CActor::AddWorldForce(const Vector3 & force, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}

void CActor::AddWorldTorque(const Vector3 & torque, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}

void CActor::AddLocalForce(const Vector3 & force)
{
//  assert1(!"pure virtual function!");
}

void CActor::AddLocalForce(const Vector3 & force, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}

void CActor::AddLocalTorque(const Vector3 & torque)
{
//  assert1(!"pure virtual function!");
}

void CActor::AddLocalTorque(const Vector3 & torque, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}

void CActor::ApplyLocalImpulse(const Vector3 & impulse)
{
//  assert1(!"pure virtual function!");
}

void CActor::ApplyLocalImpulse(const Vector3 & impulse, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}

void CActor::ApplyLocalAngularImpulse(const Vector3 & ang_impule)
{
//  assert1(!"pure virtual function!");
}

void CActor::ApplyLocalAngularImpulse(const Vector3 & angImpulse, const Vector3 & pos)
{
//  assert1(!"pure virtual function!");
}
*/

/*
//==============================================================
// clear_all_forces
//==============================================================
void CActor::ClearForces()
{
  m_vForce = Vector3(0,0,0);
  m_vTorque = Vector3(0,0,0);
}*/

/*
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
*/

/*
//==============================================================
// state_to_str
//==============================================================
static const char * state_to_str(CActor::Activity state)
{
  switch (state)
  {
  case CActor::ACTIVE: return "ACTIVE";
  case CActor::FROZEN: return "FROZEN";
  }
  return "Invalid state";
}

//==============================================================
// set_deactivation_time
//==============================================================
void CActor::SetDeactivationTime(Scalar seconds)
{
	m_deactivation_time = seconds;
}

//==============================================================
// SetActivityThreshold
//==============================================================
void CActor::SetActivityThreshold(Scalar vel, Scalar rot)
{
  m_sqrVelocityActivityThreshold = vel * vel;
  m_sqrAngularVelocityActivityThreshold = deg_to_rad(rot) * deg_to_rad(rot);
}

//==============================================================
// SetActivityThreshold
//==============================================================
void CActor::SetDeactivationThreshold( Scalar fPosThreshold, Scalar fOrientTheshold )
{
  m_fSqDeltaPosThreshold = fPosThreshold * fPosThreshold;
  m_fSqDeltaQuatOrientThreshold = fOrientTheshold * fOrientTheshold;	//deg_to_rad(rot) * deg_to_rad(rot);
}

//==============================================================
// setActivationFactor
//==============================================================
void CActor::SetActivationFactor(Scalar factor)
{
  m_sqrActivationFactor = factor * factor;
}

//==============================================================
// SetActivityState
//==============================================================
void CActor::SetActivityState(Activity state, Scalar activityFactor)
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
}*/

/*
//==============================================================
// SetAllowFreezing
//==============================================================
void CActor::SetAllowFreezing(bool allow)
{
  m_allow_freezing = allow;
  SetActivityState(ACTIVE);
}

//==============================================================
// MoveTo
//==============================================================
void CActor::MoveTo(const Vector3 & pos)
{
  if (GetActivityState() == FROZEN)
  {
    m_pPhysicsManager->ActivateObject(this);
  }
  SetPosition(pos);
}
*/

/// the following functions were inline in the original jiglib

/*
//==============================================================
// LimitVel
//==============================================================
void CActor::LimitVel()
{
	Scalar VelMax = 100.0f;

	Limit( m_vVelocity.x, -VelMax, VelMax );
	Limit( m_vVelocity.y, -VelMax, VelMax );
	Limit( m_vVelocity.z, -VelMax, VelMax );
}


//==============================================================
// LimitAngVel
//==============================================================
void CActor::LimitAngVel()
{
	Scalar AngVelMax = 100.0f;

	Limit( m_vAngularVelocity.x, -AngVelMax, AngVelMax );
	Limit( m_vAngularVelocity.y, -AngVelMax, AngVelMax );
	Limit( m_vAngularVelocity.z, -AngVelMax, AngVelMax );
}
*/


//==============================================================
// Init
//==============================================================

// now set up by each CScene

/*
void CActor::Init( CActorDesc &rActorDesc )
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

	// mass
	m_fMass = rActorDesc.fMass;
	m_fInvMass = 1.0f / m_fMass;

	m_ActorFlag = rActorDesc.ActorFlag;

	m_iCollisionGroup = rActorDesc.iCollisionGroup;

	size_t num_shapes = rActorDesc.vecpShapeDesc.size();

	// set shapes
	// ...

	if( m_ActorFlag & JL_ACTOR_STATIC )
		SetStatic();

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		SetKinematic();
}
*/

/*
bool CActor::ClipTrace( const Vector3& vStartPos,	// [in]
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
