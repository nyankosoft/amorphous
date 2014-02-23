#ifndef __GAMELIB1_BEHANDLE_SERIALIZATION_H__
#define __GAMELIB1_BEHANDLE_SERIALIZATION_H__


#include "BaseEntityHandle.hpp"
#include "amorphous/Support/Serialization/Archive.hpp"


namespace amorphous
{

namespace serialization
{


inline IArchive& operator & ( IArchive& ar, BaseEntityHandle& entity_handle )
{
	std::string name;
	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// restore the entity name from the archive
		ar & name;
		entity_handle.SetBaseEntityName( name.c_str() );

	}
	else
	{
		// record the entity name to the archive
		name = entity_handle.GetBaseEntityName();
		ar & name;
	}

	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __GAMELIB1_BEHANDLE_SERIALIZATION_H__  */
