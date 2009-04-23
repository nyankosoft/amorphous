#include "PyModule_Entity.hpp"
#include "EntityMotionPathRequest.hpp"
#include "GameMessage.hpp"
#include "Stage.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "BE_StaticParticleSet.hpp"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"

using namespace std;


static CStage *gs_pTargetStage = NULL;

void gsf::py::entity::SetStageForEntityScriptCallback( CStage* pStage )
{
	gs_pTargetStage = pStage;
}


/*
struct NamedEntityCache
{
	string name;
	CCopyEntity *pEntity;
};

#define NAMED_ENTITY_CACHE_SIZE	4
static NamedEntityCache g_NamedEntityCache[NAMED_ENTITY_CACHE_SIZE];
*/


static CCopyEntity *GetEntityByName( const char* entity_name )
{
	if( gs_pTargetStage )
        return gs_pTargetStage->GetEntitySet()->GetEntityByName(entity_name);
	else
        return NULL;
}


using namespace gsf::py::entity;



static EntityMotionPathRequest g_EntityMotionPathRequest;


// TODO: support MT. this scheme will not allow
// paths for multiple entities to be set at the same time
// \param entity name
// \param (optional) path tracking mode
PyObject* EntityStartPath( PyObject* self, PyObject* args )
{
	char *entity_name = NULL;
	int path_track_mode = EntityMotionPathRequest::SET_POSITION;
	int result = PyArg_ParseTuple( args, "s|i", &entity_name, &path_track_mode );

	g_EntityMotionPathRequest.Clear();
	g_EntityMotionPathRequest.pTargetEntity = GetEntityByName(entity_name);
	g_EntityMotionPathRequest.mode = path_track_mode;

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* EntityEndPath( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	SGameMessage msg( GM_SET_MOTION_PATH );
	msg.pUserData = &g_EntityMotionPathRequest;

	SendGameMessageTo( msg, g_EntityMotionPathRequest.pTargetEntity );

	return Py_None;
}


PyObject* EntityAddPath( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	if( !g_EntityMotionPathRequest.pTargetEntity )
		return Py_None;

	float time;
	Vector3 pos;
	float heading = 0, pitch = 0, bank = 0;	// rotation angles measured in degree

	int result = PyArg_ParseTuple( args, "ffff|fff", &time, &pos.x, &pos.y, &pos.z, &heading, &pitch, &bank );

	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient
		= Matrix33RotationZ( deg_to_rad(bank) )
		* Matrix33RotationX( deg_to_rad(pitch) )
		* Matrix33RotationY( deg_to_rad(heading) );

    g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose(time,pose) );

	return Py_None;
}


PyObject* CreateStaticParticleSet( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	char *entity_name;
	char *base_entity_name;
	Vector3 v0, v1;

	int result = PyArg_ParseTuple( args, "ssffffff", &entity_name, &base_entity_name,
		                                             &v0.x, &v0.y, &v0.z,
		                                             &v1.x, &v1.y, &v1.z );
	AABB3 aabb = AABB3( v0, v1 );

//	PERIODICAL( 8, MsgBoxFmt( "creating clouds '%s' in '%s'",
//		base_entity_name, to_string(aabb).c_str() ) )

	if( !gs_pTargetStage )
	{
		return Py_None;
	}

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_entity_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.WorldPose.vPosition = aabb.GetCenterPosition();
	desc.v1 = aabb.vMin;
	desc.v2 = aabb.vMax;

	CCopyEntity* pEntity = gs_pTargetStage->CreateEntity( desc );

	if( !pEntity )
		LOG_PRINT_WARNING( " Cannot create static clouds" );

	return Py_None;
}


PyObject* CommitStaticParticleSet( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	if( !gs_pTargetStage )
	{
		return Py_None;
	}

	char *base_entity_name;
	CBaseEntity  *pBaseEntity = NULL;
	int result = PyArg_ParseTuple( args, "s", &base_entity_name );

	pBaseEntity = gs_pTargetStage->GetEntitySet()->FindBaseEntity(base_entity_name);

	if( pBaseEntity && pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_STATICPARTICLESET )
	{
		CBE_StaticParticleSet *p = dynamic_cast<CBE_StaticParticleSet *>(pBaseEntity);
		p->CommitStaticParticles();	// create vertex buffers and copy particles
//		LOG_PRINT_WARNING( "copied particle set to vertex buffer" );
		return Py_None;	//true;
	}
	else
	{
		LOG_PRINT_WARNING( "cannot find particle set" );
		return Py_None;	//false;
	}
}


static CCopyEntity *gs_pTargetEntity = NULL;

/// \param entity name
PyObject* gsf::py::entity::SetTarget( PyObject* self, PyObject* args )
{
	char *entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity *pTargetEntity = GetEntityByName(entity_name);

	if( !IsValidEntity(pTargetEntity) )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	gs_pTargetEntity = pTargetEntity;

    Py_INCREF( Py_None );
	return Py_None;
}


/*
PyObject* gsf::py::entity::ReleaseTarget( PyObject* self, PyObject* args )
{
	char *entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity *gs_pTargetEntity = GetEntityByName(entity_name);

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

    Py_INCREF( Py_None );
	return Py_None;
}
*/


/// \param entity name
PyObject* gsf::py::entity::SetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "fff", &pos.x, &pos.y, &pos.z );

	if( !IsValidEntity(gs_pTargetEntity) )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	gs_pTargetEntity->SetPosition( pos );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef gsf::py::entity::g_PyModuleEntityMethod[] =
{
	{ "StartPath",				 EntityStartPath,              METH_VARARGS, "notify the start of motion path set routine" },
	{ "EndPath",				 EntityEndPath,                METH_VARARGS, "notify the end of motion path set routine" },
	{ "AddPath",				 EntityAddPath,                METH_VARARGS, "add a way point for motion path" },
//	{ "GetVelocity",			 GetVelocity,                  METH_VARARGS, "" },
//	{ "GetSpeed",				 GetSpeed,                     METH_VARARGS, "" },
	{ "CreateStaticParticleSet", CreateStaticParticleSet,	   METH_VARARGS, "" },
	{ "CommitStaticParticleSet", CommitStaticParticleSet,	   METH_VARARGS, "" },
	{ "SetTarget",               gsf::py::entity::SetTarget,   METH_VARARGS, "find and lock the target entity for the subsequent calls" },
	{ "SetPosition",             gsf::py::entity::SetPosition, METH_VARARGS, "" },
	{NULL, NULL}
};
