#include "HUD_TimerDisplay.hpp"
#include "amorphous/Graphics/Font/FontBase.hpp"


namespace amorphous
{


void HUD_TimerDisplay::Render( FontBase *pFont, int sx, int sy )
{
	if( !m_bDisplay 
	 || !pFont )
		return;

	pFont->SetFontColor( m_Color );

	GetTimeMMSS( m_TextBuffer );

	pFont->DrawText( m_TextBuffer, sx, sy );
}


} // namespace amorphous
