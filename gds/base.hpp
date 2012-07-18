#ifndef __BASETYPES_H__
#define __BASETYPES_H__


//==========================================================================
// platform-independent settings
//==========================================================================

//
// typedefs
//

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


#include <boost/integer.hpp>

typedef boost::uint8_t  U8;
typedef boost::uint16_t U16;
typedef boost::uint32_t U32;
typedef boost::uint64_t U64;

typedef boost::int8_t  I8;
typedef boost::int16_t I16;
typedef boost::int32_t I32;
typedef boost::int64_t I64;


//
// macros
//

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif


//
// inline functions
//

template<class T>
inline T get_clamped( const T& val, const T& min_val, const T& max_val )
{
	if( val < min_val )
		return min_val;
	else if( max_val < val )
		return max_val;
	else
		return val;
}


template<class T>
inline void clamp( T& val, const T& min_val, const T& max_val )
{
	val = get_clamped( val, min_val, max_val );
}


template<typename T>
inline T take_min( const T& a, const T& b )
{
	return a < b ? a : b;
}


template<typename T>
inline T take_max( const T& a, const T& b )
{
	return a < b ? b : a;
}


template<typename T>
inline T take_min( const T& a, const T& b, const T& c )
{
	return take_min( take_min( a, b ), c );
}


template<typename T>
inline T take_max( const T& a, const T& b, const T& c )
{
	return take_max( take_max( a, b ), c );
}



//
// function results
//

class Result
{
public:
	enum Name
	{
		SUCCESS = 0,        ///< succeeded
		OUT_OF_MEMORY,      ///< not enough memory
		INVALID_ARGS,       ///< invalid argument(s)
		RESOURCE_NOT_FOUND, ///< the requested resource was not found (to be used by async loader).
		RESOURCE_IN_USE,    ///< the resource is being used by another thread (to be used by async loader).
		UNKNOWN_ERROR,      ///< an unknown error
		NUM_RESULTS
	};
};



//==========================================================================
// platform-dependent settings
//==========================================================================

#ifdef _MSC_VER

#define NOMINMAX // avoid error in nxmath.h of Ageia PhysX SDK
#include <Windows.h> // "#include <GL/gl.h>" causes lots of mysterious errors when Windows.h is not included before it.

#else // _MSC_VER

// Put things linux needs in common.

#endif // _MSC_VER


#endif  /* __BASETYPES_H__ */
