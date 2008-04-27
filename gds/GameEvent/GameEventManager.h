#ifndef  __GAMEEVENTMANAGER_H__
#define  __GAMEEVENTMANAGER_H__

#include "GameEvent.h"

#include <vector>
using namespace std;


class CGameEventManager : public IArchiveObjectBase
{
	vector<CGameEvent *> m_vecpGameEvent;


public:
	CGameEventManager();
	~CGameEventManager();

	void AddEvent( CGameEvent *pEvent );

	int GetNumEvents() { return m_vecpGameEvent.size(); }

	CGameEvent *GetEvent( int index ) { return m_vecpGameEvent[index]; }

	void Serialize( IArchive& ar, const unsigned int version );

	bool SaveToFile( const char *pcFilename );
	bool LoadFromFile( const char *pcFilename );
};



#endif		/*  __GAMEEVENTMANAGER_H__  */