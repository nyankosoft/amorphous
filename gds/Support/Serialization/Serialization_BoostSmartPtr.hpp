#ifndef __KGL_BoostSmartPtr_SERIALIZATION_H__
#define __KGL_BoostSmartPtr_SERIALIZATION_H__

#include <boost/shared_ptr.hpp>

#include "Archive.h"


namespace GameLib1
{

namespace Serialization
{


template <class T>
inline IArchive& operator & ( IArchive& ar, boost::shared_ptr<T>& ptr )
{
	if( ar.GetMode() == IArchive::MODE_OUTPUT )
	{
		ar & *(ptr.get());
	}
	else // i.e. input mode
	{
		ptr.reset();

		ptr = boost::shared_ptr<T>( new T() );

		ar & *(ptr.get());
	}

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_BoostSmartPtr_SERIALIZATION_H__  */
