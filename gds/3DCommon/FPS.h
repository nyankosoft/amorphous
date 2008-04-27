#ifndef __FPS_H__
#define __FPS_H__

// note: "winmm.lib" has to be linked in order to use this class

#include <mmsystem.h>

#define FPS	( CFramePerSec::ms_FPS_ )


//================================================================
//	CFramePerSec
//		- manages fps, average fps and frame time
//================================================================

class CFramePerSec
{
	DWORD m_dwStartTime;	// used to record the time when this FPS object is created
	DWORD m_dwTotalFPSs;	// increased once per second  (updated with the FPS)

    DWORD m_dwLastTimeMS;	// time of the last UpdateFPS() call
    DWORD m_dwPrevTimeMS;	// time of the last FPS update in UpdateFPS()
    int m_count;

	float m_fFrameTime;		// frame time in seconds
    float m_fFPS;
	float m_fAveFPS;	// the average FPS to the present time.  updated once per frame 

protected:

	CFramePerSec();		// singleton

public:

	static CFramePerSec ms_FPS_;	// singleton instance of 'CFramePerSec'

	/// update fps system
	/// must be called at the start of each frame.
	void UpdateFPS();

	/// returns frames per seconds
	inline float GetFPS() const { return m_fFPS; }

	/// returns frame time in seconds
	inline float GetFrameTime() const { return m_fFrameTime; }

	inline float GetAverageFPS() const { return m_fAveFPS; }
};


#endif /*  __FPS_H__  */