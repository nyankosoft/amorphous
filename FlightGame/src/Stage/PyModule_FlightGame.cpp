
#include "PyModule_FlightGame.h"
#include "FG_AIAircraftEntity.h"
#include "FG_AIAircraftEntityDesc.h"

#include "3DMath/Vector3.h"
#include "3DCommon/3DGameMath.h"
#include "Support/Vec3_StringAux.h"
#include "Item/GameItemDatabase.h"

#include "Stage/StaticGeometryFG.h"	// used by SetFogColor()
#include "Stage/CopyEntityDesc.h"
#include "Stage/Stage.h"
#include "Stage/PyModule_Stage.h"

using namespace std;

/*
static CStage *gs_pTargetStage = NULL;

void SetStageForStageScriptCallback( CStage* pStage )
{
	gs_pTargetStage = pStage;
}

CStage *GetStageForScriptCallback()
{
	return gs_pTargetStage;
}
*/


/*
static CCopyEntity *CreateAircraft( CFG_AIAircraftEntityDesc &desc,
								   float heading,
								   float pitch,
								   float roll )
{
}
*/


//======================================================================
// stage
//======================================================================

PyObject* CreateNamedAircraft( PyObject* self, PyObject* args )
{
	CFG_AIAircraftEntityDesc desc;
//	Vector3& vel = desc.vVelocity;
//	Vector3& pos = desc.vPosition;
	Vector3 pos, vel = Vector3(0,0,0);
	char *aircraft_entity_name;
	char *aircraft_item_name;
	char *base_name;
	float heading = 0, pitch = 0, roll = 0;
	int result = PyArg_ParseTuple( args, "sssfff|fff", &aircraft_entity_name, &base_name, &aircraft_item_name,
		                                             &pos.x, &pos.y, &pos.z,
													 &heading, &pitch, &roll,
		                                             &vel.x, &vel.y, &vel.z );

	desc.SetWorldPosition( pos );

	// TODO: support pitch & roll
	desc.SetWorldOrient( Matrix33RotationY(heading) );

	CBaseEntityHandle base_entity_handle;
	base_entity_handle.SetBaseEntityName( base_name );

	desc.pBaseEntityHandle = &base_entity_handle;

	CStage *pStage = GetStageForScriptCallback();
	if( pStage )
	{
		CCopyEntity *pEntity = pStage->CreateEntity( desc );

/*		CFG_AIAircraftEntity *pAircraftEntity = dynamic_cast<CFG_AIAircraftEntity *>(pEntity);
		if( pAircraftEntity )
		{
		}
		else
			LOG_PRINT_ERROR( "" );*/
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* CreateSquadronAircraft( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );
	return Py_None;
}

/*
static inline CCopyEntity *CreateNamedEntity( const char *entity_name,
										      const char *base_name,
											  const Vector3& pos,
											  const Vector3& dir,
											  const Vector3& vel )
{
	if( !gs_pTargetStage )
		return NULL;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.vPosition  = pos;

	Matrix33 matOrient = CreateOrientFromFwdDir( dir );
	desc.vRight = matOrient.GetColumn(0);
	desc.vUp    = matOrient.GetColumn(1);
	desc.vDirection = dir;

	desc.vVelocity = vel;
	desc.fSpeed = Vec3Length(vel);

//	desc.vVelocity = desc.vDirection * speed;
//	desc.fSpeed = speed;


	return gs_pTargetStage->CreateEntity( desc );
}


static inline CCopyEntity *CreateNamedEntityHrz( const char *entity_name,
										         const char *base_name,
											     Vector3& pos,
											     float ang_h,
											     float speed )
{
	if( !gs_pTargetStage )
		return NULL;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.vPosition = pos;
	desc.vDirection = Matrix33RotationY( deg_to_rad(ang_h) ) * Vector3(0,0,1);
	desc.vUp = Vector3(0,1,0);
	desc.vRight = Vec3Cross( Vector3(0,1,0), desc.vDirection );
	desc.vVelocity = desc.vDirection * speed;
	desc.fSpeed = speed;


	return gs_pTargetStage->CreateEntity( desc );
}


PyObject* CreateNamedEntityHrz( PyObject* self, PyObject* args )
{
	char *entity_name;
	char *base_name;
	Vector3 pos;
	float ang_h = 0.0f;
	float speed = 0.0f;
	int result = PyArg_ParseTuple( args, "ssfff|ff", &entity_name, &base_name,
		                                             &pos.x, &pos.y, &pos.z,
		                                             &ang_h, &speed );

	CreateNamedEntityHrz( entity_name, base_name, pos, ang_h, speed );

    Py_INCREF( Py_None );
	return Py_None;
}*/

/*
/
  create entity from pos, dir, vel,
  assuming that the entity is not banking (no rotation along dir)
  \param dir forward direction
 /
static PyObject* CreateAircraft( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *base_name;
	Vector3 pos;
	Vector3 dir = Vector3(0,0,1);
	Vector3 vel = Vector3(0,0,0);
	int result = PyArg_ParseTuple( args, "sfff|ffffff", &base_name,
		                                             &pos.x, &pos.y, &pos.z,
		                                             &dir.x, &dir.y, &dir.z,
		                                             &vel.x, &vel.y, &vel.z );

	CreateNamedEntity( "", base_name, pos, Vec3GetNormalized(dir), vel );

    Py_INCREF( Py_None );
	return Py_None;
}
*/


PyMethodDef g_PyModuleFlightGameMethod[] =
{
//	{ "CreateAircraft",			CreateAircraft,			METH_VARARGS, "creates an entity at a position with a direction & a velocity" },
	{ "CreateNamedAircraft",	CreateNamedAircraft,	METH_VARARGS, "creates an entity at a given position" },
//	{ "CreateEntityAt",			CreateEntityAt,			METH_VARARGS, "creates an entity at a given position" },
//	{ "CreateEntityHrz",		CreateEntityHrz,		METH_VARARGS, "creates an entity at a given position" },
	{NULL, NULL}
};


