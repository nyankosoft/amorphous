#ifndef __BasicMaterialParams_HPP__
#define __BasicMaterialParams_HPP__


#include "../../Support/Serialization/SerializationEx.hpp"
using namespace GameLib1::Serialization;


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



#endif /* __BasicMaterialParams_HPP__ */
