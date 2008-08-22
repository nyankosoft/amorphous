#include "PyModule_Player.h"
#include "PlayerInfo.h"
#include "BE_Player.h"
#include "Item/WeaponSystem.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"

#include <string>


#define PlayerBaseEntity	(*SinglePlayerInfo().GetCurrentPlayerBaseEntity())
#define PlayerEntity		(*SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetPlayerCopyEntity())


//======================================================================
// player status interfaces for python scripts
//======================================================================

PyObject* GetPositionY( PyObject* self, PyObject* args )
{
	return Py_BuildValue( "f", PlayerEntity.Position().y );
}


PyObject* GetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos = PlayerEntity.Position();
	return Py_BuildValue( "f f f", pos.x, pos.y, pos.z );
}


PyObject* GetPose( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* GetVelocity( PyObject* self, PyObject* args )
{
	Vector3 pos = PlayerEntity.Velocity();
    return Py_BuildValue( "f f f", pos.x, pos.y, pos.z );
}


PyObject* GetSpeed( PyObject* self, PyObject* args )
{
	return Py_BuildValue( "f", PlayerEntity.GetSpeed() );
}


PyObject* HasItem( PyObject* self, PyObject* args )
{
//	return Py_BuildValue( "b", SinglePlayerInfo().HasItem() );
 
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* AddLife( PyObject* self, PyObject* args )
{
	float amount;
    PyArg_ParseTuple( args, "f", &amount );

//	PlayerEntity.AddLife( amount );
	PlayerEntity.fLife += amount;

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SupplyItem( PyObject* self, PyObject* args )
{
	char *item_name;
	int quantity;
    PyArg_ParseTuple( args, "si", &item_name, &quantity );

//	bool supplied = SinglePlayerInfo().SupplyItem( item_name, quantity );
	int supplied_quantity = SinglePlayerInfo().SupplyItem( item_name, quantity );

	bool supplied = 0 < supplied_quantity ? true : false;

	return Py_BuildValue( "b", supplied );
}


PyObject* MountWeapon( PyObject* self, PyObject* args )
{
	int weapon_slot_index;
	char *weapon_name;
    PyArg_ParseTuple( args, "is", &weapon_slot_index, &weapon_name );

	if( weapon_slot_index < 0 || CWeaponSystem::NUM_WEAPONSLOTS <= weapon_slot_index )
		return Py_BuildValue( "b", false );

	boost::shared_ptr<CGI_Weapon> pWeapon = SinglePlayerInfo().GetItemByName<CGI_Weapon>(weapon_name);
	if( !pWeapon || !(pWeapon->GetTypeFlag() & CGameItem::TYPE_WEAPON) )
		return Py_BuildValue( "b", false );

	SinglePlayerInfo().GetWeaponSystem()->GetWeaponSlot(weapon_slot_index).MountWeapon( pWeapon.get() );

	return Py_BuildValue( "b", true );
}


PyObject* LoadAmmo( PyObject* self, PyObject* args )
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

	return Py_None;
}


PyMethodDef g_PyModulePlayerMethod[] =
{
    { "GetPositionY",	GetPositionY,	METH_VARARGS, "Returns y(altimeter) component of the player's position" },
    { "GetPosition",	GetPosition,	METH_VARARGS, "Returns Position" },
    { "GetPose",		GetPose,		METH_VARARGS, "Returns Pose" },
    { "GetVelocity",	GetVelocity,	METH_VARARGS, "Returns Velocity" },
    { "GetSpeed",		GetSpeed,		METH_VARARGS, "Returns speed" },
    { "HasItem",		HasItem,		METH_VARARGS, "Returns true if the player has the specified item" },
    { "AddLife",		AddLife,		METH_VARARGS, "increase the player's life" },
    { "SupplyItem",		SupplyItem,		METH_VARARGS, "gives the player a specified amount of an item" },
    { "MountWeapon",	MountWeapon,	METH_VARARGS, "sets an weapon to slot[n]" },
    { "LoadAmmo",		LoadAmmo,		METH_VARARGS, "loads ammo to slot[n]" },
    {NULL, NULL}
};