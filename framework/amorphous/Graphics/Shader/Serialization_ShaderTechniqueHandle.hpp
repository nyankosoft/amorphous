#ifndef __Serialization_ShaderTechniqueHandle_H__
#define __Serialization_ShaderTechniqueHandle_H__


#include "ShaderTechniqueHandle.hpp"
#include "amorphous/Support/Serialization/Archive.hpp"


namespace amorphous
{

namespace serialization
{


inline IArchive& operator & ( IArchive& ar, ShaderTechniqueHandle& rShaderTechniqueHandle )
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


} // namespace serialization

} // namespace amorphous


#endif  /*  __Serialization_ShaderTechniqueHandle_H__  */
