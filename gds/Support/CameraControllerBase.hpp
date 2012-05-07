#ifndef  __CameraControllerBase_H__
#define  __CameraControllerBase_H__


#include "../Graphics/Camera.hpp"
#include "../Input/fwd.hpp"


class CameraControl
{
public:

	enum Operation
	{
		Forward,
		Backward,
		Right,
		Left,
		Up,
		Down,
		NumOperations
	};
};

/*

#include <boost/shared_ptr.hpp>


class CCameraControllerBase : public CCameraController
{
public:
};


class CCameraController_Linux: public CCameraController
{
public:
};


boost::shared_ptr<CCameraController> CreateCameraController()
{
	boost::shared_ptr<CCameraController> ptr;

#ifdef WIN32
	ptr = boost::shared_ptr<CCameraController>( new CCameraControllerBase() );
#else
	ptr = boost::shared_ptr<CCameraController>( new CCameraController_Linux() );
#endif

	return ptr;
}
*/

class CCameraControllerBase
{
	float m_fYaw;
	float m_fPitch;

//	Matrix34 m_Pose;

	CCamera m_Camera;

	/// camera translation speed (meter per second)
	float m_fTranslationSpeed;

	int m_CameraControlCode[CameraControl::NumOperations];

	enum MouseButton
	{
		MBTN_LEFT,
		MBTN_RIGHT,
		NUM_MOUSE_BUTTONS
	};

	int m_IsMouseButtonPressed[NUM_MOUSE_BUTTONS]; ///< 1: pressed / 0: released

protected:

	/// Move these variables CCameraController_Win32 if it's the only one
	/// who needs these.
	int m_iPrevMousePosX;
	int m_iPrevMousePosY;

public:

	CCameraControllerBase();

	virtual ~CCameraControllerBase();

	/// must be called every frame
	void UpdateCameraPose( float dt );

	virtual bool IsKeyPressed( int general_input_code );

	void AddYaw( float fYaw );

	void AddPitch( float fPitch );

	inline Vector3 GetPosition() const { return GetPose().vPosition; }

	inline Vector3 GetRightDirection() const   { return GetPose().matOrient.GetColumn(0); }
	inline Vector3 GetUpDirection() const      { return GetPose().matOrient.GetColumn(1); }
	inline Vector3 GetForwardDirection() const { return GetPose().matOrient.GetColumn(2); }

	inline void GetPose( Matrix34& rDestPose ) const { rDestPose = m_Camera.GetPose(); }

	inline const Matrix34 GetPose() const { return m_Camera.GetPose(); }

	inline void SetPosition( const Vector3& vPosition ) { m_Camera.SetPosition( vPosition ); }

	inline void SetPose( const Matrix34& rSrcPose ) { m_Camera.SetPose( rSrcPose ); }

	inline void Move( float fRight, float fUp, float fDir )
	{
		Matrix34 current_pose = GetPose();
		current_pose.vPosition
			+= current_pose.matOrient.GetColumn(0) * fRight
			+  current_pose.matOrient.GetColumn(1) * fUp
			+  current_pose.matOrient.GetColumn(2) * fDir;

		SetPose( current_pose );
	}

	void AssignKeyForCameraControl( int general_input_code, CameraControl::Operation cam_control_op );

	/// set the speed of the camera translation (meter per second)
	inline void SetTranslationSpeed( float meter_per_second ) { m_fTranslationSpeed = meter_per_second; }

	/// Handles mouse movements
	/// Don't use this with CPlatformDependentCameraController::HandleMessage( UINT, WPARAM, LPARAM ),
	/// otherwise the mouse input will be processed twice
	void HandleInput( const SInputData& input );
};



#endif		/*  __CameraControllerBase_H__  */
