/*
//==============================================================
// SetDefault
//==============================================================
inline void CActor::SetDefault()
{
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
}
*/

/*
//==============================================================
// SetOrientation
//==============================================================
inline void CActor::SetOrientation(const Matrix33 & orient) 
{ 
  m_matOrientation = orient; 
  m_matInvOrientation = Matrix33Transpose(m_matOrientation);
  m_matWorldInvInertia = m_matOrientation * m_matLocalInvInertia * m_matInvOrientation;
  m_matWorldInertia = m_matOrientation * m_matLocalInertia * m_matInvOrientation;
}


//==============================================================
// GetPointVelocity
//==============================================================

inline Vector3 CActor::GetPointVelocity(const Vector3 & vWorldPoint ) 
{
	Vector3 v = Vec3Cross( GetAngularVelocity(), vWorldPoint - GetPosition() );
	v += GetVelocity();
	return v;
}

inline void CActor::GetPointVelocity( Vector3 & vPointVelocity, const Vector3 & vWorldPoint ) 
{
	vPointVelocity = Vec3Cross( GetAngularVelocity(), vWorldPoint - GetPosition() );
	vPointVelocity += GetVelocity();
}

//==============================================================
// GetLocalPointVelocity
//==============================================================

inline Vector3 CActor::GetLocalPointVelocity(const Vector3 & vLocalPoint ) 
{
	return GetVelocity() + Vec3Cross( GetAngularVelocity(), vLocalPoint );
}


inline void CActor::GetLocalPointVelocity( Vector3 & vPointVelocity, const Vector3 & vLocalPoint ) 
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
inline void CActor::ApplyWorldImpulse(const Vector3 & impulse)
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		return;

#ifdef CHECK_RIGID_BODY
...
#endif

	m_vVelocity += impulse * m_fInvMass ;
	m_bVelocityChanged = true;

#ifdef CHECK_RIGID_BODY
...
#endif
}


//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void CActor::ApplyBodyWorldImpulse( const Vector3& impulse, const Vector3& delta )
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	if( m_ActorFlag & JL_ACTOR_KINEMATIC )
		return;

#ifdef CHECK_RIGID_BODY
...
#endif

	m_vVelocity += impulse * m_fInvMass ;
	m_vAngularVelocity += m_matWorldInvInertia * Vec3Cross( delta, impulse );
	m_bVelocityChanged = true;

#ifdef CHECK_RIGID_BODY
...
#endif
}


//==============================================================
// ApplyWorldImpulse
//==============================================================
inline void CActor::ApplyWorldImpulse(const Vector3& impulse, const Vector3& pos)
{
	ApplyBodyWorldImpulse( impulse, pos - GetPosition() );
}


//==============================================================
// ApplyWorldAngularImpulse
//==============================================================
inline void CActor::ApplyWorldAngularImpulse(const Vector3 & angImpulse)
{
	if( GetActivityState() == FROZEN )
		SetActivityState( ACTIVE );

	m_vAngularVelocity += m_matWorldInvInertia * angImpulse;
	m_bVelocityChanged = true;
}


inline void CActor::UpdateWorldProperties()
{
	m_WorldAABB.Nullify();

	for( size_t i=0; i<m_vecpShape.size(); i++ )
	{
		m_vecpShape[i]->UpdateWorldProperties();
		m_WorldAABB += m_vecpShape[i]->GetWorldAABB();
	}
}
*/
