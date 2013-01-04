namespace amorphous
{

//==============================================================
// SetDefault
//==============================================================
inline void CJL_PhysicsActor::SetDefault()
{
	m_vPosition = Vector3(0,0,0);
	m_matOrientation = Matrix33Identity();
	m_vAngularVelocity = Vector3(0,0,0);
	m_vVelocity = Vector3(0,0,0);

	SetInvMass(0.0f);
	SetLocalInvInertia(0.0f, 0.0f, 0.0f);

	m_ActorFlag = false;

	m_vForce = Vector3(0,0,0);
	m_vTorque = Vector3(0,0,0);

	m_ActivityState = ACTIVE;
	m_inactive_time = 0.0f;
	m_deactivation_time = 1.0f;
//	SetActivityThreshold(0.3f, 20.0f);
	SetActivityThreshold(0.5f, 30.0f);	// jiglib-0.70
	SetDeactivationThreshold(0.1f, 0.2f);	// jiglib-0.70
	SetDeactivationThreshold(0.2f, 0.3f);	// my test
//	m_allow_freezing = true;
	m_sqrActivationFactor = 1.0f;

	m_vLastPositionForDeactivation = m_vPosition;
	m_qLastOrientForDeactivation.FromRotationMatrix( m_matOrientation );

	m_iCollisionGroup = 0;

	m_bDoShockProcessing = true;

	m_bVelocityChanged = true;

	CopyCurrentStateToOld();
}


//==============================================================
// AddMovementActivation
//==============================================================
inline void CJL_PhysicsActor::AddMovementActivation(const Vector3 & pos, CJL_PhysicsActor * pOther)
{
  const size_t num_bodies = m_vecpActorsToBeActivatedOnMovement.size();
  size_t i;
  for (i = 0 ; i < num_bodies ; ++i)
  {
    if (m_vecpActorsToBeActivatedOnMovement[i] == pOther)
      return; // already made a note of this body
  }
  if (num_bodies == 0)
    m_vStoredPositionForActivation = pos;

  m_vecpActorsToBeActivatedOnMovement.push_back( pOther );
}


//==============================================================
// TryToFreeze
//==============================================================
inline void CJL_PhysicsActor::TryToFreeze(Scalar dt)
{
	if ( (!m_allow_freezing) || (m_ActorFlag & JL_ACTOR_STATIC) )
		return;

	if (m_ActivityState == FROZEN)
	{
//		m_inactive_time += dt;	// jiglib-0.70
		return;
	}

	if( m_fSqDeltaPosThreshold < Vec3LengthSq( m_vPosition - m_vLastPositionForDeactivation ) )
	{
		m_vLastPositionForDeactivation = m_vPosition;
		m_inactive_time = 0.0f;
		return;
	}

	Quaternion qOrient;
	qOrient.FromRotationMatrix( m_matOrientation );
	Quaternion qDelta = qOrient - m_qLastOrientForDeactivation;

	if( m_fSqDeltaQuatOrientThreshold < qDelta.GetLengthSq() )
	{
		m_qLastOrientForDeactivation = qOrient;
		m_inactive_time = 0.0f;
		return;
	}

  // active - check the thresholds
	if ( ShouldBeActive() )
	{
//		m_inactive_time = 0.0f;
		// let the inactivity timer continue
		return;
	}

	m_inactive_time += dt;

	if (m_inactive_time > m_deactivation_time)
	{
		// sleep!
		m_qLastOrientForDeactivation.FromRotationMatrix( m_matOrientation );
		m_vLastPositionForDeactivation = m_vPosition;

		SetActivityState(FROZEN);
	}
}

//==============================================================
// SetOrientation
//==============================================================
inline void CJL_PhysicsActor::SetOrientation(const Matrix33 & orient) 
{ 
  m_matOrientation = orient; 
  m_matInvOrientation = Matrix33Transpose(m_matOrientation);
  m_matWorldInvInertia = m_matOrientation * m_matLocalInvInertia * m_matInvOrientation;
  m_matWorldInertia = m_matOrientation * m_matLocalInertia * m_matInvOrientation;
}


//==============================================================
// GetPointVelocity
//==============================================================

inline Vector3 CJL_PhysicsActor::GetPointVelocity(const Vector3 & vWorldPoint ) 
{
	Vector3 v = Vec3Cross( GetAngularVelocity(), vWorldPoint - GetPosition() );
	v += GetVelocity();
	return v;
}

inline void CJL_PhysicsActor::GetPointVelocity( Vector3 & vPointVelocity, const Vector3 & vWorldPoint ) 
{
	vPointVelocity = Vec3Cross( GetAngularVelocity(), vWorldPoint - GetPosition() );
	vPointVelocity += GetVelocity();
}

//==============================================================
// GetLocalPointVelocity
//==============================================================

inline Vector3 CJL_PhysicsActor::GetLocalPointVelocity(const Vector3 & vLocalPoint ) 
{
	return GetVelocity() + Vec3Cross( GetAngularVelocity(), vLocalPoint );
}


inline void CJL_PhysicsActor::GetLocalPointVelocity( Vector3 & vPointVelocity, const Vector3 & vLocalPoint ) 
{
	vPointVelocity = Vec3Cross( GetAngularVelocity(), vLocalPoint );
	vPointVelocity += GetVelocity();
}


#ifndef FINAL
#define CHECK_RIGID_BODY
#endif

//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void CJL_PhysicsActor::ApplyWorldImpulse(const Vector3 & impulse)
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		return;

#ifdef CHECK_RIGID_BODY
	Vector3 origVelocity = m_vVelocity;
#endif

	m_vVelocity += impulse * m_fInvMass ;
	m_bVelocityChanged = true;

#ifdef CHECK_RIGID_BODY
  if ( !IsSensible(m_vVelocity) )
  {
/*    TRACE("Velocity is not sensible after impulse: this = %p\n", this);
    origVelocity.show("orig vel");
    impulse.show("impulse");
    m_vVelocity.show("velocity");
    while (1) {}*/
  }
#endif
}


//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void CJL_PhysicsActor::ApplyBodyWorldImpulse( const Vector3& impulse, const Vector3& delta )
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		return;

#ifdef CHECK_RIGID_BODY
  Vector3 origVelocity = m_vVelocity;
  Vector3 origAngularVelocity = m_vAngularVelocity;
#endif

	m_vVelocity += impulse * m_fInvMass ;
	m_vAngularVelocity += m_matWorldInvInertia * Vec3Cross( delta, impulse );
	m_bVelocityChanged = true;

#ifdef CHECK_RIGID_BODY
  if ( !IsSensible(m_vAngularVelocity) )
  {
/*    TRACE("rotation is not sensible after impulse: this = %p\n", this);
    origAngularVelocity.show("orig vel");
    impulse.show("impulse");
    m_vAngularVelocity.show("rotation");
    while (1) {}*/
  }
  if ( !IsSensible(m_vVelocity) )
  {
/*    TRACE("Velocity is not sensible after impulse: this = %p\n", this);
    origVelocity.show("orig vel");
    impulse.show("impulse");
    m_vVelocity.show("velocity");
    while (1) {}*/
  }
#endif
}


//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void CJL_PhysicsActor::ApplyWorldImpulse(const Vector3& impulse, const Vector3& pos)
{
	ApplyBodyWorldImpulse( impulse, pos - GetPosition() );
}


//==============================================================
// ApplyWorldAngularImpulse
//==============================================================
inline void CJL_PhysicsActor::ApplyWorldAngularImpulse(const Vector3 & angImpulse)
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	m_vAngularVelocity += m_matWorldInvInertia * angImpulse;
	m_bVelocityChanged = true;
}


inline void CJL_PhysicsActor::UpdateWorldProperties()
{
	m_WorldAABB.Nullify();

	for( size_t i=0; i<m_vecpShape.size(); i++ )
	{
		m_vecpShape[i]->UpdateWorldProperties();
		m_WorldAABB += m_vecpShape[i]->GetWorldAABB();
	}
}


//==============================================================
// CopyCurrentStateToOld
//==============================================================
inline void CJL_PhysicsActor::CopyCurrentStateToOld()
{
	m_OldWorldPose.vPosition = m_vPosition;
	m_OldWorldPose.matOrient = m_matOrientation;
	m_vOldVelocity = m_vVelocity;
	m_vOldAngularVelocity = m_vAngularVelocity;

	for( size_t shape = m_vecpShape.size(); shape-- != 0 ; )
	{
		m_vecpShape[shape]->CopyCurrentWorldPoseToOld();
	}
}


//==============================================================
// StoreState
//==============================================================
inline void CJL_PhysicsActor::StoreState()
{
	m_StoredWorldPose.vPosition	= m_vPosition;
	m_StoredWorldPose.matOrient	= m_matOrientation;
	m_vStoredVelocity			= m_vVelocity;
	m_vStoredAngularVelocity	= m_vAngularVelocity;
}


//==============================================================
// RestoreState
//==============================================================
inline void CJL_PhysicsActor::RestoreState()
{
	m_vPosition			= m_StoredWorldPose.vPosition;
	m_matOrientation	= m_StoredWorldPose.matOrient;
	m_vVelocity			= m_vStoredVelocity;
	m_vAngularVelocity	= m_vStoredAngularVelocity;
}

} // namespace amorphous
