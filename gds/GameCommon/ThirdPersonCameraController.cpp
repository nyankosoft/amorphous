#include "ThirdPersonCameraController.hpp"
#include "../Graphics/3DGameMath.hpp"
#include "../Stage.hpp"

using namespace boost;


CThirdPersonCameraController::CThirdPersonCameraController()
{
	m_CameraOrientation.vel = Quaternion(Matrix33Identity());
	m_CameraOrientation.smooth_time = 0.1f;

	m_CameraPosition.vel = Vector3(1,1,1);
	m_CameraPosition.smooth_time = 0.1f;

	m_VerticalAngle.vel = 1.0f;
	m_VerticalAngle.target = 1.0f;
	m_VerticalAngle.current = 1.0f;
	m_VerticalAngle.smooth_time = 0.05f;
}


void CThirdPersonCameraController::Update( float dt )
{
	m_CameraOrientation.Update( dt );
	m_CameraPosition.Update( dt );
	m_VerticalAngle.Update( dt );

	shared_ptr<CCopyEntity> pEntity = m_TargetEntity.Get();
	if( !pEntity )
		return;

	Vector3 vEntityWorldPos = pEntity->GetWorldPosition();
	const Matrix33 matCamOrientation = m_CameraOrientation.current.ToRotationMatrix();
	Vector3 vInvCamDir = -matCamOrientation.GetColumn(2);

//	CInputState::Name input_state = ???->GetActionInputState( ACTION_CAMERA_ALIGN );
	bool close_up_camera = m_CloseUpCamera;//(input_state == CInputState::PRESSED) ? true : false;

	const float cam_shift = close_up_camera ? 1.5f : 3.8f;
	const float cam_up    = close_up_camera ? 0.5f : 1.0f;
	pEntity->GroupIndex = 10;
	pEntity->GetStage()->GetEntitySet()->SetCollisionGroup( 10, 15, false );
	STrace tr;
	Vector3 vStart = vEntityWorldPos + Vector3(0,1,0) * 1.2f;
	Vector3 vGoal
		= vStart
		+ vInvCamDir * cam_shift
		+ Vector3(0,1,0) * cam_up;
	tr.pvStart = &vStart;
	tr.pvGoal  = &vGoal;
	tr.GroupIndex = 15;
	tr.aabb.vMin = tr.aabb.vMax = Vector3(0,0,0);
	tr.SetAABB();
	pEntity->GetStage()->ClipTrace( tr );
	float frac = tr.fFraction;
	clamp( frac, 0.3f, 1.0f );
	Vector3 vCamPos
		= vStart
		+ ( vGoal - vStart ) * frac;

	m_CameraPosition.target = vCamPos;

	if( close_up_camera )
		m_CameraOrientation.target.FromRotationMatrix( pEntity->GetWorldPose().matOrient );

	Horizontalize( m_CameraOrientation.target );

	m_CurrentCameraOrientation
		= m_CameraOrientation.current.ToRotationMatrix()
		* Matrix33RotationX( m_VerticalAngle.current );
}


Matrix34 CThirdPersonCameraController::GetCameraPose() const
{
//	return Matrix34( m_CameraPosition.current, m_CameraOrientation.current.ToRotationMatrix() );
	return Matrix34( m_CameraPosition.current, m_CurrentCameraOrientation );
}


void CThirdPersonCameraController::SetCameraPose( const Matrix34& pose )
{
	m_CameraPosition.target    = m_CameraPosition.current    = pose.vPosition;
	m_CameraOrientation.target = m_CameraOrientation.current = Quaternion( pose.matOrient );
}
