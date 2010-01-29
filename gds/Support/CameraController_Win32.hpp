#ifndef  __CameraController_Win32_H__
#define  __CameraController_Win32_H__


#include <windows.h>
#include "CameraControllerBase.hpp"


class CPlatformDependentCameraController : public CCameraControllerBase
{
public:

	CPlatformDependentCameraController() {}

	~CPlatformDependentCameraController() {}

	bool IsKeyPressed( int general_input_code );

	/// Call this in window messege handler
    void HandleMessage( UINT msg, WPARAM wParam, LPARAM lParam );
};


#endif		/*  __CameraController_Win32_H__  */
