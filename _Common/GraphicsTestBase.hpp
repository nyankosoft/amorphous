#ifndef __GraphicsTestBase_H__
#define __GraphicsTestBase_H__


#include <vector>
#include <string>
#include <boost/weak_ptr.hpp>

#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/FloatRGBAColor.hpp"
#include "KeyState.hpp"
#include "gds/3DMath/fwd.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Input/InputHandler.hpp"
#include "gds/Support/CameraController.hpp"

using namespace amorphous;


class CGraphicsTestBase;


extern CGraphicsTestBase *g_pGraphicsTest;
extern Camera g_Camera;
extern std::string g_CmdLine;

inline std::string GetCommonShadersDirectoryPath() { return "../../../Shaders/"; }


class CGraphicsTestBase
{
	int m_WindowWidth;
	int m_WindowHeight;

	bool m_UseRenderBase;

	SFloatRGBAColor m_BackgroundColor;

	boost::shared_ptr<CameraController> m_pCameraController;

protected:

	static const int ms_CameraControllerInputHandlerIndex = 0;

protected:

	void SetUseRenderBase( bool use_render_base ) { m_UseRenderBase = use_render_base; }

	void SetBackgroundColor( const SFloatRGBAColor& color ) { m_BackgroundColor = color; }

	const Camera& GetCurrentCamera() const { return g_Camera; };

public:

	CGraphicsTestBase()
		:
	m_WindowWidth(1280),
	m_WindowHeight(720),
	m_UseRenderBase(false),
	m_BackgroundColor( SFloatRGBAColor( 0.19f, 0.19f, 0.19f, 1.00f ) )
	{
		m_pCameraController.reset( new amorphous::CameraController( ms_CameraControllerInputHandlerIndex ) );
	}

	virtual ~CGraphicsTestBase() {}

	virtual const char *GetAppTitle() const { return ""; }

	/// returns 0 on success
	virtual int Init() { return 0; }

	virtual void Release() {}
	virtual void Update( float dt ) {}

	// Testing framework calls  BeginScene() and EndScene() pairs
	// - override this for simple tests
	virtual void Render() {}

	// User needs to call BeginScene() and EndScene() pairs
	// - override this for tests that use more customized rendering routines
	virtual void RenderScene() {}

	bool UseRenderBase() const { return m_UseRenderBase; }
	virtual void RenderBase() {}

	void UpdateCameraController( float dt ) { if( m_pCameraController ) m_pCameraController->UpdateCameraPose( dt ); }

	const boost::shared_ptr<CameraController> GetCameraController() const { return m_pCameraController; }

	boost::shared_ptr<CameraController> CameraController() { return m_pCameraController; }

	virtual void UpdateCameraPose( const Matrix34& camera_pose ) {}

	virtual void UpdateViewTransform( const Matrix44& matView ) {}
	virtual void UpdateProjectionTransform( const Matrix44& matProj ) {}

	virtual void OnKeyPressed( KeyCode::Code key_code ) {}
	virtual void OnKeyReleased( KeyCode::Code key_code ) {}

	virtual void HandleInput( const InputData& input ) {}

	void SetWindowSize( int w, int h ) { m_WindowWidth = w; m_WindowHeight = h; }

	int GetWindowWidth() const { return m_WindowWidth; }
	int GetWindowHeight() const { return m_WindowHeight; }

	virtual void AcquireInputDevices() {}

	const SFloatRGBAColor& GetBackgroundColor() const { return m_BackgroundColor; }
};


class CGraphicsTestInputHandler : public InputHandler
{
	boost::weak_ptr<CGraphicsTestBase> m_pTest;

public:

	CGraphicsTestInputHandler( boost::weak_ptr<CGraphicsTestBase> pTest )
		:
	m_pTest(pTest) {}

	void ProcessInput( InputData& input )
	{
		boost::shared_ptr<CGraphicsTestBase> pTest = m_pTest.lock();

		if( pTest )
		{
			pTest->HandleInput( input );
		}
	}
};


#endif  /* __GraphicsTestBase_H__ */
