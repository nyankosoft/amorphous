#ifndef  __SOUNDHANDLE_H__
#define  __SOUNDHANDLE_H__


#include "fwd.h"
#include <string>


class CSoundHandle
{
	/// name of a sound
	std::string m_strSoundName;

	/// index to a sound
	int m_iIndex;

public:

	CSoundHandle() : m_iIndex(UNINITIALIZED) {}

	CSoundHandle( const std::string& sound_name ) : m_strSoundName(sound_name), m_iIndex(UNINITIALIZED) {}

//	inline int GetIndex() { return m_iIndex; }

	inline std::string &GetSoundName() { return m_strSoundName; }

	inline void SetSoundName( const std::string& sound_name )
	{
		m_strSoundName = sound_name;
		m_iIndex = UNINITIALIZED;	// clear the index when a new name is set
	}

	enum eSoundIndex { INVALID_INDEX = -1, UNINITIALIZED = -2 };

	friend class CGameSoundManager;
};



#endif		/*  __SOUNDHANDLER_H__  */
