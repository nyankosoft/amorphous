#include "PyModule_Entity.hpp"
#include "EntityMotionPathRequest.hpp"
#include "GameMessage.hpp"
#include "Stage.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "BE_StaticParticleSet.hpp"
#include "BaseEntity_Draw.hpp"

#include "Script/PythonScriptManager.hpp"
#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"
#include "Support/fixed_string.hpp"


namespace amorphous
{

using namespace std;


static CStage *gs_pTargetStage = NULL;

double g_fTimeOffset = 0.0f;

void amorphous::py::entity::SetStageForEntityScriptCallback( CStage* pStage )
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


using namespace py::entity;



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

	GameMessage msg( GM_SET_MOTION_PATH );
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

    g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose( time + (float)g_fTimeOffset, pose ) );

	return Py_None;
}


PyObject* SetCurrentPoseAsKeyframe( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	if( !g_EntityMotionPathRequest.pTargetEntity )
		return Py_None;

	float time = 0;

	int result = PyArg_ParseTuple( args, "|f", &time );

	const Matrix34 pose = g_EntityMotionPathRequest.pTargetEntity->GetWorldPose();

    g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose( time + (float)g_fTimeOffset, pose ) );

	return Py_None;
}


/// Calculates and adds a keyframe from the specified speed and delta time.
/// - The functions calculates the velocity from the current pose of the entity
///   and the specified speed.
/// - Then, it sets the pose as the keyframe by the following calculation
/// - time of the keyframe is calculated by adding the specified delta time
///   to the last keyframe.
/// - The current orientation of the entity is set to the keyframe. 
PyObject* CalculateNextKeyframeFromDeltaTimeAndSpeed( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	if( !g_EntityMotionPathRequest.pTargetEntity )
		return Py_None;

	float delta_time, speed;

	int result = PyArg_ParseTuple( args, "ff", &delta_time, &speed );

	const Matrix34 current_pose = g_EntityMotionPathRequest.pTargetEntity->GetWorldPose();

	Vector3 vVelocity = current_pose.matOrient.GetColumn(2) * speed;
	Vector3 vDestPos = current_pose.vPosition + vVelocity * delta_time;

	// pose
	const Matrix34 pose = Matrix34( vDestPos, current_pose.matOrient );

	// time
	float base_time = 0 < g_EntityMotionPathRequest.vecKeyPose.size() ? g_EntityMotionPathRequest.vecKeyPose.back().time : 0;
	float time = base_time + delta_time;

    g_EntityMotionPathRequest.vecKeyPose.push_back( KeyPose( time + (float)g_fTimeOffset, pose ) );

	return Py_None;
}


PyObject* amorphous::py::entity::SetTimeOffset( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	double time = -1000000.0f;
	int result = PyArg_ParseTuple( args, "|d", &time );

	if( time < 0 )
	{
		// Consider this no-argument call
		// - Set the current time in the stage as the offset
		if( gs_pTargetStage )
			g_fTimeOffset = gs_pTargetStage->GetElapsedTime();
	}
	else
		g_fTimeOffset = time;

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

	BaseEntityHandle baseentity_handle;
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
	BaseEntity  *pBaseEntity = NULL;
	int result = PyArg_ParseTuple( args, "s", &base_entity_name );

	pBaseEntity = gs_pTargetStage->GetEntitySet()->FindBaseEntity(base_entity_name);

	if( pBaseEntity && pBaseEntity->GetArchiveObjectID() == BaseEntity::BE_STATICPARTICLESET )
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
PyObject* amorphous::py::entity::SetTarget( PyObject* self, PyObject* args )
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
PyObject* amorphous::py::entity::ReleaseTarget( PyObject* self, PyObject* args )
{
	char *entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity *gs_pTargetEntity = GetEntityByName(entity_name);

//	MsgBoxFmt( "starting to set motion path for entity: %s", entity_name );

    Py_INCREF( Py_None );
	return Py_None;
}
*/


PyObject* amorphous::py::entity::SetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "fff", &pos.x, &pos.y, &pos.z );

	if( !IsValidEntity(gs_pTargetEntity) )
	{
		Py_INCREF( Py_None );
		return Py_None;
	}

	gs_pTargetEntity->SetWorldPosition( pos );

    Py_INCREF( Py_None );
	return Py_None;
}


/// \param entity name
PyObject* SetEntityGroup( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	const char *entity_group_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_group_name );

	if( !IsValidEntity(gs_pTargetEntity) )
		return Py_None;

	if( !gs_pTargetStage )
		return Py_None;

	if( !entity_group_name || string(entity_group_name).length() == 0 )
		return Py_None;

	int entity_group_id
		= gs_pTargetStage->GetEntitySet()->GetEntityGroupFromName( entity_group_name );

	if( entity_group_id == ENTITY_GROUP_INVALID_ID )
		return Py_None;

	gs_pTargetEntity->GroupIndex = entity_group_id;

	return Py_None;
}


PyObject* SetMeshMaterialParamValue( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	const char *property_name = NULL;
	float value = 0.0f;
	int subset_index = 0;
	int result = PyArg_ParseTuple( args, "isf", &subset_index, &property_name, &value );

	if( !IsValidEntity(gs_pTargetEntity) )
		return Py_None;

	boost::shared_ptr<BasicMesh> pMesh = gs_pTargetEntity->m_MeshHandle.GetMesh();
	if( !pMesh )
		return Py_None;

	if( subset_index < 0 || (int)pMesh->GetNumMaterials() <= subset_index )
		return Py_None;

	CBasicMaterialParams& mat = pMesh->Material(subset_index).m_Mat;

	fixed_string<16> name(property_name);
	if(      name == "luminosity" )  mat.fLuminosity  = value;
	else if( name == "specularity" ) mat.fSpecularity = value;
	else if( name == "glossiness" )  mat.fGlossiness  = value;
	else if( name == "reflection" )
	{
		const float prev_reflection = mat.fReflection;
		mat.fReflection = value; // mat.fReflection needs be updated before InitEntityGraphics() is called.
		if( 0.001 < abs(value - prev_reflection) )
		{
			// Initialize it again for the following reasons.
			// 1. reflective/non-reflective meshes use different shaders.
			// 2. need to register/unregister as the planar reflector or as a env map target
			//    if parameter is changed from 0 to x(>0)/x(>0) to 0.
			gs_pTargetEntity->pBaseEntity->InitEntityGraphics( *gs_pTargetEntity );
		}
	}

	return Py_None;
}


static PyMethodDef sg_PyModuleEntityMethod[] =
{
	{ "StartPath",				  EntityStartPath,                 METH_VARARGS, "notify the start of motion path set routine" },
	{ "EndPath",				  EntityEndPath,                   METH_VARARGS, "notify the end of motion path set routine" },
	{ "AddPath",				  EntityAddPath,                   METH_VARARGS, "add a way point (keyframe) for motion path" },
	{ "SetCurrentPoseAsKeyframe", SetCurrentPoseAsKeyframe,        METH_VARARGS, "" },
	{ "CalculateNextKeyframeFromDeltaTimeAndSpeed", CalculateNextKeyframeFromDeltaTimeAndSpeed,        METH_VARARGS, "" },
	{ "SetTimeOffset",			  amorphous::py::entity::SetTimeOffset,  METH_VARARGS, "set the offset of the time argument of AddPath()" },
//	{ "GetVelocity",			  GetVelocity,                     METH_VARARGS, "" },
//	{ "GetSpeed",				  GetSpeed,                        METH_VARARGS, "" },
	{ "CreateStaticParticleSet",  CreateStaticParticleSet,         METH_VARARGS, "" },
	{ "CommitStaticParticleSet",  CommitStaticParticleSet,         METH_VARARGS, "" },
	{ "SetTarget",                amorphous::py::entity::SetTarget,      METH_VARARGS, "find and lock the target entity for the subsequent calls" },
	{ "SetPosition",              amorphous::py::entity::SetPosition,    METH_VARARGS, "" },
	{ "SetEntityGroup",           SetEntityGroup,                  METH_VARARGS, "Sets an entity group to the entity currently selected as the target" },
	{ "SetMeshMaterialParamValue",SetMeshMaterialParamValue,       METH_VARARGS, "" },
	{NULL, NULL}
};


void py::entity::RegisterPythonModule_Entity( PythonScriptManager& mgr )
{
	mgr.AddModule( "Entity", sg_PyModuleEntityMethod );
}


} // namespace amorphous
