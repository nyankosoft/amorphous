#ifndef __Serialization_ShaderTechniqueHandle_H__
#define __Serialization_ShaderTechniqueHandle_H__


#include "ShaderTechniqueHandle.hpp"
#include "gds/Support/Serialization/Archive.hpp"


namespace GameLib1
{

namespace Serialization
{


inline IArchive& operator & ( IArchive& ar, CShaderTechniqueHandle& rShaderTechniqueHandle )
{
	std::string strTemp;
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
