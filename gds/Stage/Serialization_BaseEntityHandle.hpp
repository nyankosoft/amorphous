
#ifndef __GAMELIB1_BEHANDLE_SERIALIZATION_H__
#define __GAMELIB1_BEHANDLE_SERIALIZATION_H__


#include "BaseEntityHandle.hpp"

#include "Support/Serialization/Archive.hpp"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, CBaseEntityHandle& entity_handle )
{
	string name;
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


}  /*  Serialization  */

}  /*  GameLib1  */


#endif  /*  __GAMELIB1_BEHANDLE_SERIALIZATION_H__  */