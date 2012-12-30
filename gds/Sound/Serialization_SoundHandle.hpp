#ifndef __GAMELIB1_SOUNDHANDLE_SERIALIZATION_H__
#define __GAMELIB1_SOUNDHANDLE_SERIALIZATION_H__


#include "SoundHandle.hpp"
#include "../Support/Serialization/Archive.hpp"


namespace amorphous
{

namespace serialization
{


inline IArchive& operator & ( IArchive& ar, CSoundHandle& rSoundHandle )
{
/*	string strTemp;
	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// restore the sound name from the archive
		ar & strTemp;
		rSoundHandle.SetResourceName( strTemp.c_str() );
	}
	else
	{
		// record the entity name to the archive
		strTemp = rSoundHandle.GetSoundName();
		ar & strTemp;
	}
*/
	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __GAMELIB1_SOUNDHANDLE_SERIALIZATION_H__  */