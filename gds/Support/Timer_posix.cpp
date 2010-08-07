#include "Timer_posix.hpp"
#include <memory.h>


//--------------------------------------------------------------------------------------
CTimer::CTimer()
{
	memset( &m_StartTime,       0, sizeof(m_StartTime) );
	memset( &m_LastElapsedTime, 0, sizeof(m_LastElapsedTime) );

	m_count = 0;

	m_dFrameTime = 0.01;
	m_fFrameTime = 0.01f;
	m_fFPS = 0.0f;
}


//--------------------------------------------------------------------------------------
void CTimer::Reset()
{
}


//--------------------------------------------------------------------------------------
void CTimer::Start()
{
	gettimeofday( &m_StartTime, NULL );
}


//--------------------------------------------------------------------------------------
void CTimer::Stop()
{
	if( !m_bTimerStopped )
	{
		// Get either the current time or the stop time
/*		LARGE_INTEGER qwTime;
		if( m_llStopTime != 0 )
			qwTime.QuadPart = m_llStopTime;
		else
			QueryPerformanceCounter( &qwTime );

		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;*/
		m_bTimerStopped = true;
	}
}

/*
//--------------------------------------------------------------------------------------
void CTimer::Advance()
{
}*/


//--------------------------------------------------------------------------------------
double CTimer::GetAbsoluteTime()
{
	return 0;
}


//--------------------------------------------------------------------------------------
double CTimer::GetTime() const
{
	struct timeval current_time;
	gettimeofday( &current_time, NULL );

	stime s(m_StartTime);
	stime c(current_time);

	stime delta = c - s;

	return delta.get_sec();
}


const double max_fps = 1000;
const double min_frametime = 1.0 / max_fps;

//--------------------------------------------------------------------------------------
void CTimer::UpdateFrameTime()
{
	struct timeval current_time;
	gettimeofday( &current_time, NULL );

	stime c(current_time);

	stime delta = c - stime(m_LastElapsedTime);

	m_dFrameTime = delta.get_sec();
	m_fFrameTime = (float)m_dFrameTime;

	m_LastElapsedTime = current_time;
}

/*
//--------------------------------------------------------------------------------------
double CTimer::GetFrameTime64()
{
}*/
