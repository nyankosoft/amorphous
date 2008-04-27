
#ifndef __KGL_LIGHT_SERIALIZATION_H__
#define __KGL_LIGHT_SERIALIZATION_H__


#include "Serialization_3DMath.h"
#include "Serialization_Color.h"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, CHemisphericLight& light )
{
	ar & light.vPosition;
	ar & light.UpperColor;
	ar & light.LowerColor;

	for( int i=0; i<3; i++ )
		ar & light.fAttenuation[i];

	return ar;
}


inline IArchive& operator & ( IArchive& ar, CHemisphericDirLight& light )
{
	ar & light.vDirection;
	ar & light.UpperColor;
	ar & light.LowerColor;

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_LIGHT_SERIALIZATION_H__  */
