#ifndef  __GraphicsApplicationBase_HPP__
#define  __GraphicsApplicationBase_HPP__


#include <memory>
#include "ApplicationBase.hpp"
#include "amorphous/3DMath/Rectangular.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Support/CameraController.hpp"


namespace amorphous
{


/**
 * \brief Base class for simple graphics applications
 * 
 * This class is for creating state-less applications that use a single window.
 * It takes care of low-level setups related to rendering, but does not provide
 * a high degree of flexibility.
 * In other words, it is not as heavy-duty as to withstand the use for serious
 * production-ready games.
 * 
 * What this class provides:
 * - It provides a first-person camera which the application user can move
 *   to navigate inside the scene.
 * 
 * Requirements for derived (user-defined) classes (what they need to implement):
 * - Implement Render() to render the custom scene.
 * 
 * What derived (user-defined) classes can do:
 * - Define a custom initialization function by implementing Init()
 * - Set a custom background color by setting a color to m_BackgroundColor.
 * - Set a custom window title by overriding GetApplicationTitle()
 */
class GraphicsApplicationBase : public ApplicationBase
{
	Camera m_Camera;

//	CPlatformDependentCameraController m_CameraController;
	std::shared_ptr<CameraControllerBase> m_pCameraController;

	std::shared_ptr<FontBase> m_pFont;

	std::shared_ptr<InputHandler> m_pInputHandler;

	SFloatRGBAColor m_BackgroundColor;

	SRectangular m_WindowedModeResolution;

private:

//	void Execute();
//	void InitDebugItems();
//	void ReleaseDebugItems();

	virtual const std::string GetApplicationTitle() { return "Graphics Application"; }

	virtual int Init() { return 0; }

	void GetResolution( int& w, int& h );

	void UpdateCameraMatrices();

	void UpdateFrame();

	// Override this to implement the rendering routine
	virtual void Render() = 0;

	virtual void Update( float dt ) {}

	void RenderBase();

	void ToggleScreenModes();

protected:

	bool m_UseCameraController;

	std::shared_ptr<CameraControllerBase> GetCameraController() { return m_pCameraController; }

	Camera& GetCamera() { return m_Camera; }

	void SetBackgroundColor( const SFloatRGBAColor& bg_color ) { m_BackgroundColor = bg_color; }

	virtual void HandleInput( const InputData& input );

public:

	GraphicsApplicationBase();

	virtual ~GraphicsApplicationBase();

//	bool InitBase();

//	void Release();

//	void AcquireInputDevices();

//	static void SetDefaultSleepTime( int sleep_time_in_ms ) { ms_DefaultSleepTimeMS = sleep_time_in_ms; }

	/**
	 * \brief Creates the application window and returns enters the main loop.
	 */
	void Run();

	void EnableCameraControl();

	void DisableCameraControl();

	void RenderScreenshotScene() { RenderBase(); } 

	friend class CInputDataDelegate<GraphicsApplicationBase>;
};

} // namespace amorphous



#endif		/*  __GraphicsApplicationBase_HPP__  */
