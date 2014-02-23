
#ifndef __KGL_LIGHT_SERIALIZATION_H__
#define __KGL_LIGHT_SERIALIZATION_H__


#include "Serialization_3DMath.hpp"
#include "Serialization_Color.hpp"


namespace amorphous
{

namespace serialization
{

/*
inline IArchive& operator & ( IArchive& ar, CHemisphericLightAttribute& hs )
{
	ar & hs.UpperColor & hs.LowerColor;

	for( int i=0; i<3; i++ )
		ar & light.fAttenuation[i];

	return ar;
}


inline IArchive& operator & ( IArchive& ar, CHemisphericPointLight& light )
{
	ar & light.vPosition;
	ar & light.Att;

	for( int i=0; i<3; i++ )
		ar & light.fAttenuation[i];

	return ar;
}


inline IArchive& operator & ( IArchive& ar, CHemisphericDirectionalLight& light )
{
	ar & light.vDirection;
	ar & light.Attribute;

	return ar;
}
*/

} // namespace serialization

} // namespace amorphous


#endif  /*  __KGL_LIGHT_SERIALIZATION_H__  */
