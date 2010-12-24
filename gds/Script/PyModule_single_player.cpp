#include "PyModule_single_player.hpp"
#include "Stage/PlayerInfo.hpp"
#include "Stage/BE_Player.hpp"
#include "Item/WeaponSystem.hpp"
#include "3DMath/Matrix34.hpp"
#include <boost/python.hpp>

using namespace boost;


static CCopyEntity *GetPlayerEntity()
{
	if( SinglePlayerInfo().GetCurrentPlayerBaseEntity() )
		return SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity();
	else
		return NULL;
}

/*
static Vector3 GetPlayerPosition()
{
	CCopyEntity *pPlayer = GetPlayerEntity();
	if( pPlayer )
		return pPlayer->Position();
	else
		return Vector3(0,0,0);
}
*/

//======================================================================
// player status interfaces for python scripts
//======================================================================

bool IsInStage()
{
	return ( GetPlayerEntity() != NULL );
}


Vector3 GetPosition()
{
	CCopyEntity *pEntity = GetPlayerEntity();
	return (pEntity ? pEntity->GetWorldPose().vPosition : Vector3(0,0,0));
}


Matrix34 GetPose()
{
	CCopyEntity *pEntity = GetPlayerEntity();
	return pEntity ? pEntity->GetWorldPose() : Matrix34Identity();
}


Vector3 GetVelocity()
{
	CCopyEntity *pEntity = GetPlayerEntity();
    return pEntity ? pEntity->Velocity() : Vector3(0,0,0);
}


float GetSpeed()
{
	CCopyEntity *pEntity = GetPlayerEntity();
	return pEntity ? pEntity->GetSpeed() : 0.0f;
}


void AddLife( float amount )
{
	CCopyEntity *pEntity = GetPlayerEntity();

	if( pEntity )
		pEntity->fLife += amount;
}


bool HasItem( const std::string& item_name )
{
	shared_ptr<CGameItem> pItem = SinglePlayerInfo().GetItemByName<CGameItem>( item_name.c_str() );

	return ( pItem ? true : false );
}


/// \return the number of items actually supplied to the player
int SupplyItem( const std::string& item_name, int quantity )
{
//	const char *item_name;

//	bool supplied = SinglePlayerInfo().SupplyItem( item_name, quantity );

	int supplied_quantity = SinglePlayerInfo().SupplyItem( item_name, quantity );

	return supplied_quantity;
}


/*
void MountWeapon( int weapon_slot_index, const std::string& weapon_name )
{
	if( weapon_slot_index < 0 || CWeaponSystem::NUM_WEAPONSLOTS <= weapon_slot_index )
		return;

	boost::shared_ptr<CGI_Weapon> pWeapon = SinglePlayerInfo().GetItemByName<CGI_Weapon>(weapon_name);
	if( !pWeapon || !(pWeapon->GetTypeFlag() & CGameItem::TYPE_WEAPON) )
		return;

	SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot(weapon_slot_index).MountWeapon( pWeapon.get() );

	return;
}


void LoadAmmo()
{
	int weapon_slot_index;
	char *ammo_name;
    PyArg_ParseTuple( args, "is", &weapon_slot_index, &ammo_name );

	if( weapon_slot_index < 0 || CWeaponSystem::NUM_WEAPONSLOTS <= weapon_slot_index )
		return Py_BuildValue( "b", false );

	boost::shared_ptr<CGI_Ammunition> pAmmo = SinglePlayerInfo().GetItemByName<CGI_Ammunition>(ammo_name);
	if( !pAmmo || !(pAmmo->GetTypeFlag() & CGameItem::TYPE_AMMO) )
		return Py_BuildValue( "b", false );

	bool loaded = SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot(weapon_slot_index).Load( pAmmo.get() );

	return Py_BuildValue( "b", loaded );
}
*/


BOOST_PYTHON_MODULE(single_player)
{
	using namespace boost::python;

	def( "GetPosition",         GetPosition );
	def( "GetPose",             GetPose );
	def( "GetVelocity",         GetVelocity );
	def( "GetSpeed",            GetSpeed);
	def( "AddLife",             AddLife );
	def( "HasItem",             HasItem );
	def( "SupplyItem",          SupplyItem ,   ( python::arg("item_name"), python::arg("quantity") = 1 ) );
}


void RegisterPythonModule_single_player()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("single_player", initsingle_player) == -1)
	{
		const char *msg = "Failed to add 'single_player' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}
