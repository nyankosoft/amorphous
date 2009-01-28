#include "ShockWaveCameraEffect.h"

#include "3DMath/Vector3.h"

#include "GameCommon/BasicGameMath.h"
#include "Support/VectorRand.h"
#include "GameCommon/MTRand.h"


CShockWaveCameraEffect::CShockWaveCameraEffect()
{
	m_fOverlapTime = 0;
	m_fTimeStep = 0.02f;


	// feel like a spring
	// apparently stable
//		m_fMass = 0.6f;
//		m_fSpring = 1250.0f;
//		m_fDamper = 5.0f;

	// test for target position offset
	m_fMass = 1.0f;
	m_fSpring = 300.0f;
//	m_fDamper = 15.0f;
	m_fDamper = 2.0f * sqrtf( m_fMass * m_fSpring );

	// for recoils
//	m_fMass = 1.0f;
//	m_fSpring = 720.0f;
//	m_fDamper = 16.0f;

	// feel like a stiff spring
//		m_fMass = 0.6f;
//		m_fSpring = 1250.0f;
//		m_fDamper = 20.0f;

	// too soft
	// tend to diverge
//		m_fMass = 0.6f;
//		m_fSpring = 30.0f;
//		m_fDamper = 5.5f;

//		m_fMass   = 0.9f;
//		m_fSpring = 250.0f;
//		m_fDamper = 25.0f;

	m_vPosition = Vector3(0,0,0);
	m_vVelocity = Vector3(0,0,0);
	m_vForce    = Vector3(0,0,0);
	m_vTargetPosition = Vector3(0,0,0);

	m_CameraOrient.SetIdentity();

	m_fTargetOffsetFactor = 0;
	m_fTargetOffsetCounter = 0;
}
//-----------------------------------------------------------------------------
// Name: Update()
// Desc: 
//-----------------------------------------------------------------------------

void CShockWaveCameraEffect::Update( float fFrameTime )
{
	int i, num_loops;
	float dt = m_fTimeStep;

	float fTotalTime = fFrameTime + m_fOverlapTime;

	num_loops = (int) (fTotalTime / dt);

	m_fOverlapTime = fTotalTime - num_loops * dt;

    Vector3 vDist, vForce;

///	m_vTargetPosition = Vector3(0,0,0);

	/// target position noise test1 - simply offset from the original position
	m_fTargetOffsetCounter -= fFrameTime;
	if( m_fTargetOffsetCounter < 0 )
	{
		m_fTargetOffsetCounter += RangedRand( 0.20f, 0.25f );
		m_vTargetPosition = Vec3RandDir()
			              * RangedRand( 0.2f, 0.3f )
						  * m_fTargetOffsetFactor;
		m_vTargetPosition.z *= 0.1f;
//		m_vTargetPosition.x *= 0.5f;

	}

	/// target position noise test2 - use sin/cos
	float fOffset = timeGetTime() / 300.0f;
	float x,y;
	x = cos(fOffset * 1.2f) * sin(fOffset * 1.8f) * 0.25f;
	y = cos(fOffset * 1.5f) * sin(fOffset * 1.4f) * 0.25f;

/*	m_vTargetPosition.x = x;
	m_vTargetPosition.y = y;
	m_vTargetPosition.z = 0;*/
	m_vTargetPosition = m_CameraOrient.GetColumn(0) * x + m_CameraOrient.GetColumn(1) * y;
	m_vTargetPosition *= m_fTargetOffsetFactor;


	m_fTargetOffsetFactor -= fFrameTime;
	if( m_fTargetOffsetFactor < 0 )
		m_fTargetOffsetFactor = 0;

	if( num_loops == 0 )
		return;

	Vector3 vAcmForce = m_vForce / (float)num_loops;

	for( i=0; i<num_loops; i++ )
	{
		Vector3& vVel = m_vVelocity;
		vDist = m_vTargetPosition - m_vPosition;
//		vDist = m_vTargetPosition - vCurrentPosition;

		vForce = vDist * m_fSpring - vVel * m_fDamper + vAcmForce;

//		vForce += Vec3RandDir()
//			* RangedRand( 0.8f, 1.2f )
//			* Vec3LengthSq( &vDist ) * 12.5f;

		vVel += vForce / m_fMass  * dt;

		m_vPosition += vVel * dt;
	}

	m_vForce = Vector3(0,0,0);
}
