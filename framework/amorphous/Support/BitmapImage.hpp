#ifndef __amorphous_BitmapImage_HPP__
#define __amorphous_BitmapImage_HPP__


#ifdef BUILD_WITH_FREEIMAGE

#include "BitmapImage_FreeImage.hpp"

#else /* BUILD_WITH_FREEIMAGE */

#include "BitmapImage_stb.hpp"

#endif/* BUILD_WITH_FREEIMAGE */


#endif /* __amorphous_BitmapImage_HPP__ */
