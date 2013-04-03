#include "GameEvent.hpp"
#include "GE_TextEvent.hpp"
#include "GE_SpawnEvent.hpp"


namespace amorphous
{


CGameEvent::CGameEvent()
{
	m_Type = -1;

	m_iEventCount = GTS_EVENTCOUNT_INFINITE;
	m_AABB.Nullify();
}


CGameEvent::~CGameEvent()
{
}


CGameEvent *CGameEvent::CreateObject(const unsigned int id)
{
	switch( id )
	{
	case TEXT_MESSAGE:	return new CGE_TextEvent;
	case ENTITY_SPAWN:	return new CGE_SpawnEvent;
//	case MOVIE:			return new CDerived1;
	default:	return NULL;
	}
}


void CGameEvent::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_Type;

	ar & m_strEventName;
	ar & m_AABB;

	ar & m_iEventCount;

	ar & m_vecCondition;
}



IArchiveObjectBase *CEventFactory::CreateObject( const unsigned int id )
{
	switch( id )
	{
	case CGameEvent::TEXT_MESSAGE:	return new CGE_TextEvent;
	case CGameEvent::ENTITY_SPAWN:	return new CGE_SpawnEvent;
//	case MOVIE:			return new CDerived1;
	default:	return NULL;
	}
}


} // namespace amorphous
