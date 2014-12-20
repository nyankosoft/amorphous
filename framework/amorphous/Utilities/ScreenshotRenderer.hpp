#ifndef __ScreenshotRenderer_HPP__
#define __ScreenshotRenderer_HPP__


#include <string>


namespace amorphous
{


class ScreenshotRenderer
{
public:
	virtual ~ScreenshotRenderer(){}

	virtual void Render() = 0;
};


template<class Renderer>
class ScreenshotRendererDelegate : public ScreenshotRenderer
{
	Renderer& m_Renderer;
public:

	ScreenshotRendererDelegate(Renderer& renderer)
		:
	m_Renderer(renderer)
	{}

	void Render()
	{
		m_Renderer.RenderScreenshotScene();
	}
};


void _TakeScreenshot(
	ScreenshotRenderer& renderer,
	const std::string& directory_path,
	const std::string& basename,
	const char *image_format = ""
	);


template<class Renderer>
inline void TakeScreenshot(
	Renderer& renderer,
	const std::string& directory_path,
	const std::string& basename,
	const char *image_format = ""
	)
{
	ScreenshotRendererDelegate<Renderer> renderer_delegate(renderer);

	_TakeScreenshot( renderer_delegate, directory_path, basename, image_format );
}


void SetScreenshotImageFormat( const std::string& image_format );


} // amorphous


#endif /* __ScreenshotRenderer_HPP__ */
