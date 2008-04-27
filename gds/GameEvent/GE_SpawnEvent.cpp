
#include "GE_SpawnEvent.h"


void CGE_SpawnEvent::Serialize( IArchive& ar, const unsigned int version )
{
	CGameEvent::Serialize( ar, version );

	ar & m_vecEntity;
}