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
	ar & color.red;
	ar & color.green;
	ar & color.blue;

	return ar;
}


inline IArchive& operator & ( IArchive& ar, SFloatRGBAColor& color )
{
	ar & color.red;
	ar & color.green;
	ar & color.blue;
	ar & color.alpha;

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_COLOR_SERIALIZATION_H__  */
