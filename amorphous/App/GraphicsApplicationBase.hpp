#ifndef  __GraphicsApplicationBase_HPP__
#define  __GraphicsApplicationBase_HPP__


#include <boost/shared_ptr.hpp>
#include "ApplicationBase.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Support/CameraController.hpp"


namespace amorphous
{


/**
 Base class for graphics application
 - This class is for state-less application that uses a window and graphics.
 */
class GraphicsApplicationBase : public ApplicationBase
{
	Camera m_Camera;

//	CPlatformDependentCameraController m_CameraController;
	boost::shared_ptr<CameraControllerBase> m_pCameraController;

	boost::shared_ptr<FontBase> m_pFont;

	boost::shared_ptr<InputHandler> m_pInputHandler;

	SFloatRGBAColor m_BackgroundColor;

private:

//	void Execute();
//	void InitDebugItems();
//	void ReleaseDebugItems();

	virtual const std::string GetApplicationTitle() { return "Graphics Application"; }

	virtual int Init() { return 0; }

	void UpdateCameraMatrices();

	void UpdateFrame();

	// Override this to implement the rendering routine
	virtual void Render() = 0;

	virtual void Update( float dt ) {}

	virtual void HandleInput( const InputData& input ) {}

	void RenderBase();

protected:

	bool m_UseCameraController;

	boost::shared_ptr<CameraControllerBase> GetCameraController() { return m_pCameraController; }

	Camera& Camera() { return m_Camera; }

	void SetBackgroundColor( const SFloatRGBAColor& bg_color ) { m_BackgroundColor = bg_color; }

public:

	GraphicsApplicationBase();

	virtual ~GraphicsApplicationBase();

//	bool InitBase();

//	void Release();

//	void AcquireInputDevices();

//	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	void Run();

	friend class CInputDataDelegate<GraphicsApplicationBase>;
};

} // namespace amorphous



#endif		/*  __GraphicsApplicationBase_HPP__  */
