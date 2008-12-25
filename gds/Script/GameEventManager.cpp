
#include "GameEventManager.h"

#include "Support/memory_helpers.h"


CGameEventManager::CGameEventManager()
{
}


CGameEventManager::~CGameEventManager()
{
	int i;
	for( i=0; i<m_vecpGameEvent.size(); i++ )
		SafeDelete( m_vecpGameEvent[i] );
}


void CGameEventManager::AddEvent( CGameEvent *pEvent )
{
	m_vecpGameEvent.push_back( pEvent );
}


void CGameEventManager::Serialize( IArchive& ar, const unsigned int version )
{
//	ar & m_vecpGameEvent;

	CEventFactory factory;
	ar.Polymorphic( m_vecpGameEvent, factory );
}


bool CGameEventManager::SaveToFile( const char *pcFilename )
{
	CBinaryArchive_Output archive( pcFilename );

	archive << (*this);

	return true;
}


bool CGameEventManager::LoadFromFile( const char *pcFilename )
{
	CBinaryArchive_Input archive( pcFilename );

	archive >> (*this);

	return true;
}