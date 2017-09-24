#ifndef __PlayerCustomer_HPP__
#define __PlayerCustomer_HPP__


#include "GameItemShop.hpp"
#include "amorphous/Stage/PlayerInfo.hpp"


namespace amorphous
{


class PlayerCustomer : public CCustomer
{
public:

	virtual int AddItem( std::shared_ptr<GameItem> pItem )
	{
		if( !pItem )
			return 0;

//		GameItem* pTgtItem = SinglePlayerInfo().GetItemByName( pItem->GetName() );
//		int orig_quantity = 0;
//		if( pTgtItem ) orig_quantity = pTgtItem->GetCurrentQuantity();

		return SinglePlayerInfo().SupplyItem( pItem );
	}

	virtual bool Pay( int amount )
	{
		if( SinglePlayerInfo().m_Money < amount )
			return false;

		SinglePlayerInfo().m_Money -= amount;
		return true;
	}

	virtual int GetMoneyLeft() const { return SinglePlayerInfo().m_Money; }

	virtual const GameItem& GetItem( const std::string& name ) { return *SinglePlayerInfo().GetItemByName<GameItem>( name.c_str() ).get(); }
};


} // namespace amorphous


#endif /* __PlayerCustomer_HPP__ */
