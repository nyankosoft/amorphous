#ifndef  __HUD_TimerDisplay_H__
#define  __HUD_TimerDisplay_H__


#include "3DCommon/FontBase.h"
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

	void Show() { m_bDisplay = true; }
	void Hide() { m_bDisplay = false; }
};


#endif		/*  __HUD_TimerDisplay_H__  */
