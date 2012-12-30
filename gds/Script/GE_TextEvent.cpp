
#include "GE_TextEvent.hpp"


namespace amorphous
{


void CGE_TextEvent::Serialize( IArchive& ar, const unsigned int version )
{
	CGameEvent::Serialize( ar, version );

	ar & m_TextSet;
}

} // namespace amorphous
