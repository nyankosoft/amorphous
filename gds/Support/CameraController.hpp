#ifndef  __CameraController_H__
#define  __CameraController_H__


#include "CameraControllerBase.hpp"
#include "gds/Input/InputHandler.hpp"
#include <boost/shared_ptr.hpp>


class CCameraControllerInputHandler;


class CCameraController : public CCameraControllerBase
{
	int m_InputHandlerIndex;

	boost::shared_ptr<CCameraControllerInputHandler> m_pInputHandler;

public:

	CCameraController( int input_handler_index );

	CCameraController( CInputHandler *pParentInputHandler );

	~CCameraController();

	bool IsKeyPressed( int general_input_code );
};


class CCameraControllerInputHandler : public CInputHandler
{
	CCameraController *m_pCameraController;

public:

	CCameraControllerInputHandler( CCameraController *pCameraController )
		:
	m_pCameraController(pCameraController)
	{}

	void ProcessInput(SInputData& input);
};


#endif		/*  __CameraController_H__  */
