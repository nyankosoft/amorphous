#ifndef __amorphous_Timer_HPP__
#define __amorphous_Timer_HPP__


#include <chrono>


namespace amorphous
{

/**
\brief A timer that keeps how much time has passed since the creation of the timer object.

Timer automatically starts when the object is instantiated.
*/
class Timer
{
	std::chrono::high_resolution_clock m_Clock;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;

	/// Below are somewhat undesirable backward compatibility necessities
	/// The thing is that this Timer class assumes the duty of keeping frame time
	/// as well as functions as timer.

	unsigned long m_LastElapsedTimeInMilliseconds;

	float m_fFrameTime;

public:

	Timer() : m_StartTime(m_Clock.now()),
		m_LastElapsedTimeInMilliseconds(0),
		m_fFrameTime(0.0f)
	{}

	/**
	\brief Resets and starts the timer.
	*/
	void Reset()
	{
		m_StartTime = m_Clock.now();
	}

	/**
	\brief Returns the elapsed time in milliseconds.
	*/
	inline unsigned long GetElapsedTimeInMilliseconds() const
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> now = m_Clock.now();
		std::chrono::duration<double, std::milli> elapsed = now - m_StartTime;
		return (unsigned long)elapsed.count();
	}

	inline double GetElapsedTimeInSeconds() const
	{
		unsigned long milliseconds = GetElapsedTimeInMilliseconds();
		return (double)milliseconds * 0.001;
	}

	inline float GetFrameTime() const { return m_fFrameTime; }

	inline void UpdateFrameTime()
	{
		auto elapsed = GetElapsedTimeInMilliseconds();
		m_fFrameTime = (elapsed - m_LastElapsedTimeInMilliseconds) * 0.001f;
		m_LastElapsedTimeInMilliseconds = elapsed;
	}

	inline float GetFPS() const { return 1.0f / m_fFrameTime; }
};


inline Timer& GlobalTimer()
{
	// Using an accessor function gives control of the construction order
	// - This is a non-thread safe implementation of singleton pattern.
	// - Make sure GlobalTimer() is not called by more than one thread at the same time.
	static Timer timer;

//	static int initialized = 0;
//	if( initialized == 0 )
//	{
//		timer.Start();
//		initialized = 1;
//	}

	return timer;
}

} // namespace amorphous



#endif /* __amorphous_Timer_HPP__ */
