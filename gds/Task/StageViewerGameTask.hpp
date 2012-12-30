#ifndef  __StageViewerGameTask_H__
#define  __StageViewerGameTask_H__


#include "SingleStageGameTask.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Support/CameraController.hpp"
#include <boost/shared_ptr.hpp>


namespace amorphous
{


class CStageViewerGameTask : public CSingleStageGameTask
{
	CCamera m_Camera;

	boost::shared_ptr<CCameraController> m_pCameraController;

	/// Use the above camera controller to control the camera movement.
	/// - default: true
	bool m_EnableCameraController;

protected:

	const CCamera& GetCamera() const { return m_Camera; }

	boost::shared_ptr<CCameraController> CameraController() { return m_pCameraController; }

	void SetCameraPosition( const Vector3& pos ) { m_Camera.SetPosition(pos); m_pCameraController->SetPosition(pos); }

	void SetCameraPose( const Matrix34& pose ) { m_Camera.SetPose(pose); m_pCameraController->SetPose(pose); }

public:

	CStageViewerGameTask();

	virtual ~CStageViewerGameTask();

	void UpdateCameraPose();

	virtual int FrameMove( float dt );

	virtual void Render();

	void EnableCameraController( bool enable ) { m_EnableCameraController = enable; }

	bool IsCameraControllerEnabled() const { return m_EnableCameraController; }
};

} // namespace amorphous



#endif  /*  __StageViewerGameTask_H__  */
