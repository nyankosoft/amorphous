#include "OnScreenDebugInfoRenderer.hpp"
#include "Font/BuiltinFonts.hpp"
#include "amorphous/Support/Timer.hpp"


namespace amorphous
{


OnScreenDebugInfoRenderer::OnScreenDebugInfoRenderer()
{
	m_pFont = CreateDefaultBuiltinFont();

	int font_width  =  6;
	int font_height = 14;

	if( m_pFont )
	{
		m_pFont->SetFontColor( SFloatRGBAColor::White() );
		m_pFont->SetFontSize( font_width, font_height );
	}

	m_vTopLeftPos = Vector2( 5, 5 );
}


void OnScreenDebugInfoRenderer::RenderBGRect( int text_width )
{
	C2DRect rect( RectLTWH(
		(int)m_vTopLeftPos.x,
		(int)m_vTopLeftPos.y,
		text_width + 10,//640,
		640 ),
		0x60000000 );

	rect.Draw();
}


void OnScreenDebugInfoRenderer::RenderDebugInfo( const std::string& debug_info_text )
{
	if( !m_pFont )
		return;

	SetRenderStatesForTextureFont( AlphaBlend::One );
//	C2DRect rect( 745, 45, 1400, 700, 0x60000000 );

	int text_width = m_pFont->GetTextWidth( debug_info_text.c_str() );

	RenderBGRect( text_width );

	RenderFPS();

	Vector2 pos = m_vTopLeftPos;

	// Reserve the space for FPS display
	pos.y += m_pFont->GetFontHeight() * 1.6f;

	Vector2 vTextTopLeftPos = m_vTopLeftPos + Vector2(5,5);
	m_pFont->DrawText( debug_info_text, vTextTopLeftPos, 0xFFFFFFFF );
}


void OnScreenDebugInfoRenderer::RenderDebugInfo(
	const std::vector<std::string>& debug_info_texts,
	const std::vector<SFloatRGBAColor> text_colors
	)
{
	if( !m_pFont )
		return;

	SetRenderStatesForTextureFont( AlphaBlend::One );

	int max_text_width = 0;
	for( size_t i=0; i<debug_info_texts.size(); i++ )
		max_text_width = take_max( max_text_width, m_pFont->GetTextWidth( debug_info_texts[i].c_str() ) );

	RenderBGRect( max_text_width );

	RenderFPS();

	Vector2 pos = m_vTopLeftPos;

	// Reserve the space for FPS display
	pos.y += m_pFont->GetFontHeight() * 1.6f;

	for( size_t i=0; i<debug_info_texts.size(); i++ )
	{
		SFloatRGBAColor color = (i < text_colors.size()) ? text_colors[i] : SFloatRGBAColor::White();
		m_pFont->DrawText( debug_info_texts[i], pos, color.GetARGB32() );
		pos.y += (int)(m_pFont->GetFontHeight() * 1.2f );
	}
}


void OnScreenDebugInfoRenderer::RenderFPS()
{
	if( !m_pFont )
		return;

///	StateLog.Update( 0, "FPS: " + to_string(GlobalTimer().GetFPS()) );
//	StateLog.Update( 1, "AVE. FPS: " + to_string(FPS.GetAverageFPS()) );
	std::string fps_text = "FPS: " + to_string(GlobalTimer().GetFPS());

	U32 color = ( GlobalTimer().GetFPS() < 40.0f ) ? 0xFFFF0000 /* fps low - red */ : 0xFF00FF00; /* green */

	m_pFont->DrawText( fps_text.c_str(), m_vTopLeftPos, color );
}


} // namespace amorphous
