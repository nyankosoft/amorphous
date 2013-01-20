#ifndef  __CameraController_H__
#define  __CameraController_H__


#include "CameraControllerBase.hpp"
#include "gds/Input/InputHandler.hpp"
#include <boost/shared_ptr.hpp>


namespace amorphous
{


class CameraControllerInputHandler;


class CameraController : public CameraControllerBase
{
	int m_InputHandlerIndex;

	boost::shared_ptr< CInputDataDelegate<CameraController> > m_pInputDataDelagate;

public:

	CameraController( int input_handler_index );

	CameraController( CInputHandler *pParentInputHandler );

	~CameraController();

	bool IsKeyPressed( int general_input_code );
};


} // namespace amorphous



#endif		/*  __CameraController_H__  */
