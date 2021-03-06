#ifndef __GE_TEXTEVENT_H__
#define __GE_TEXTEVENT_H__


#include "GameEvent.hpp"
#include "../GameTextSystem/GameTextSet.hpp"


namespace amorphous
{


class CGE_TextEvent : public CGameEvent
{
	CGameTextSet m_TextSet;

public:

	CGE_TextEvent() { m_Type = TEXT_MESSAGE; /*SetArchiveObjectID( m_Type );*/ }

	inline CGameTextSet& GetTextSet() { return m_TextSet; }

	void AddTextComponent( CGameTextComponent& rTextComponent ) { m_TextSet.AddTextComponent(rTextComponent); }

	void Serialize( IArchive& ar, const unsigned int version );

	unsigned int GetArchiveObjectID() const { return m_Type; }
};

} // namespace amorphous



#endif  /*  __GE_TEXTEVENT_H__  */