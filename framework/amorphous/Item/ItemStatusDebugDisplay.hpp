#ifndef __ItemStatusDebugDisplay_HPP__
#define __ItemStatusDebugDisplay_HPP__


#include "GameItem.hpp"
#include "amorphous/Support/DebugOutput.hpp"


namespace amorphous
{


class ItemStatusDebugDisplay : public DebugInfo
{
	std::weak_ptr<GameItem> m_pItem;

public:

	ItemStatusDebugDisplay(){}

	ItemStatusDebugDisplay( std::weak_ptr<GameItem> pItem ) : m_pItem(pItem) {}

	~ItemStatusDebugDisplay(){}

	void SetItem( std::weak_ptr<GameItem> pItem ) { m_pItem = pItem; }

	void UpdateDebugInfoText()
	{
		m_TextBuffer.resize( 0 );

		std::shared_ptr<GameItem> pItem = m_pItem.lock();

		if( pItem )
		{
			pItem->GetStatusForDebugging( m_TextBuffer );
		}
		else
		{
			m_TextBuffer += "The item has not been set or has already been released.";
		}
	}
};


} // amorphous



#endif /* __ItemStatusDebugDisplay_HPP__ */
