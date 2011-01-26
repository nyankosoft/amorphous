#ifndef __ThirdPersonCameraController_HPP__
#define __ThirdPersonCameraController_HPP__


#include "../Stage/EntityHandle.hpp"
#include "../3DMath/Quaternion.hpp"
#include "../GameCommon/CriticalDamping.hpp"


class CThirdPersonCameraController
{
	CEntityHandle<> m_TargetEntity;

	cdv<Vector3> m_CameraPosition;

	cdv<Quaternion> m_CameraOrientation;

	bool m_CloseUpCamera;

public:

	CThirdPersonCameraController();
	~CThirdPersonCameraController(){}

	void Update( float dt );

	Matrix34 GetCameraPose() const;

	void SetCameraPose( const Matrix34& pose );

	void EnableCloseUpCamera( bool enable ) { m_CloseUpCamera = enable; }

	void SetTargetEntity( CEntityHandle<> target ) { m_TargetEntity = target; }
};


#endif /* __ThirdPersonCameraController_HPP__ */
