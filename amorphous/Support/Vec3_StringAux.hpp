#ifndef __VEC3_STRINGAUX_H__
#define __VEC3_STRINGAUX_H__


#include "StringAux.hpp"

#include "../3DMath/AABB3.hpp"
#include "../3DMath/OBB3.hpp"
#include "../3DMath/Quaternion.hpp"
#include "../3DMath/AABB2.hpp"


namespace amorphous
{


inline std::string to_string( const Vector3& v, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + ", " + fmt_float + " )";

	char buffer[64];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), v.x, v.y, v.z );

	return std::string(buffer);
}


inline std::string to_string( const AABB3& aabb )
{
	return std::string( "[aabb3] min" + to_string(aabb.vMin) + ", max" + to_string(aabb.vMax) );
}


inline std::string to_string( const SPlane& plane )
{
	return std::string( "[plane] d: " + to_string(plane.dist) + ", n" + to_string(plane.normal) );
}


inline std::string to_string( const Sphere& sphere )
{
	return std::string( "[sphere] c" + to_string(sphere.center) + ", r: " + to_string(sphere.radius) );
}


inline std::string to_string( const Vector2& v, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + " )";

	char buffer[64];
	sprintf( buffer, fmt_buffer.c_str(), v.x, v.y );

	return std::string(buffer);
}


inline std::string to_string( const AABB2& aabb )
{
	return std::string( "[aabb2] min" + to_string(aabb.vMin) + ", max" + to_string(aabb.vMax) );
}


inline std::string to_string( const Quaternion& q, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + ", " + fmt_float + ", " + fmt_float + " )";

	char buffer[80];
	memset( buffer, 0, sizeof(buffer) );
	snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), q.x, q.y, q.z, q.w );

	return std::string(buffer);
}


inline std::string to_string( const OBB3& obb )
{
	return std::string( "[obb3] radii" + to_string(obb.radii) + ", center" + to_string(obb.center.vPosition) + ", rotation" + to_string(Quaternion(obb.center.matOrient)) );
}


} // namespace amorphous



#endif  /*  __VEC3_STRINGAUX_H__  */
