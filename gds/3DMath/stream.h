#ifndef __3DMATH_STREAM_H__
#define __3DMATH_STREAM_H__


#include <ostream>

#include "aabb3.h"


inline std::ostream& operator << ( std::ostream& o, const AABB3& aabb )
{
    o << "min( ";
	o << aabb.vMin.x << ", " << aabb.vMin.y << ", " << aabb.vMin.z;
	o << " ) - max( ";
	o << aabb.vMax.x << ", " << aabb.vMax.y << ", " << aabb.vMax.z;
	o << " )";

/*	for (size_t i = 0; i < 4; ++i)
    {
        o << " row" << (unsigned)i << "{";
        for(size_t j = 0; j < 4; ++j)
        {
            o << m[i][j] << " ";
        }
        o << "}";
    }
    o << ")";*/
    return o;
}


#endif /* __3DMATH_STREAM_H__ */
