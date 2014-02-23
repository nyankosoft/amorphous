#ifndef __Serialization_linear_interpolation_HPP__
#define __Serialization_linear_interpolation_HPP__

#include "../linear_interpolation.hpp"

#include "Archive.hpp"


namespace amorphous
{

namespace serialization
{


template <typename T>
inline IArchive& operator & ( IArchive& ar, linear_interpolation_table<T>& obj )
{
	ar & obj.table();

	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __Serialization_linear_interpolation_HPP__  */
