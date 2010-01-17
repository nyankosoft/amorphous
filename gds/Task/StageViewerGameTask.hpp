#ifndef  __StageViewerGameTask_H__
#define  __StageViewerGameTask_H__


#include "SingleStageGameTask.hpp"
#include "Graphics/Camera.hpp"
#include "Support/CameraController.hpp"
#include <boost/shared_ptr.hpp>


class CStageViewerGameTask : public CSingleStageGameTask
{
	CCamera m_Camera;

	boost::shared_ptr<CCameraController> m_pCameraController;

	/// Use the above camera controller to control the camera movement.
	/// - default: true
	bool m_EnableCameraController;

protected:

	CCamera& Camera() { return m_Camera; }

	boost::shared_ptr<CCameraController> CameraController() { return m_pCameraController; }

public:

	CStageViewerGameTask();

	virtual ~CStageViewerGameTask();

	void UpdateCameraPose();

	virtual int FrameMove( float dt );

	virtual void Render();

	void EnableCameraController( bool enable ) { m_EnableCameraController = enable; }

	bool IsCameraControllerEnabled() const { return m_EnableCameraController; }
};


#endif  /*  __StageViewerGameTask_H__  */
