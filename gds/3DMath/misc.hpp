/*
  Sss - a slope soaring simulater.
  Copyright (C) 2002 Danny Chapman - flight@rowlhouse.freeserve.co.uk
*/

#ifndef SSS_MISC_H
#define SSS_MISC_H

#include "precision.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

// some useful angle things
#define PI 3.1415926535897932384626433832795
//#define TWO_PI 6.28318530717958647692528676655901
#define PI_DIV_180 0.0174532925199432957692369076848861
#define _180_DIV_PI 57.2957795130823208767981548141052

#ifndef GOLDEN_RATIO
#define GOLDEN_RATIO 1.6180339887
#endif

/// returns a golden ratio
/// \return x such that val : x = 1.0 : 1.6180339887
template<class T>
inline T grof( T val )
{
	return (T)( val * GOLDEN_RATIO );
}


inline Scalar deg_to_rad(Scalar deg) {return (Scalar) (deg * PI_DIV_180);}
inline Scalar rad_to_deg(Scalar rad) {return (Scalar) (rad * _180_DIV_PI);}

inline Scalar sin_deg(Scalar deg) {return (Scalar) sin(deg_to_rad(deg));}
inline Scalar cos_deg(Scalar deg) {return (Scalar) cos(deg_to_rad(deg));}
//inline Scalar asin_deg(Scalar x) {return rad_to_deg(asin((Scalar) x));}	// original
//inline Scalar acos_deg(Scalar x) {return rad_to_deg(acos((Scalar) x));}	// original
inline Scalar asin_deg(Scalar x) {return rad_to_deg((Scalar)asin((Scalar) x));}
inline Scalar acos_deg(Scalar x) {return rad_to_deg((Scalar)acos((Scalar) x));}
inline Scalar tan_deg(Scalar deg) {return (Scalar) tan(deg_to_rad(deg));}
inline Scalar atan2_deg(Scalar x, Scalar y) {return rad_to_deg((Scalar) atan2(x, y));}
inline Scalar atan_deg(Scalar x) {return rad_to_deg((Scalar) atan(x));}

// there is an STL version somewhere...
template<class T>
inline T sss_min(const T a, const T b) {return (a < b ? a : b);}
template<class T>
inline T sss_max(const T a, const T b) {return (a > b ? a : b);}


/*! Indicates if the machine is little-endian */
bool is_little_endian();

//! Takes a 4-byte word and converts it to little endian (if necessary)
void convert_word_to_little_endian(void * orig);
//! Takes a 4-byte word and converts it from little endian to whatever
//! is this machine is (if necessary)
void convert_word_from_little_endian(void * orig);

template<typename T>
T square(T val) 
{
  return val * val;
}

#if 0

#if defined(__APPLE__) || defined(MACOSX) || defined(unix) || defined(linux)
#ifndef linux
#ifndef __APPLE__
#include <ieeefp.h>
#endif
#endif
inline bool is_finite(Scalar val)
{
    return (finite(val) != 0);
}
#else
#include <float.h>
inline bool is_finite(Scalar val)
{
    return (_finite(val) != 0);
}
#endif

#else // use a hand-crafted is_finite - catches more
inline bool is_finite(Scalar val)
{
  return ((val < 1e8) && (val > -1e8));
}

#endif


#endif







