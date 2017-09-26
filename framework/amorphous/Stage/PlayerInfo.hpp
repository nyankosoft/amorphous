#ifndef  __PLAYERINFO_H__
#define  __PLAYERINFO_H__

#include "fwd.hpp"
#include "BE_Player.hpp"
#include "amorphous/Task/GameTask.hpp"
#include "amorphous/GameCommon/KeyBind.hpp"
#include "amorphous/GameCommon/SaveDataManager.hpp"
#include "amorphous/GameCommon/PlayTime.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/singleton.hpp"
#include <memory>


namespace amorphous
{

// forward declerations
class GameItem;
class CWeaponSystem;

class CGI_Aircraft;


class CItemCategory
{
public:
	enum Name
	{
		WEAPON = 0,
		AMMUNITION,
		AMMO_762X51,
		AMMO_12GAUGE,
		AMMO_40MMGRENADE,
		AMMO_MISSILE,
		AMMO_RIGIDBODY,		// for physics test
		SUPPLY,
		UTILITY,
		KEY,
		NUM_CATEGORIES
	};
};



/// holds properties of the single player in the client application
/// - singleton
/// - holds the following items
///   - owned references of game items
///   - a borrowed reference of a copy entity that represents the player in the stage
///   - key binds
///   - other settings & properties of the single player
class CSinglePlayerInfo : public CSaveDataComponent
{
private:

	/// holds a base entity currently used by the player in the stage.
	/// (borrowed reference)
	/// - set by a player entity in CBE_Player::InitCopyEntity()
	/// - detached in CBE_Player::OnEntityDestroyed()
	CBE_Player *m_pCurrentPlayerBaseEntity;

	std::weak_ptr<CStage> m_pCurrentStage;

	CWeaponSystem *m_pWeaponSystem;

	/// used to change tasks
//	GameTask::eGameTask m_TaskID;
	int m_TaskID;

	/// a list of items the player is carrying
	std::vector< std::shared_ptr<GameItem> > m_vecpItem;

	/// lists of items sorted by categories
	std::vector< std::shared_ptr<GameItem> > m_vecpCategoryItem[CItemCategory::NUM_CATEGORIES];

	/// borrowed ref?
	std::vector<GameItem *> m_vecpActiveItem;

	/// borrowed ref?
	GameItem *m_pFocusedItem;

	int m_Money;

	/// aircraft used by flight games
	/// a valid aircraft item has to be set before a stage is loaded.
	/// During the stage initialization, base entities are loaded and initialized.
	/// In that phase, one base entity, 'CBE_PlayerPseudoAircraft' access this variable
	/// and saves the borrowed reference, then use it as the player's aircraft in the stage
	std::shared_ptr<CGI_Aircraft> m_pCurrentAircraft;

	KeyBind m_KeyBind;

	PlayTime m_PlayTime;

private:

	void AddItemToCategoryList( std::shared_ptr<GameItem> pItem );

protected:

	static singleton<CSinglePlayerInfo> m_obj;

public:

	CSinglePlayerInfo();

	~CSinglePlayerInfo();

	static CSinglePlayerInfo* Get() { return m_obj.get(); }

	void Release();

	inline CBE_Player *GetCurrentPlayerBaseEntity() { return m_pCurrentPlayerBaseEntity; }

	void SetStage( CStageWeakPtr pStage );

	CCopyEntity *GetCurrentPlayerEntity();

	inline void SetPlayerBaseEntity(CBE_Player *pPlayer) { m_pCurrentPlayerBaseEntity = pPlayer; }

	inline CWeaponSystem *GetWeaponSystem() { return m_pWeaponSystem; }

//	inline GameTask::eGameTask GetTaskRequest() { return m_TaskID; }
	inline int GetTaskRequest() { return m_TaskID; }

	// request a task change
//	inline void RequestTaskChange( GameTask::eGameTask task_id ) { m_TaskID = task_id; }
	inline void RequestTaskChange( int task_id ) { m_TaskID = task_id; }

	/// --- item control ---

	/// \return actual supplied quantity
	int SupplyItem( const char *pcItemName, const int iSupplyQuantity );

	/// \return actual supplied quantity
	inline int SupplyItem( const std::string item_name, const int iSupplyQuantity );

	/// add an owned reference of an item to the item list
	/// TODO: what if the player already has the item
	/// currently,
	/// 1. call SupplyItem( pItem->Name )
	/// 2. delete pItem
	/// \return actual supplied quantity
	int SupplyItem( GameItem* pItem );

	/// Add an owned reference of an item to the item list
	/// - The argument pItem is not stored to the item array of the player.
	/// - A copy of the item is drawn from game item database and stored in the item array
	///   of the player.
	int SupplyItem( std::shared_ptr<GameItem> pItem );

	const std::vector<std::shared_ptr<GameItem>>& GetItemList() { return m_vecpItem; }

	template<class CItemType>
	inline std::shared_ptr<CItemType> GetItemByName( const char *pcItemName );

	inline std::vector<std::shared_ptr<GameItem>>& GetCategoryItemList( int category ) { return m_vecpCategoryItem[category]; }

	inline void AddActiveItem( GameItem *pItem );
	inline void ReleaseActiveItem( GameItem *pItem );


	/// select an item to operate
	inline void SetItemFocus( GameItem *pItem ) { m_pFocusedItem = pItem; }

	/// release focus from the item currently in operation
	inline void ReleaseItemFocus() { m_pFocusedItem = NULL; }

	inline GameItem *GetFocusedItem() { return m_pFocusedItem; }  

	inline bool HasKeycode( const char *pcKeyCode) { return false; }

	void Update( float dt );

	void RenderHUD();

	/// returns currently selected aircraft
	std::shared_ptr<CGI_Aircraft> GetAircraft() { return m_pCurrentAircraft; }

	/// sets an aircraft for the player
	/// must be chosen from the player's item list
	/// \param pAircraft pointer to an aircraft item selected from m_vecpItem (borrowed reference)
	void SetAircraft( std::shared_ptr<CGI_Aircraft> pAircraft ) { m_pCurrentAircraft = pAircraft; }

	int GetMoneyLeft() const { return m_Money; }

	KeyBind& KeyBind() { return m_KeyBind; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	virtual const std::string& GetKeyString() const { return string("PlayerInfo"); }
	virtual const char* GetKeyString() const { return "PlayerInfo"; }

	const PlayTime& GetPlayTime() { return m_PlayTime; }

	friend class PlayerCustomer;
};


//============================= inline implementations =============================

template<class CItemType>
inline std::shared_ptr<CItemType> CSinglePlayerInfo::GetItemByName( const char *pcItemName )
{
	size_t i=0, num_items = m_vecpItem.size();
	for( i=0; i<num_items; i++ )
	{
		if( m_vecpItem[i]->GetName() == pcItemName )
			return std::dynamic_pointer_cast<CItemType,GameItem>(m_vecpItem[i]);
	}

	LOG_PRINT( "Cannot find the item: " + std::string(pcItemName) );

	return std::shared_ptr<CItemType>();
}


inline void CSinglePlayerInfo::AddActiveItem( GameItem *pItem )
{
	size_t i, num = m_vecpActiveItem.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpActiveItem[i] == pItem )
			return;	// already registered as an active item
	}

	m_vecpActiveItem.push_back( pItem );
}


inline void CSinglePlayerInfo::ReleaseActiveItem( GameItem *pItem )
{
	size_t i=0, num = m_vecpActiveItem.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpActiveItem[i] == pItem )
		{
			m_vecpActiveItem.erase( m_vecpActiveItem.begin() + i );
			return;
		}
	}
}


inline int CSinglePlayerInfo::SupplyItem( const std::string item_name, const int iSupplyQuantity )
{
	return SupplyItem( item_name.c_str(), iSupplyQuantity );
}


inline CSinglePlayerInfo& SinglePlayerInfo()
{
	return *CSinglePlayerInfo::Get();
}

} // namespace amorphous



#endif		/*  __PLAYERINFO_H__  */
