
#ifndef __KGL_32BITCOLOR_SERIALIZATION_H__
#define __KGL_32BITCOLOR_SERIALIZATION_H__


#include "../../3DCommon/32BitColor.h"

#include "Archive.h"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, S32BitColor& color )
{
	ar & color.red();
	ar & color.green();
	ar & color.blue();
	ar & color.alpha();

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_32BITCOLOR_SERIALIZATION_H__  */
