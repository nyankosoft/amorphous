#include "MotionBlender.hpp"
#include "MotionPrimitiveBlender.hpp"
#include "MotionDatabase.hpp"
//#include "HumanoidMotionTable.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;
using namespace msynth;


CMotionBlender::CMotionBlender()
{
}


CMotionBlender::~CMotionBlender()
{
}


void CSteeringMotionBlender::SetDestDirection( Vector3 vDestDir )
{
	m_vDestDir = vDestDir;

	m_vDestDir.y = 0;

	if( 0.001f < Vec3LengthSq(m_vDestDir) )
		Vec3Normalize( m_vDestDir, m_vDestDir );
}

/*
/// \return signed angle between a & b. range: [-pi,pi]
static inline Scalar Vec3GetSignedAngleBetween( const Vector3& a, const Vector3& b )
{
	return Vec3GetAngleBetween(a,b) * ( 0 <= ? 
}
*/

void CSteeringMotionBlender::Update( float dt )
{
	if( fabs(m_fSteeringSpeed) < 0.001 )
		return;

	// update steering
	const Matrix34& current_horizontal_root_pose = m_pMotionPrimitiveBlender->GetHorizontalCurrentRootPose();

	Vector3 vCurrentDir = current_horizontal_root_pose.matOrient.GetColumn(2);

	// calculate maximum angle that has to be covered

	Scalar max_angle_to_cover = 0;

	if( 0.001 < Vec3LengthSq( m_vDestDir ) )
	{
		// steer to the dest direction
		Vec3GetAngleBetween( vCurrentDir, m_vDestDir );

		if( max_angle_to_cover < 0.05f )
			return;
	}
	else
	{
		// no dest direction has been given
		// - just turn at the current turn speed
		max_angle_to_cover = 1000.0f;
	}

	Scalar delta_angle = m_fSteeringSpeed * dt;
	Limit( delta_angle, -max_angle_to_cover, max_angle_to_cover );

	delta_angle *= ( 0 <= Vec3Cross(vCurrentDir,m_vDestDir).y ) ? 1.0f : -1.0f;

	m_pMotionPrimitiveBlender->RotateCurrentPoseHorizontally( delta_angle );
}


void CSteeringMotionBlender::CalculateKeyframe( CKeyframe& dest_keyframe )
//void CSteeringMotionBlender::Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt )
{
/*
	Vector3 vCurrentHorizontalDir = GetRootPoseHorizontalDirection( dest_keyframe );

	if( Vec3LengthSq( vCurrentHorizontalDir ) < 0.001f )
		return;

	// angle between vCurrentDir and m_DestDir
	float max_delta_angle = (float)acos( Vec3Dot( vCurrentHorizontalDir, m_vDestDir ) );

	float delta_angle = m_fSteeringSpeed * 0.01f;//dt;

	Limit( delta_angle, -max_delta_angle, max_delta_angle );

	// change horizontal orientation of the current_keyframe

	Matrix34 steering_trans( Vector3(0,0,0), Matrix33RotationY(delta_angle) );

	Matrix34& rRootPose = dest_keyframe.GetRootPose();

	rRootPose = rRootPose * steering_trans;
*/
}


} // namespace amorphous
