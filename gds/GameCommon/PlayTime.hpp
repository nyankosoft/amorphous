#ifndef __PLAYTIME_H__
#define __PLAYTIME_H__


#include <string>

#include "gds/Support/Timer.hpp"
#include "gds/Support/Serialization/Serialization.hpp"


namespace amorphous
{
using namespace serialization;


class PlayTime : public IArchiveObjectBase
{
	unsigned long m_LastSavedTimeMS;

	Timer m_Timer;

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

	PlayTime() : m_LastSavedTimeMS(0) { m_Timer.Start(); }

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


inline unsigned long PlayTime::GetCurrentPlayTimeInSec() const
{
	return GetCurrentPlayTimeInMS() / 1000;
}


inline unsigned long PlayTime::GetCurrentPlayTimeInMS() const
{
	return m_LastSavedTimeMS + m_Timer.GetTimeMS();
}


inline std::string PlayTime::GetCurrentPlayTimeString( int format ) const
{
	std::string dest;
	GetCurrentPlayTimeString( dest, format );
	return dest;
}


inline void PlayTime::GetCurrentPlayTimeString( std::string& dest, int format ) const
{
	unsigned long total_playtime_sec = GetCurrentPlayTimeInSec();
	unsigned long playtime_hours   = total_playtime_sec / 3600;
	unsigned long playtime_minutes = ( total_playtime_sec - (playtime_hours * 3600) ) / 60;
	unsigned long playtime_sec     = total_playtime_sec % 60;

	char buf[16];
	sprintf( buf, "%02d:%02d", playtime_hours, playtime_minutes );

	dest = buf;
}

} // namespace amorphous



#endif /* __PLAYTIME_H__ */
