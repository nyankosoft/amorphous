#ifndef __ASSAULTRECORD_H__
#define __ASSAULTRECORD_H__


#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace serialization;


/**
 * holds destroyed entities records
 * name & type are copied from KillReport
 * some information exist in KillReport is omitted
 */
class KillRecord : public IArchiveObjectBase
{
public:
	std::string name;	///< copied from KillReport::entity_name
	int num_destroyed;	///< how many entities of this type has been destroyed

	int type;	///< type of the destroyed target

public:

	KillRecord() : num_destroyed(0), type(0) {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & name & num_destroyed;
		ar & type;
	}
};


class CAssaultRecord : public IArchiveObjectBase
{
	std::vector<KillRecord> m_vecKillRecord;

public:

	void AddRecord( const CombatReocrd& combat_record )
	{
		size_t i, num = m_vecKillRecord.size();
		for( i=0; i<num; i++ )
		{
			const KillReport& rep = combat_record.m_vecKillReport[i];
			if( rep.entity_name == rec.name )
			{
				rec.num_destroyed++;
				return;
			}
		}

		// create a new item
		m_vec.push_back( KillRecord() );
		m_vec.back().num_destroyed = 1;
		m_vec.back().name = rep.entity_name;
	}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecKillRecord;
	}

};


/*
class CSaveObjectBase : public IArchiveObjectBase
{
public:

	CSaveObjectBase() { SaveObjectManager.Regsiter(this); }

	~CSaveObjectBase() { SaveObjectManager.Release(this); }
};
*/

} // namespace amorphous



#endif /* __ASSAULTRECORD_H__ */
