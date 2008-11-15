#include "Timer.h"


#pragma comment( lib, "winmm.lib" )


//--------------------------------------------------------------------------------------
CTimer::CTimer()
{
	m_bUsingQPF         = false;
	m_bTimerStopped     = true;
	m_llQPFTicksPerSec  = 0;

	m_llStopTime        = 0;
	m_llLastElapsedTime = 0;
	m_llBaseTime        = 0;

	// Use QueryPerformanceFrequency() to get frequency of timer.  
	LARGE_INTEGER qwTicksPerSec;
	m_bUsingQPF = (bool) (QueryPerformanceFrequency( &qwTicksPerSec ) != 0);
	m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

	if( !m_bUsingQPF )
	{
		/// QueryPerformanceCounter() is not available
		/// check the maximum precision of timeGetTime(), and make it as precise as possible

		m_dwLastTimeMS = timeGetTime();
		m_dwPrevTimeMS = timeGetTime();
	}

	m_count = 0;

	m_dFrameTime = 0.01;
	m_fFrameTime = 0.01f;
	m_fFPS = 0.0f;

	// set the base time
//	LARGE_INTEGER qwTime;
//	QueryPerformanceCounter( &qwTime );
//	m_llBaseTime = qwTime.QuadPart;

}


//--------------------------------------------------------------------------------------
void CTimer::Reset()
{
    if( !m_bUsingQPF )
        return;

    // Get either the current time or the stop time
    LARGE_INTEGER qwTime;
    if( m_llStopTime != 0 )
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter( &qwTime );

    m_llBaseTime        = qwTime.QuadPart;
    m_llLastElapsedTime = qwTime.QuadPart;
    m_llStopTime        = 0;
    m_bTimerStopped     = FALSE;
}


//--------------------------------------------------------------------------------------
void CTimer::Start()
{
	if( !m_bUsingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
//	if( m_llStopTime != 0 )
//		qwTime.QuadPart = m_llStopTime;
//	else
		QueryPerformanceCounter( &qwTime );

	if( m_bTimerStopped )
		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
	m_llStopTime = 0;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CTimer::Stop()
{
    if( !m_bUsingQPF )
        return;

    if( !m_bTimerStopped )
    {
        // Get either the current time or the stop time
        LARGE_INTEGER qwTime;
        if( m_llStopTime != 0 )
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter( &qwTime );

        m_llStopTime = qwTime.QuadPart;
        m_llLastElapsedTime = qwTime.QuadPart;
        m_bTimerStopped = TRUE;
    }
}

/*
//--------------------------------------------------------------------------------------
void CTimer::Advance()
{
    if( !m_bUsingQPF )
        return;

    m_llStopTime += m_llQPFTicksPerSec/10;
}*/


//--------------------------------------------------------------------------------------
double CTimer::GetAbsoluteTime()
{
    if( !m_bUsingQPF )
        return -1.0;

    // Get either the current time or the stop time
    LARGE_INTEGER qwTime;
    if( m_llStopTime != 0 )
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter( &qwTime );

    double fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;

    return fTime;
}


//--------------------------------------------------------------------------------------
double CTimer::GetTime() const
{
    if( !m_bUsingQPF )
        return -1.0;

    // Get either the current time or the stop time
    LARGE_INTEGER qwTime;
    if( m_llStopTime != 0 )
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter( &qwTime );

    double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;

    return fAppTime;
}


const double max_fps = 1000;
const double min_frametime = 1.0 / max_fps;

//--------------------------------------------------------------------------------------
void CTimer::UpdateFrameTime()
{
    if( m_bUsingQPF )
	{
		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( m_llStopTime != 0 )
			qwTime.QuadPart = m_llStopTime;
		else
		{
			QueryPerformanceCounter( &qwTime );

			while( (double)(qwTime.QuadPart - m_llLastElapsedTime) / (double)m_llQPFTicksPerSec < min_frametime )
			{
				QueryPerformanceCounter( &qwTime );
			}
		}

		double fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
		m_llLastElapsedTime = qwTime.QuadPart;

		m_dFrameTime = fElapsedTime;
		m_fFrameTime = (float)fElapsedTime;

		// update FPS about every one second
		m_count++;
		LONGLONG llAccumulatedTime = qwTime.QuadPart - m_llPrevTime;
		if( m_llQPFTicksPerSec < llAccumulatedTime )
		{
			// update FPS
			m_fFPS = (float)m_count / (float)(llAccumulatedTime / m_llQPFTicksPerSec);
			m_count = 0;
			m_llPrevTime = qwTime.QuadPart;
		}
	}
	else
	{
		DWORD dwCurrentTimeMS = timeGetTime();

		// limit the maximum FPS to 1000 (/*1*/) or MAX_FPS (/*2*/)
		while( dwCurrentTimeMS == m_dwLastTimeMS )     /*1*/
	//	while( dwCurrentTimeMS - (1000 / MAX_FPS ) < m_dwLastTimeMS )	/*2*/
		{
			dwCurrentTimeMS = timeGetTime();
		}

		// update frametime
		m_dFrameTime = ( dwCurrentTimeMS - m_dwLastTimeMS ) / 1000.0;
		m_dwLastTimeMS = dwCurrentTimeMS;

		m_fFrameTime = (float)m_dFrameTime;

		// limit the maximum frame time (for debug )
		if( 0.05f < m_fFrameTime )
			m_fFrameTime = 0.05f;

		// update FPS about every second second
		m_count++;
		DWORD dwAccumulatedTime = dwCurrentTimeMS - m_dwPrevTimeMS;
		if(1000 < dwAccumulatedTime)
		{
			// Update FPS
			m_fFPS = (float)( m_count * 1000 / dwAccumulatedTime );
			m_count = 0;
			m_dwPrevTimeMS = dwCurrentTimeMS;

			// Update average FPS
//			m_dwTotalFPSs += (DWORD)m_fFPS;
//			m_fAveFPS = m_dwTotalFPSs / (float)( (dwCurrentTimeMS - m_dwStartTime) / 1000 );
		}
	}

}

/*
//--------------------------------------------------------------------------------------
double CTimer::GetFrameTime64()
{
    if( !m_bUsingQPF )
        return -1.0;

    // Get either the current time or the stop time
    LARGE_INTEGER qwTime;
    if( m_llStopTime != 0 )
        qwTime.QuadPart = m_llStopTime;
    else
        QueryPerformanceCounter( &qwTime );

    double fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
    m_llLastElapsedTime = qwTime.QuadPart;

    return fElapsedTime;
}*/


//--------------------------------------------------------------------------------------
bool CTimer::IsStopped()
{
    return m_bTimerStopped;
}

