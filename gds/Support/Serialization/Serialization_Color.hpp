#ifndef __KGL_COLOR_SERIALIZATION_H__
#define __KGL_COLOR_SERIALIZATION_H__


#include "../../Graphics/FloatRGBColor.hpp"
#include "../../Graphics/FloatRGBAColor.hpp"

#include "Archive.hpp"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, SFloatRGBColor& color )
{
	ar & color.fRed;
	ar & color.fGreen;
	ar & color.fBlue;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, SFloatRGBAColor& color )
{
	ar & color.fRed;
	ar & color.fGreen;
	ar & color.fBlue;
	ar & color.fAlpha;

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_COLOR_SERIALIZATION_H__  */
