#ifndef __KGL_32BITCOLOR_SERIALIZATION_H__
#define __KGL_32BITCOLOR_SERIALIZATION_H__


#include "../../Graphics/32BitColor.hpp"

#include "Archive.hpp"


namespace amorphous
{

namespace serialization
{


inline IArchive& operator & ( IArchive& ar, S32BitColor& color )
{
	ar & color.red();
	ar & color.green();
	ar & color.blue();
	ar & color.alpha();

	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __KGL_32BITCOLOR_SERIALIZATION_H__  */
