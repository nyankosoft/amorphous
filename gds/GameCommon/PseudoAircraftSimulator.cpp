
#include "PseudoAircraftSimulator.h"

#include "GameCommon/GameMathMisc.h"
#include "Support/Macro.h"

using namespace std;

//#define QFRIC_EPSILON	3.125f
#define QFRIC_EPSILON	0.25f


CPseudoAircraftSimulator::CPseudoAircraftSimulator()
{
	m_fYaw = m_fPitch = m_fRoll = 0.0f;

	m_WorldPose.Identity();

	m_Forward.m_fSpeed = 0.0f;

	m_vLastValidHrzFwdDir = GetWorldOrient().GetColumn(2);
	m_vLastValidHrzRightDir = GetWorldOrient().GetColumn(0);


	m_Forward.m_Accel.smooth_time = 0.08f;
	m_Yaw.m_Accel.smooth_time = 0.10f;
	m_Pitch.m_Accel.smooth_time = 0.08f;
	m_Roll.m_Accel.smooth_time = 0.08f;
}


CPseudoAircraftSimulator::~CPseudoAircraftSimulator()
{
}

/*
inline float GetControlFriction( float current, RangedSet& range )
{
	if( 
}
*/

void CPseudoAircraftSimulator::Update( float dt )
{
	m_Forward.m_Accel.Update( dt );
	m_Yaw.m_Accel.Update( dt );
	m_Pitch.m_Accel.Update( dt );
	m_Roll.m_Accel.Update( dt );

	Matrix34 prev_pose = m_WorldPose;

	m_Forward.m_fSpeed += m_Forward.GetCurrentAccel() * dt;

	ApplyQFriction( m_Forward.m_fSpeed, dt, 0.2f, QFRIC_EPSILON );

	m_WorldPose.vPosition += prev_pose.matOrient.GetColumn(2) * m_Forward.m_fSpeed * dt;

	// update pitch
	m_Pitch.m_fSpeed += m_Pitch.GetCurrentAccel() * dt;

//	float extra_friction = GetControlFriction( m_PitchRange );
//	ApplySQFriction( m_Pitch.m_fSpeed, dt, 2.0f + extra_friction, QFRIC_EPSILON );

//	or

//	float accel = current_pitch??? < max_pitch ? m_Pitch.m_fAccel : 0;
//	m_Pitch.m_fSpeed += accel * dt;

	float pitch_friction = 2.0f;
	ApplySQFriction( m_Pitch.m_fSpeed, dt, pitch_friction, QFRIC_EPSILON );

	Matrix33 matPitch = Matrix33RotationAxis( m_Pitch.m_fSpeed * dt, prev_pose.matOrient.GetColumn(0) );

	m_WorldPose.matOrient = matPitch * m_WorldPose.matOrient;

	// update roll
	m_Roll.m_fSpeed += m_Roll.GetCurrentAccel() * dt;

	float roll_friction = 2.0f;
	ApplySQFriction( m_Roll.m_fSpeed, dt, roll_friction, QFRIC_EPSILON );

	Matrix33 matRoll = Matrix33RotationAxis( m_Roll.m_fSpeed * dt, prev_pose.matOrient.GetColumn(2) );

	m_WorldPose.matOrient = matRoll * m_WorldPose.matOrient;

	// update yaw
	m_Yaw.m_fSpeed += m_Yaw.GetCurrentAccel() * dt;

	ApplySQFriction( m_Yaw.m_fSpeed, dt, 2.5f, QFRIC_EPSILON );

	Matrix33 matYaw = Matrix33RotationAxis( m_Yaw.m_fSpeed * dt * 0.1f, prev_pose.matOrient.GetColumn(1) );

	m_WorldPose.matOrient = matYaw * m_WorldPose.matOrient;

	// normalize orientation matrix
	m_WorldPose.matOrient.Orthonormalize();


	UpdateHorizontalForwardDirection();

	// update orientation information
	UpdateYawPitchRoll();
}


void CPseudoAircraftSimulator::SetAccel( float accel )
{
	m_Forward.TargetAccel() = accel;
}


void CPseudoAircraftSimulator::ResetSimulator()
{
	// reset pose
	m_WorldPose = Matrix34Identity();
	m_fYaw = 0;
	m_fPitch = 0;
	m_fRoll = 0;
	m_vLastValidHrzFwdDir   = Vector3(0,0,1);
	m_vLastValidHrzRightDir = Vector3(1,0,0);

	// reset velocities & accelerations
	CControlProperty *pControl[4] = { &m_Forward, &m_Yaw, &m_Pitch, &m_Roll };
	for( int i=0; i<numof(pControl); i++ )
	{
		pControl[i]->m_Accel.SetZeroState();
		pControl[i]->m_fSpeed = 0;
	}
}


void CPseudoAircraftSimulator::GetStatus( vector<string>& rDestTextBuffer )
{
	vector<string>& buffer = rDestTextBuffer;

	if( buffer.size() < 6 )
		buffer.resize( 6 );

	char str_buffer[256];

	Vector3 pos = m_WorldPose.vPosition;
	sprintf( str_buffer, "position: ( %.02f,  %.02f,  %.02f )", pos.x, pos.y, pos.z );
	buffer[0] = str_buffer;

	sprintf( str_buffer, "accel: %.02f", m_Forward.GetCurrentAccel() );
	buffer[1] = str_buffer;

	sprintf( str_buffer, "speed: %.02f", m_Forward.m_fSpeed );
	buffer[2] = str_buffer;

	sprintf( str_buffer, "pitch_accel: %.02f", m_Pitch.GetCurrentAccel() );
	buffer[3] = str_buffer;

	sprintf( str_buffer, "pitch_speed: %.02f", m_Pitch.GetCurrentAccel() );
	buffer[4] = str_buffer;

	sprintf( str_buffer, "pitch: %.02f[deg]", rad_to_deg(m_fPitch) );
	buffer[5] = str_buffer;

}

/*
class AircraftData
{
	float m_fPower;	// forward accel
	float m_fMaxPitchAccel;
	float m_fMaxYawAccel;
	float m_fMaxRollAccel;

	string m_str3DModelFilename;

	vector<string> m_vecDefaultWeaponName;
//	vector<CPA_Weapon> m_vecWeapon;
	vector<CPA_Weapon> m_vecWeapon;


	vector<CPA_WeaponBay> m_vecWeaponBay
};*/
