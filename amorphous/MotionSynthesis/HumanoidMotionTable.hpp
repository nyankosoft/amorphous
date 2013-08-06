#ifndef  __HumanoidMotionTable_H__
#define  __HumanoidMotionTable_H__


#include <vector>
#include <string>

#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{
using namespace serialization;


namespace msynth
{


/**
 used by HumanoidMotionTable

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


class HumanoidMotionTable : public IArchiveObjectBase
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

} // namespace amorphous



#endif /* __HumanoidMotionTable_H__ */
