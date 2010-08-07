#ifndef __Timer_posix_HPP__
#define __Timer_posix_HPP__


#include <sys/time.h>
#include "time.hpp"


/**
 Performs timer operations.
 - Use GetGlobalTimer() or macro 'TIMER' to get the global instance.
 */
class CTimer
{
public:

	CTimer();

	void Reset(); ///< resets the timer

	void Start(); ///< starts the timer

	void Stop();  ///< stop (or pause) the timer

	double GetAbsoluteTime(); ///< get the absolute system time [(in sec?)]

	double GetTime() const; ///< get the current time [s]

//	double GetFrameTime64(); // get the time that elapsed between GetElapsedTime() calls

	/// get the current time [ms]
	inline unsigned long GetTimeMS() const { return (unsigned long)(GetTime() * 1000.0); }

	/// returns the currenet frametime
	inline float GetFrameTime() const { return m_fFrameTime; }

	inline double GetFrameTime64() { return m_dFrameTime; }

	/// returns the frames per second
	inline float GetFPS() const { return m_fFPS; }

	/// updates the timer. must be called once per frame
	void UpdateFrameTime();

	/// Returns true if timer stopped
	bool IsStopped() const { return m_bTimerStopped; }

//	void Advance(); // advance the timer by 0.1 seconds

protected:

	/// for PerformanceQueryCounter() version of the timer
	bool m_bTimerStopped;

	int m_count;

	/// stores frametime. updated every frame in UpdateFrameTime()
	float m_fFrameTime;
	double m_dFrameTime;

	/// stores FPS. updated about every one second in UpdateFrameTime()
	float m_fFPS;

	struct timeval m_StartTime;

	struct timeval m_LastElapsedTime;
};


#endif  /* __Timer_posix_HPP__ */
