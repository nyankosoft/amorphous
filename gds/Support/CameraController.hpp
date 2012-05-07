#ifndef  __CameraController_H__
#define  __CameraController_H__


#include "CameraControllerBase.hpp"
#include "gds/Input/InputHandler.hpp"
#include <boost/shared_ptr.hpp>


class CCameraControllerInputHandler;


class CCameraController : public CCameraControllerBase
{
	int m_InputHandlerIndex;

	boost::shared_ptr< CInputDataDelegate<CCameraController> > m_pInputDataDelagate;

public:

	CCameraController( int input_handler_index );

	CCameraController( CInputHandler *pParentInputHandler );

	~CCameraController();

	bool IsKeyPressed( int general_input_code );
};



#endif		/*  __CameraController_H__  */
