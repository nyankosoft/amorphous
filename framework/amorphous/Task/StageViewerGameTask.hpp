#ifndef  __StageViewerGameTask_H__
#define  __StageViewerGameTask_H__


#include "SingleStageGameTask.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Support/CameraController.hpp"
#include <memory>


namespace amorphous
{


class StageViewerGameTask : public SingleStageGameTask
{
	Camera m_Camera;

	std::shared_ptr<CameraController> m_pCameraController;

	/// Use the above camera controller to control the camera movement.
	/// - default: true
	bool m_EnableCameraController;

protected:

	const Camera& GetCamera() const { return m_Camera; }

	const std::shared_ptr<CameraController> GetCameraController() const { return m_pCameraController; }

	void SetCameraPosition( const Vector3& pos ) { m_Camera.SetPosition(pos); m_pCameraController->SetPosition(pos); }

	void SetCameraPose( const Matrix34& pose ) { m_Camera.SetPose(pose); m_pCameraController->SetPose(pose); }

public:

	StageViewerGameTask();

	virtual ~StageViewerGameTask();

	void UpdateCameraPose();

	virtual int FrameMove( float dt );

	virtual void Render();

	void EnableCameraController( bool enable ) { m_EnableCameraController = enable; }

	bool IsCameraControllerEnabled() const { return m_EnableCameraController; }
};

} // namespace amorphous



#endif  /*  __StageViewerGameTask_H__  */
