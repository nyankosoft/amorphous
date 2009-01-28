
#ifndef  __GAMEEVENT_SPAWNEVENT_H__
#define  __GAMEEVENT_SPAWNEVENT_H__

#include "GameEvent.h"


class CGE_SpawnEvent : public CGameEvent
{
/*	class CEntitySpawnSet : public IArchiveObjectBase
	{
	public:
		Matrix34 matWorldPose;
		string strEntityName;
		
		void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & matWorldPose;
			ar & strEntityName;
		}
	};*/

	vector<CEventEntityDesc> m_vecEntity;

public:

	CGE_SpawnEvent() { m_Type = ENTITY_SPAWN; /*SetArchiveObjectID( m_Type );*/ }

	void AddEntity( CEventEntityDesc& rEntity ) { m_vecEntity.push_back(rEntity); }

	inline vector<CEventEntityDesc>& GetEntity() { return m_vecEntity; }
	
	void Serialize( IArchive& ar, const unsigned int version );

	unsigned int GetArchiveObjectID() { return m_Type; }
};


#endif		/*  __GAMEEVENT_SPAWNEVENT_H__  */