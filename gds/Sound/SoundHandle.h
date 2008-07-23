#ifndef  __SoundHandle_H__
#define  __SoundHandle_H__


#include <string>
#include "fwd.h"
#include "Support/prealloc_pool.h"
#include "Support/Serialization/ArchiveObjectBase.h"
#include "Support/Serialization/Archive.h"
using namespace GameLib1;
using namespace Serialization;


/// only for non-streamed sound
/// - Use this as a sound argument of CSoundManager().Play(), PlayAt(), etc.
/// - cache the pointer to the sound buffer when the sound is played for the first time
///   - Sound manager can skip the search in subsequent calls of CSoundManager().Play(), PlayAt(), etc.
class CSoundHandle : public IArchiveObjectBase
{
	pooled_object_handle m_Handle;

	/// name of a sound
	std::string m_ResourceName;

public:

	CSoundHandle() {}

	CSoundHandle( const std::string& sound_name )
		:
	m_ResourceName(sound_name)
	{}

	inline const std::string &GetResourceName() const { return m_ResourceName; }

	inline void SetResourceName( const std::string& sound_resource_name )
	{
		m_ResourceName = sound_resource_name;
		m_Handle = pooled_object_handle(); // reset the handle
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_ResourceName;

		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			// initialize the handle
			m_Handle = pooled_object_handle();
		}
	}

//	friend class CSoundManager;
	friend class CSoundManagerImpl;
};


#endif		/*  __SoundHandle_H__  */
