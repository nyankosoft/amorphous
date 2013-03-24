#include "GraphicsTestBase.hpp"
#include "gds/Graphics/TextureRenderTarget.hpp"
#include "gds/App/GameWindowManager.hpp"
#include <boost/filesystem.hpp>


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
		m_Renderer.Render();
	}
};


unsigned int GetNextImageFileNumber( const std::string& directory_path, const std::string& basename )
{
	using namespace boost::filesystem;
	
	unsigned int count = 0;

	directory_iterator end_itr; // default construction yields past-the-end
	for( directory_iterator itr( directory_path );
		itr != end_itr;
		++itr )
	{
		const path& pathname = *itr;

		if( pathname.leaf().string().find( basename ) == 0 )
			count += 1;
	}

	return count;
}


/// \param image_extension Starts with '.', e.g. ".png"
void TakeScreenshot(
	ScreenshotRenderer& renderer,
	const std::string& directory_path,
	const std::string& basename,
	const char *image_extension = ".png"
	)
{
	using namespace boost::filesystem;

	boost::shared_ptr<TextureRenderTarget> pTextureRenderTarget = TextureRenderTarget::Create();
	const int screen_width  = GameWindowManager().GetScreenWidth();
	const int screen_height = GameWindowManager().GetScreenHeight();
	pTextureRenderTarget->Init( screen_width, screen_height );
	pTextureRenderTarget->SetRenderTarget();

	renderer.Render();

	pTextureRenderTarget->ResetRenderTarget();
	unsigned int count = GetNextImageFileNumber( directory_path, basename );
	path directory_path_and_basename = path(directory_path) / path(basename);
//	std::string img_file_path = fmt_string( "screenshots/image%04d", count ) + "." + img_ext;
	std::string img_file_pathname
		= fmt_string( "%s%04d", directory_path_and_basename.string().c_str(), count ) + image_extension;

	pTextureRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( img_file_pathname );
}


//template<class T>
//void TakeScreenshot(
//	T& renderer_class_object,
//	const std::string& directory_path,
//	const std::string& basename,
//	const char *image_extension = ".png"
//	)
//{
//	ScreenshotRendererDelegate<T> renderer(renderer_class_object);
//	TakeScreenshot( renderer, directory_path, basename, image_extension );
//}


CGraphicsTestBase::CGraphicsTestBase()
:
m_WindowWidth(1280),
m_WindowHeight(720),
m_UseRenderBase(false),
m_BackgroundColor( SFloatRGBAColor( 0.19f, 0.19f, 0.19f, 1.00f ) )
{
	m_pCameraController.reset( new amorphous::CameraController( ms_CameraControllerInputHandlerIndex ) );
}


void CGraphicsTestBase::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			ScreenshotRendererDelegate<CGraphicsTestBase> renderer(*this);
			TakeScreenshot(
				renderer,
				"screenshots",
				"graphics_demo"
				);
		}
		break;
	default:
		break;
	}
}
