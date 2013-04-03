#ifndef __BasicMaterialParams_HPP__
#define __BasicMaterialParams_HPP__


#include "../../Support/Serialization/SerializationEx.hpp"


namespace amorphous
{
using namespace serialization;


class CBasicMaterialParams : public IArchiveObjectBase
{
public:
	float fLuminosity;
	float fSpecularity;
	float fGlossiness;
	float fReflection;

public:

	CBasicMaterialParams()
		:
	fLuminosity(0),
	fSpecularity(0),
	fGlossiness(0),
	fReflection(0)
	{}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & fLuminosity;
		ar & fSpecularity;
		ar & fGlossiness;
		ar & fReflection;
	}
};


} // amorphous



#endif /* __BasicMaterialParams_HPP__ */
