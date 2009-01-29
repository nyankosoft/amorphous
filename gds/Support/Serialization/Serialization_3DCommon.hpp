#ifndef __KGL_3DCOMMON_SERIALIZATION_H__
#define __KGL_3DCOMMON_SERIALIZATION_H__


#include "../../Graphics/TextureCoord.h"

#include "Archive.h"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, TEXCOORD2& tex )
{
	ar & tex.u & tex.v;

	return ar;
}


}	  /*  Serialization  */

}	  /*  GameLib1  */


#endif  /*  __KGL_3DCOMMON_SERIALIZATION_H__  */
