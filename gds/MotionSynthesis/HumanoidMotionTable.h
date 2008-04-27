#ifndef  __HumanoidMotionTable_H__
#define  __HumanoidMotionTable_H__


#include <vector>
#include <string>

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;


namespace msynth
{


/**
 used by CHumanoidMotionTable

*/
class CHumanoidMotionEntry : public IArchiveObjectBase
{
public:

	std::string m_MotionType; /// e.g.) Walk, Run, etc.

	std::vector<std::string> m_vecMotionPrimitiveName;

public:

	void Serialize( IArchive & ar, const unsigned int version )
	{
		ar & m_MotionType & m_vecMotionPrimitiveName;
	}
};


class CHumanoidMotionTable : public IArchiveObjectBase
{
public:

	std::string m_Name;

	std::vector<CHumanoidMotionEntry> m_vecEntry;

public:

	void Serialize( IArchive & ar, const unsigned int version )
	{
		ar & m_vecEntry;
	}
};


} // namespace msynth


#endif /* __HumanoidMotionTable_H__ */
