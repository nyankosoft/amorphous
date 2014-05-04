#include "GraphicsTestBase.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/Camera.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include <boost/filesystem.hpp>


extern Camera g_Camera;


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
	const int screen_width  = GetGameWindowManager().GetScreenWidth();
	const int screen_height = GetGameWindowManager().GetScreenHeight();
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
m_BackgroundColor( SFloatRGBAColor( 0.19f, 0.19f, 0.19f, 1.00f ) ),
m_DisplayDebugInfo(false)
{
	m_pCameraController.reset( new amorphous::CameraController( ms_CameraControllerInputHandlerIndex ) );
}


Result::Name CGraphicsTestBase::InitBase()
{
	m_pFont = CreateDefaultBuiltinFont();
	m_pFont->SetFontSize( 8, 16 );
//	m_pFont->SetFontSize( 6, 12 );

	// You can also specify the font with a name.
//	shared_ptr<TextureFont> pTexFont( new TextureFont );
//	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
//	pTexFont->SetFontSize( 8, 16 );
//	m_pFont = pTexFont;

	return Result::SUCCESS;
}


void CGraphicsTestBase::DisplayDebugInfo()
{
	if( !m_DisplayDebugInfo )
		return;

	GraphicsDevice().Enable(  RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	m_TextBuffer.resize( 0 );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();

	const std::vector<std::string>& profile_results = GetProfileText();

	if( m_pFont )
	{
		m_pFont->DrawText( m_TextBuffer, vTopLeft );

		const Vector3 cam_pos = GetCurrentCamera().GetPosition();
		m_pFont->DrawText( std::string("Camera: ") + to_string( cam_pos ), Vector2( 20, 300 ) );

		// display fps
		m_pFont->DrawText( to_string(GlobalTimer().GetFPS()).c_str(), Vector2(20,20), 0xFFFFFFFF );

		for( size_t i=0; i<profile_results.size(); i++ )
		{
			const std::string& text = profile_results[i];
			m_pFont->DrawText( text, Vector2( 20, 40 + i*16 ), 0xF0F0F0FF );
		}
	}
}


void CGraphicsTestBase::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_DisplayDebugInfo = !m_DisplayDebugInfo;
		break;

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


const Camera& CGraphicsTestBase::GetCurrentCamera() const
{
	return g_Camera;
}