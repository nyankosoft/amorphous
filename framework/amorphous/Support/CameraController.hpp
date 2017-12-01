#ifndef  __CameraController_H__
#define  __CameraController_H__


#include "CameraControllerBase.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include <memory>


namespace amorphous
{


/**
 * \brief Enables the user to control a free flying first-person camera using a keyboard and a mouse
 *
 * - Stores the pose of a camera object.
 * - Takes input from keyboard and mouse, and calculates the pose, i.e. position and rotation, of the .s
 * - The control is based on the common first-person key binds, i.e. WASD for moving and mouse for looking around
 *
 * CameraController implements the camera controller using the input handler (listener) mechanism which the framework provides.
 * CameraControllerBase functions calculate positions but it does not contain code related to input handler.
 * The client is responsible for calling UpdateCameraPose() every frame with the delta time so that the camera controller
 * can update the camera position.
 */
class CameraController : public CameraControllerBase
{
	int m_InputHandlerIndex;

	std::shared_ptr< CInputDataDelegate<CameraController> > m_pInputDataDelagate;

public:

	CameraController( int input_handler_index );

	CameraController( InputHandler *pParentInputHandler );

	~CameraController();

//	bool IsKeyPressed( int general_input_code );

	void SetActive( bool active );
};


} // namespace amorphous



#endif		/*  __CameraController_H__  */
