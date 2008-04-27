#include "HUD_TimerDisplay.h"


void HUD_TimerDisplay::Render( CFontBase *pFont, int sx, int sy )
{
	if( !m_bDisplay 
	 || !pFont )
		return;

	pFont->SetFontColor( m_Color );

	int ms = m_TimeMS % 1000;
	int total_time_sec = m_TimeMS / 1000;
	int sec = total_time_sec % 60;
	int min = total_time_sec / 60;

	sprintf( m_TextBuffer, "%02d:%02d", min, sec );

	pFont->DrawText( m_TextBuffer, sx, sy );
}
