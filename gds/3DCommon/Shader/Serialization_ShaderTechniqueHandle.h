
#ifndef __Serialization_ShaderTechniqueHandle_H__
#define __Serialization_ShaderTechniqueHandle_H__


#include "ShaderTechniqueHandle.h"

#include "Support/Serialization/Archive.h"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, CShaderTechniqueHandle& rShaderTechniqueHandle )
{
	string strTemp;
	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// restore the technique name from the archive
		ar & strTemp;
		rShaderTechniqueHandle.SetTechniqueName( strTemp.c_str() );
	}
	else
	{
		// record the technique name to the archive
		strTemp = rShaderTechniqueHandle.GetTechniqueName();
		ar & strTemp;
	}

	return ar;
}


}  /*  Serialization  */

}  /*  GameLib1  */


#endif  /*  __Serialization_ShaderTechniqueHandle_H__  */
