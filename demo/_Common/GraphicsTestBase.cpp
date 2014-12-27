#include "GraphicsTestBase.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Graphics/GraphicsResourceManager.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Support/Timer.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include <boost/filesystem.hpp>


int CGraphicsTestBase::ms_CameraControllerInputHandlerIndex = 1;


CGraphicsTestBase::CGraphicsTestBase()
:
m_WindowWidth(1280),
m_WindowHeight(720),
m_UseRenderBase(false)
//m_BackgroundColor( SFloatRGBAColor( 0.19f, 0.19f, 0.19f, 1.00f ) ),
{
//	m_pCameraController.reset( new amorphous::CameraController( ms_CameraControllerInputHandlerIndex ) );
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
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}
}


const Camera& CGraphicsTestBase::GetCurrentCamera()
{
	if( m_pCameraController )
		m_Camera.SetPose( m_pCameraController->GetPose() );

	return m_Camera;
}


void CGraphicsTestBase::CreateParamFileIfNotFound( const char *param_file, const char *text )
{
	using namespace boost::filesystem;
	
	if( exists(param_file) )
		return;

	std::ofstream ofs( param_file );
	if( !ofs.is_open() )
		return;

	ofs << text;
}
