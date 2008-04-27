
#include <windows.h>

#include "fps.h"
//#include <mmsystem.h>


//The instance of the static member has to be created outside its class
CFramePerSec CFramePerSec::ms_FPS_;

//  This constructor is deleared as 'protected', making sure that the object of 'CFramePerSec'
//cannot be created outside.
CFramePerSec::CFramePerSec()
{
	m_dwStartTime = timeGetTime();
	m_dwLastTimeMS = timeGetTime();
	m_dwPrevTimeMS = timeGetTime();
	m_count = 0;
	m_dwTotalFPSs = 0;

	m_fFrameTime = 0.0f;
	m_fFPS = 0.0f;
	m_fAveFPS = 0.0f;
}

#define MAX_FPS 60

//This function must be called at the start of each frame
void CFramePerSec::UpdateFPS()
{
    DWORD dwCurrentTimeMS = timeGetTime();

	// limit the maximum FPS to 1000 (/*1*/) or MAX_FPS (/*2*/)
	while( dwCurrentTimeMS == m_dwLastTimeMS )     /*1*/
//	while( dwCurrentTimeMS - (1000 / MAX_FPS ) < m_dwLastTimeMS )	/*2*/
	{
		dwCurrentTimeMS = timeGetTime();
	}

	// update frametime
	m_fFrameTime = ( dwCurrentTimeMS - m_dwLastTimeMS ) / 1000.0f;
	m_dwLastTimeMS = dwCurrentTimeMS;

	// limit the maximum frame time (for debug )
	if( 0.05f < m_fFrameTime )
		m_fFrameTime = 0.05f;

	// update FPS about every 1 second
    m_count++;
    DWORD dwAccumulatedTime = dwCurrentTimeMS - m_dwPrevTimeMS;
    if(1000 < dwAccumulatedTime)
    {
		// Update FPS
        m_fFPS = (float)( m_count * 1000 / dwAccumulatedTime );
        m_count = 0;
		m_dwPrevTimeMS = dwCurrentTimeMS;

		// Update average FPS
		m_dwTotalFPSs += (DWORD)m_fFPS;
		m_fAveFPS = m_dwTotalFPSs / (float)( (dwCurrentTimeMS - m_dwStartTime) / 1000 );
    }
}