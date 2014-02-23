#ifndef __KGL_BoostSmartPtr_SERIALIZATION_H__
#define __KGL_BoostSmartPtr_SERIALIZATION_H__

#include <boost/shared_ptr.hpp>

#include "Archive.hpp"


namespace amorphous
{

namespace serialization
{


/// Works only when the shared_ptr is not shared.
template <class T>
inline IArchive& operator & ( IArchive& ar, boost::shared_ptr<T>& ptr )
{
	int saved = 0;
	if( ar.GetMode() == IArchive::MODE_OUTPUT )
	{
		if( ptr )
		{
			// save the object
			saved = 1;
			ar & saved;
			ar & *(ptr.get());
		}
		else
		{
			// ptr is NULL - do not serialize
			saved = 0;
			ar & saved;
		}
	}
	else // i.e. input mode
	{
		ptr.reset();

		ar & saved;

		// load only if the object is in the stream
		if( saved == 1 )
		{
			// load the object
			ptr = boost::shared_ptr<T>( new T() );
			ar & *(ptr.get());
		}


	}

	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __KGL_BoostSmartPtr_SERIALIZATION_H__  */
