#ifndef  __CameraController_H__
#define  __CameraController_H__


#include "CameraControllerBase.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include <memory>


namespace amorphous
{


class CameraControllerInputHandler;


class CameraController : public CameraControllerBase
{
	int m_InputHandlerIndex;

	std::shared_ptr< CInputDataDelegate<CameraController> > m_pInputDataDelagate;

public:

	CameraController( int input_handler_index );

	CameraController( InputHandler *pParentInputHandler );

	~CameraController();

	bool IsKeyPressed( int general_input_code );

	void SetActive( bool active );
};


} // namespace amorphous



#endif		/*  __CameraController_H__  */
