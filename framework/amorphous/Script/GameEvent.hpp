#ifndef  __GAMEEVENTCONDITION_AND_GAMEEVENT_H__
#define  __GAMEEVENTCONDITION_AND_GAMEEVENT_H__


#include "../3DMath/AABB3.hpp"
#include "../Support/Serialization/Serialization.hpp"
#include "../Support/Serialization/Serialization_3DMath.hpp"
#include "../Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace serialization;


class CGameEventCompiler;


//===========================================================================
// CGameEventCondition
//===========================================================================

class CGameEventCondition : public IArchiveObjectBase
{
public:

	enum eEventConditionType
	{
//		NO_CONDITION,
		HAS_ITEM = 0,
		NOT_HAVE_ITEM,
		HAS_KEYCODE,
		NOT_HAVE_KEYCODE,
		NO_ENTITY_IN_AREA,
	};

	int type;	// one of the above conditions

	std::string strData;
	int iData;
	unsigned int uiData;

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & type;
		ar & strData;
		ar & iData;
		ar & uiData;
	}
};



//===========================================================================
// CGameEvent
//   store info of one entity. used by entity-related events. (e.g. CGE_SpawnEvent)
//===========================================================================

class CEventEntityDesc : public IArchiveObjectBase
{
public:
	Matrix34 matWorldPose;
	std::string strEntityName;

	CEventEntityDesc() { matWorldPose.Identity(); }
		
	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & matWorldPose;
		ar & strEntityName;
	}
};



//===========================================================================
// CGameEvent
//   base class of game events
//===========================================================================

#define GTS_EVENTCOUNT_INFINITE		-8


class CGameEvent : public IArchiveObjectBase
{
protected:

	/// event type - for runtime identification
	int m_Type;

	/// event name
	std::string m_strEventName;

	/// event trigger
	AABB3 m_AABB;

	/// decides when the event happens, such as on entering or leaving the trigger volume.
	/// NOT IMPLEMENTED
	// int m_TriggerConditionFlag

	/// how many times the event occurs
	/// setting this to GTS_EVENTCOUNT_INFINITE means event happens
	/// every time a valid entity enters the trigger
	int m_iEventCount;

	/// conditions required for the event to happen
	std::vector<CGameEventCondition> m_vecCondition;

	/// sound to make when the event happens
	std::string m_strSound;

public:

	CGameEvent();
	~CGameEvent();

	inline int GetType() { return m_Type; }

	inline void GetAABB( AABB3& aabb ) { aabb = m_AABB; }
//	void SetAABB( AABB3& aabb );

	inline int GetEventCount() { return m_iEventCount; }
//	void SetEventCounter(int iCount) { m_iEventCount = iCount; }

	void AddEventCondition(CGameEventCondition& rCondition) { m_vecCondition.push_back(rCondition); }

	inline std::vector<CGameEventCondition>& GetCondition() { return m_vecCondition; }

	void Serialize( IArchive& ar, const unsigned int version );

	enum eEventType
	{
		TEXT_MESSAGE,
		ENTITY_SPAWN,
		MUSIC_CONTROL,
		MOVIE,
	};

	static CGameEvent *CreateObject(const unsigned int id);
//	CGameEvent *CreateObject(const unsigned int id);


	friend class CGameEventCompiler;
};


class CEventFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject( const unsigned int id );
};


} // namespace amorphous



#endif		/*  __GAMEEVENTCONDITION_AND_GAMEEVENT_H__  */