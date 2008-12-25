
#include "GE_TextEvent.h"


void CGE_TextEvent::Serialize( IArchive& ar, const unsigned int version )
{
	CGameEvent::Serialize( ar, version );

	ar & m_TextSet;
}