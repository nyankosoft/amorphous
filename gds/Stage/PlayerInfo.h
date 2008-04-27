
#ifndef  __PLAYERINFO_H__
#define  __PLAYERINFO_H__

#include "Task/GameTask.h"
#include "BE_Player.h"
#include "GameCommon/KeyBind.h"
#include "GameCommon/SaveDataManager.h"
#include "GameCommon/PlayTime.h"
#include "Support/memory_helpers.h"


//#include "Support/Serialization/Serialization.h"
//#include "Support/Serialization/ArchiveObjectFactory.h"
//using namespace GameLib1::Serialization;


// forward declerations
class CCopyEntity;
class CInputHandler_PlayerBase;
class HUD_PlayerBase;
class CGameItem;
class CFont;
class CWeaponSystem;

class CGI_Aircraft;


class CItemCategory
{
public:
	enum eCategory
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



#define PLAYERINFO ( CPlayerInfo::ms_SingletonInstance_ )


/// holds properties of the single player in the client application
/// - singleton
/// - holds the following items
///   - owned references of game items
///   - a borrowed reference of a copy entity that represents the player in the stage
///   - key binds
///   - other settings & properties of the single player
class CPlayerInfo : public CSaveDataComponent
{
private:

	/// holds a base entity currently used by the player in the stage.
	/// (borrowed reference)
	/// - set by a player entity in CBE_Player::InitCopyEntity()
	/// - detached in CBE_Player::OnEntityDestroyed()
	CBE_Player *m_pCurrentPlayerBaseEntity;

	CInputHandler_PlayerBase *m_pInputHandler;

	/// owned ref?
	HUD_PlayerBase *m_pHUD;

	CWeaponSystem *m_pWeaponSystem;

	/// used to change tasks
//	CGameTask::eGameTask m_TaskID;
	int m_TaskID;

	/// a list of items the player is carrying (owned reference)
	std::vector<CGameItem *> m_vecpItem;

	/// lists of items sorted by categories (borrowed reference)
	std::vector<CGameItem *> m_vecpCategoryItem[CItemCategory::NUM_CATEGORIES];

	/// borrowed ref?
	std::vector<CGameItem *> m_vecpActiveItem;

	/// borrowed ref?
	CGameItem *m_pFocusedItem;

	int m_Money;

	/// aircraft used by flight games (borrowed reference)
	/// a valid aircraft item has to be set before a stage is loaded.
	/// During the stage initialization, base entities are loaded and initialized.
	/// In that phase, one base entity, 'CBE_PlayerPseudoAircraft' access this variable
	/// and saves the borrowed reference, then use it as the player's aircraft in the stage
	CGI_Aircraft *m_pCurrentAircraft;

	CKeyBind m_KeyBind;

	CPlayTime m_PlayTime;

private:

	void AddItemToCategoryList( CGameItem *pItem );

protected:

	CPlayerInfo();		// singleton

public:

	static CPlayerInfo ms_SingletonInstance_;	// single instance of 'CPlayerInfo'

	~CPlayerInfo();

	void Release();

	inline CBE_Player *GetCurrentPlayerBaseEntity() { return m_pCurrentPlayerBaseEntity; }

	CCopyEntity *GetCurrentPlayerEntity();

	inline void SetPlayerBaseEntity(CBE_Player *pPlayer) { m_pCurrentPlayerBaseEntity = pPlayer; }

	inline CWeaponSystem *GetWeaponSystem() { return m_pWeaponSystem; }

//	inline CGameTask::eGameTask GetTaskRequest() { return m_TaskID; }
	inline int GetTaskRequest() { return m_TaskID; }

	// request a task change
//	inline void RequestTaskChange( CGameTask::eGameTask task_id ) { m_TaskID = task_id; }
	inline void RequestTaskChange( int task_id ) { m_TaskID = task_id; }

	void SetInputHandlerForPlayerShip();
//	inline CInputHandler_PlayerBase *GetInputHandlerForPlayerShip() { return m_pInputHandler; }
	inline CInputHandler_PlayerBase *GetInputHandler() { return m_pInputHandler; }

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
	int SupplyItem( CGameItem* pItem );

	const std::vector<CGameItem *>& GetItemList() { return m_vecpItem; }

	CGameItem *GetItemByName( const char *pcItemName );

	inline std::vector<CGameItem *>& GetCategoryItemList( int category ) { return m_vecpCategoryItem[category]; }

	inline void AddActiveItem( CGameItem *pItem );
	inline void ReleaseActiveItem( CGameItem *pItem );


	/// select an item to operate
	inline void SetItemFocus( CGameItem *pItem ) { m_pFocusedItem = pItem; }

	/// release focus from the item currently in operation
	inline void ReleaseItemFocus() { m_pFocusedItem = NULL; }

	inline CGameItem *GetFocusedItem() { return m_pFocusedItem; }  

	inline bool HasKeycode( const char *pcKeyCode) { return false; }

	void Update( float dt );

	void RenderHUD();

	inline HUD_PlayerBase *GetHUD() { return m_pHUD; }

	/// returns currently selected aircraft
	CGI_Aircraft *GetAircraft() { return m_pCurrentAircraft; }

	/// sets an aircraft for the player
	/// must be chosen from the player's item list
	/// \param pAircraft pointer to an aircraft item selected from m_vecpItem (borrowed reference)
	void SetAircraft( CGI_Aircraft* pAircraft ) { m_pCurrentAircraft = pAircraft; }

	int GetMoneyLeft() const { return m_Money; }

	CKeyBind& KeyBind() { return m_KeyBind; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	virtual const std::string& GetKeyString() const { return string("PlayerInfo"); }
	virtual const char* GetKeyString() const { return "PlayerInfo"; }

	const CPlayTime& GetPlayTime() { return m_PlayTime; }

	friend class CPlayerCustomer;
};


//============================= inline implementations =============================


inline void CPlayerInfo::AddActiveItem( CGameItem *pItem )
{
	size_t i, num = m_vecpActiveItem.size();
	for( i=0; i<num; i++ )
	{
		if( m_vecpActiveItem[i] == pItem )
			return;	// already registered as an active item
	}

	m_vecpActiveItem.push_back( pItem );
}


inline void CPlayerInfo::ReleaseActiveItem( CGameItem *pItem )
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


inline int CPlayerInfo::SupplyItem( const std::string item_name, const int iSupplyQuantity )
{
	return SupplyItem( item_name.c_str(), iSupplyQuantity );
}


#endif		/*  __PLAYERINFO_H__  */
