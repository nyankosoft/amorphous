#ifndef  __GvrAppBase_HPP__
#define  __GvrAppBase_HPP__


#include <memory>
#include <array>
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Input/InputHandler.hpp"
#include "amorphous/Support/fwd.hpp"



namespace amorphous
{


/**
 \brief Application base class for developing an app for Cardboard VR using Google VR SDK
        with the help of amorphous framework
 
 - User can derive from this class to make a VR app.
 - Note that this app class is designed specifically to work with Google VR SDK,
   and is independent of any application class of amorphous framework.
 */
class GvrAppBase
{
	Camera m_Camera;

    Matrix44 m_ViewTransform;
    Matrix44 m_ProjectionTransform;

	std::shared_ptr<CameraController> m_pCameraController;

//	std::shared_ptr<FontBase> m_pFont;

	//std::shared_ptr<InputHandler> m_pInputHandler;
    std::shared_ptr< CInputDataDelegate<GvrAppBase> > m_pInputEventListener;
	
	SFloatRGBAColor m_BackgroundColor;

protected:

//	bool m_UseCameraController;

//	std::shared_ptr<CameraControllerBase> GetCameraController() { return m_pCameraController; }

	const Camera& GetCamera() const { return m_Camera; }

	void SetBackgroundColor( const SFloatRGBAColor& bg_color ) { m_BackgroundColor = bg_color; }

	virtual void HandleInput( const InputData& input ) {}

public:

	GvrAppBase() {}

	virtual ~GvrAppBase() {}

	int InitBase();

	void RenderBase();

	virtual int Init() { return 0; }

	// Override this to implement the rendering routine
	virtual void Render() = 0;

	void SetViewTransform(const std::array<float, 16>& view);

	void SetProjectionTransform(const std::array<float, 16>& proj);

	void OnKeyDown(int key_code);

	void OnKeyUp(int key_code);

	virtual void Update( float dt );

	friend class CInputDataDelegate<GvrAppBase>;
};

} // namespace amorphous


#endif		/*  __GvrAppBase_HPP__  */
