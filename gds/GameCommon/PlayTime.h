#ifndef __PLAYTIME_H__
#define __PLAYTIME_H__


#include <string>

#include "Support/Timer.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CPlayTime : public IArchiveObjectBase
{
	unsigned long m_LastSavedTimeMS;

	CTimer m_Timer;

public:

	enum eFormat
	{
		FMT_HHMM = 0, ///< hours:minitues with full z-fills
		FMT_hhmm,     ///< no z-fills?
		FMT_hhMM,
		FMT_HHMMSS,
		NUM_FORMATS
	};

public:

	CPlayTime() : m_LastSavedTimeMS(0) { m_Timer.Start(); }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			m_Timer.Reset();
		}
		else
		{
		}

		ar & m_LastSavedTimeMS;
	}

	inline unsigned long GetCurrentPlayTimeInSec() const;
	inline unsigned long GetCurrentPlayTimeInMS() const;

	inline void GetCurrentPlayTimeString( std::string& dest, int format = FMT_HHMM ) const;
	inline std::string GetCurrentPlayTimeString( int format ) const;
};


inline unsigned long CPlayTime::GetCurrentPlayTimeInSec() const
{
	return GetCurrentPlayTimeInMS() / 1000;
}


inline unsigned long CPlayTime::GetCurrentPlayTimeInMS() const
{
	return m_LastSavedTimeMS + m_Timer.GetTimeMS();
}


inline std::string CPlayTime::GetCurrentPlayTimeString( int format ) const
{
	std::string dest;
	GetCurrentPlayTimeString( dest, format );
	return dest;
}


inline void CPlayTime::GetCurrentPlayTimeString( std::string& dest, int format ) const
{
	unsigned long total_playtime_sec = GetCurrentPlayTimeInSec();
	unsigned long playtime_hours   = total_playtime_sec / 3600;
	unsigned long playtime_minutes = ( total_playtime_sec - (playtime_hours * 3600) ) / 60;
	unsigned long playtime_sec     = total_playtime_sec % 60;

	char buf[16];
	sprintf( buf, "%02d:%02d", playtime_hours, playtime_minutes );

	dest = buf;
}


#endif /* __PLAYTIME_H__ */
