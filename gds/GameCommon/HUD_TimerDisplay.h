#ifndef  __HUD_TimerDisplay_H__
#define  __HUD_TimerDisplay_H__


#include "3DCommon/Font/FontBase.h"
#include "../base.h"


class HUD_TimerDisplay
{
	char m_TextBuffer[32];

public:

	unsigned long m_TimeMS;

	U32 m_Color;

	bool m_bDisplay;

public:

	HUD_TimerDisplay() : m_TimeMS(0), m_bDisplay(false) { memset( m_TextBuffer, 0, sizeof(m_TextBuffer) ); }

	void Render( CFontBase *pFont, int sx, int sy );

	inline void GetTimeMMSS( char *pDestBuffer );

	void Show() { m_bDisplay = true; }
	void Hide() { m_bDisplay = false; }
};


// ================================ inline implementations ================================

inline void HUD_TimerDisplay::GetTimeMMSS( char *pDestBuffer )
{
	int ms = m_TimeMS % 1000;
	int total_time_sec = m_TimeMS / 1000;
	int sec = total_time_sec % 60;
	int min = total_time_sec / 60;

	sprintf( pDestBuffer, "%02d:%02d", min, sec );
}

#endif		/*  __HUD_TimerDisplay_H__  */
