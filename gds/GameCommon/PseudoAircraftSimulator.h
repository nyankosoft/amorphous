#ifndef  __PSEUDOAIRCRAFTSIMULATOR_H__
#define  __PSEUDOAIRCRAFTSIMULATOR_H__


#include "3DMath/Matrix34.h"
#include "3DMath/MathMisc.h"

#include <vector>
#include <string>


#include "GameCommon/CriticalDamping.h"


class CPseudoAircraftSimulator
{
	Matrix34 m_WorldPose;

	class CControlProperty
	{
	public:
//		float m_fAccel;
		cdv<float> m_Accel;
		float m_fSpeed;

//		float GetCurrentAccel() { return m_fAccel; }
//		float& TargetAccel() { return m_fAccel; }
		float GetCurrentAccel() const { return m_Accel.current; }
		float& TargetAccel() { return m_Accel.target; }

//		CControlProperty() : m_fAccel(0), m_fSpeed(0) {}
		CControlProperty() : m_fSpeed(0) { m_Accel.SetZeroState(); }
	};

	CControlProperty m_Forward;

	CControlProperty m_Pitch;

	CControlProperty m_Roll;

	CControlProperty m_Yaw;

	float m_fYaw, m_fPitch, m_fRoll;

	Vector3 m_vLastValidHrzFwdDir;

	Vector3 m_vLastValidHrzRightDir;

private:

	inline void UpdateHorizontalForwardDirection();

	inline void UpdateYawPitchRoll();

public:

	CPseudoAircraftSimulator();
	~CPseudoAircraftSimulator();

	/// dt must be small enough for critical dampings
	/// critical dampings are used by accels
	void Update( float dt );

	inline const Matrix34& GetWorldPose() const { return m_WorldPose; }

	inline const Matrix33& GetWorldOrient() const { return m_WorldPose.matOrient; }

	inline float GetAltitude() const { return m_WorldPose.vPosition.y; }

	/// TODO: correct velocity calculation
	inline Vector3 GetVelocity() const { return GetWorldPose().matOrient.GetColumn(2) * m_Forward.m_fSpeed; }

	/// range: [-PI, PI]
	inline float GetYaw() const { return m_fYaw; }

	/// range: [-PI/2, PI/2]
	inline float GetPitch() const { return m_fPitch; }

	inline float GetRoll() const { return m_fRoll; }


	inline float GetPitchAccel() const { return m_Pitch.GetCurrentAccel(); }
		   
	inline float GetRollAccel() const { return m_Roll.GetCurrentAccel(); }

	inline float GetYawAccel() const { return m_Yaw.GetCurrentAccel(); }


	inline Vector3 GetHorizontalForwardDirection() const { return m_vLastValidHrzFwdDir; }

	inline Vector3 GetHorizontalRightDirection() const { return m_vLastValidHrzRightDir; }

	inline void SetWorldPose( const Matrix34& world_pose ) { m_WorldPose = world_pose; }

	/// TODO: correct velocity calculation
	inline void SetForwardVelocity( const Vector3& velocity );

	void SetAccel( float accel );

	inline void SetPitchAccel( float accel ) { m_Pitch.TargetAccel() = accel; }

	inline void SetRollAccel( float accel ) { m_Roll.TargetAccel() = accel; }

	inline void SetYawAccel( float accel ) { m_Yaw.TargetAccel() = accel; }

	/// resets simulator
	/// - set world pose to identity
	/// - set accels and speeds for forward, yaw, pitch & roll to 0
	void ResetSimulator();

	void GetStatus( std::vector<std::string>& rDestBuffer );
};


//===============================/ inline implementations /===============================//


inline void CPseudoAircraftSimulator::UpdateHorizontalForwardDirection()
{
	Vector3 vDirH, vDir = m_WorldPose.matOrient.GetColumn(2);

	if( 0.999f < fabsf(vDir.y) ) 
	{
		return;	// keep the current last valid HFD
	}
	else
	{
		vDirH = vDir;
		vDirH.y = 0;
		Vec3Normalize( vDirH, vDirH );
		m_vLastValidHrzFwdDir = vDirH;

		Matrix33 matRotY90 = Matrix33RotationY( (Scalar)PI * 0.5f );
		m_vLastValidHrzRightDir = matRotY90 * vDirH;
	}
}


inline float acos_safe( float x )
{
	static const float x_margin = 0.0001f;
	Limit( x, -1.0f + x_margin, 1.0f - x_margin );

	return (float)acos(x);
}


inline void CPseudoAircraftSimulator::UpdateYawPitchRoll()
{
	Vector3 vDirH, vDir = m_WorldPose.matOrient.GetColumn(2);
	float dp;
/*
	if( 0.999f < fabsf(vDir.y) ) 
		dp = 0.0f;
	else
	{
		vDirH = vDir;
		vDirH.y = 0;
		Vec3Normalize( vDirH, vDirH );
		dp = Vec3Dot( vDir, vDirH );
	}*/

	vDirH = GetHorizontalForwardDirection();
	dp = Vec3Dot( vDir, vDirH );

	m_fPitch = acos_safe(dp) * ( 0 < vDir.y ? -1.0f : 1.0f );

//	if( vDir.y < 0 )
//		pitch *= (-1.0f);

	Vector3 vRightDir = GetWorldOrient().GetColumn(0);
	Vector3 vRightDirH = Matrix33RotationY((Scalar)PI * 0.5f) * vDirH;
	dp = Vec3Dot( vRightDir, vRightDirH );
	
	m_fRoll = acos_safe(dp) * ( 0 < vRightDir.y ? 1.0f : -1.0f );

	dp = Vec3Dot( Vector3(0,0,1), vDirH );
	m_fYaw = acos_safe(dp) * ( 0 < vDirH.x ? 1.0f : -1.0f );
}


inline void CPseudoAircraftSimulator::SetForwardVelocity( const Vector3& velocity )
{
	m_Forward.m_fSpeed = Vec3Dot( GetWorldPose().matOrient.GetColumn(2), velocity );
}


#endif		/*  __PSEUDOAIRCRAFTSIMULATOR_H__  */
