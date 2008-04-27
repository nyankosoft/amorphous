#ifndef  __SHOCKWAVECAMERAEFFECT_H__
#define  __SHOCKWAVECAMERAEFFECT_H__


#include "3DMath/Vector3.h"
#include "3DMath/Matrix33.h"


class CShockWaveCameraEffect
{
	Vector3 m_vForce;

	Vector3 m_vVelocity;
	Vector3 m_vPosition;

	Vector3 m_vTargetPosition;

	Matrix33 m_CameraOrient;

	/// down counter decreased by the elapsed time every frame
	/// when it becomes zero, offset is applied to the target position
	/// and the counter is set to a certain value
	float m_fTargetOffsetCounter;

	float m_fTargetOffsetFactor;

	float m_fMass;

	float m_fSpring;
	float m_fDamper;

	float m_fTimeStep;
	float m_fOverlapTime;

public:

	CShockWaveCameraEffect();

	void Update( float fFrameTime );

	inline void AddForce( const Vector3 vForce ) { m_vForce += vForce; m_fTargetOffsetFactor = 1.0f; }

	inline void AddVelocity( const Vector3 vVelocity ) { m_vVelocity += vVelocity; }

	inline Vector3 GetPosition() const { return m_vPosition; }

	inline float GetMass() const { return m_fMass; }
	inline float GetSpringConst() const { return m_fSpring; }
	inline float GetDamperConst() const { return m_fDamper; }

	inline void SetMass( const float mass ) { m_fMass = mass; }
	inline void SetSpringConst( const float spring ) { m_fSpring = spring; }
	inline void SetDamperConst( const float damper) { m_fDamper = damper; }

	inline void SetCameraOrientation( const Matrix33& matOrient ) { m_CameraOrient = matOrient; }
};


#endif  /*  __SHOCKWAVECAMERAEFFECT_H__  */
