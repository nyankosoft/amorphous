#ifndef __KGL_3DCOMMON_SERIALIZATION_H__
#define __KGL_3DCOMMON_SERIALIZATION_H__


#include "../../Graphics/TextureCoord.hpp"

#include "Archive.hpp"


namespace amorphous
{

namespace serialization
{


inline IArchive& operator & ( IArchive& ar, TEXCOORD2& tex )
{
	ar & tex.u & tex.v;

	return ar;
}


} // namespace serialization

} // namespace amorphous


#endif  /*  __KGL_3DCOMMON_SERIALIZATION_H__  */
