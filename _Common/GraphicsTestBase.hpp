#ifndef __GraphicsTestBase_H__
#define __GraphicsTestBase_H__


#include <vector>
#include <string>
#include <boost/weak_ptr.hpp>
#include <d3dx9.h>

#include "Graphics/fwd.hpp"
#include "KeyState.hpp"
#include "Input/InputHandler.hpp"


class Matrix34;
class Matrix44;
class CGraphicsTestBase;


extern CGraphicsTestBase *g_pGraphicsTest;
extern CCamera g_Camera;
extern std::string g_CmdLine;

inline std::string GetCommonShadersDirectoryPath() { return "../../../Shaders/"; }


class CGraphicsTestBase
{
	int m_WindowWidth;
	int m_WindowHeight;

	bool m_UseRenderBase;

	SFloatRGBAColor m_BackgroundColor;

protected:

	void SetUseRenderBase( bool use_render_base ) { m_UseRenderBase = use_render_base; }

	void SetBackgroundColor( const SFloatRGBAColor& color ) { m_BackgroundColor = color; }

public:

	CGraphicsTestBase()
		:
	m_WindowWidth(800),
	m_WindowHeight(600),
	m_UseRenderBase(false),
	m_BackgroundColor( SFloatRGBAColor( 0.19f, 0.19f, 0.19f, 1.00f ) )
	{}

	virtual ~CGraphicsTestBase() {}

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

	virtual void UpdateCameraPose( const Matrix34& camera_pose ) {}

	virtual void UpdateViewTransform( const Matrix44& matView ) {}
	virtual void UpdateProjectionTransform( const Matrix44& matProj ) {}

	virtual void OnKeyPressed( KeyCode::Code key_code ) {}
	virtual void OnKeyReleased( KeyCode::Code key_code ) {}

	virtual void HandleInput( const SInputData& input ) {}

	void SetWindowSize( int w, int h ) { m_WindowWidth = w; m_WindowHeight = h; }

	int GetWindowWidth() const { return m_WindowWidth; }
	int GetWindowHeight() const { return m_WindowHeight; }

	virtual void AcquireInputDevices() {}

	const SFloatRGBAColor& GetBackgroundColor() const { return m_BackgroundColor; }
};


class CGraphicsTestInputHandler : public CInputHandler
{
	boost::weak_ptr<CGraphicsTestBase> m_pTest;

public:

	CGraphicsTestInputHandler( boost::weak_ptr<CGraphicsTestBase> pTest )
		:
	m_pTest(pTest) {}

	void ProcessInput( SInputData& input )
	{
		boost::shared_ptr<CGraphicsTestBase> pTest = m_pTest.lock();

		if( pTest )
		{
			pTest->HandleInput( input );
		}
	}
};


#endif  /* __GraphicsTestBase_H__ */
