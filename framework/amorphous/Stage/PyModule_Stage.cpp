#include "PyModule_Stage.hpp"

#include "3DMath/3DGameMath.hpp"
#include "Physics/ActorDesc.hpp"

#include "Stage.hpp"
#include "CopyEntityDesc.hpp"
#include "StaticGeometryBase.hpp"	// used by SetFogColor()
#include "BE_Skybox.hpp"	// used by SetFogColor()
#include "StageUtility.hpp"
#include "../Script/convert_python_to_x.hpp"
#include "../Script/PythonScriptManager.hpp"


namespace amorphous
{

using namespace std;

using namespace physics;


static CStage *gs_pTargetStage = NULL;

void SetStageForStageScriptCallback( CStage* pStage )
{
	gs_pTargetStage = pStage;
}


CStage *GetStageForScriptCallback()
{
	return gs_pTargetStage;
}


//======================================================================
// stage
//======================================================================

CCopyEntity *CreateNamedEntity( const char *entity_name,
								const char *base_name,
								const Matrix34& pose,
								const Vector3& vel,
								CActorDesc *pPhysActorDesc )
{
	if( !gs_pTargetStage )
		return NULL;

	BaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.WorldPose = pose;
	desc.pPhysActorDesc = pPhysActorDesc;
//	desc.SetWorldOrient( CreateOrientFromFwdDir( dir ) );

	desc.vVelocity = vel;
	desc.fSpeed = Vec3Length(vel);

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

	BaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.SetWorldPosition( pos );
	desc.SetWorldOrient( Matrix33RotationY( deg_to_rad(ang_h) ) );

	desc.vVelocity = desc.WorldPose.matOrient.GetColumn(2) * speed;
	desc.fSpeed = speed;


	return gs_pTargetStage->CreateEntity( desc );
}


PyObject* CreateEntityAt( PyObject* self, PyObject* args )
{
	char *base_name;
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "(sfff)", &base_name, &pos.x, &pos.y, &pos.z );

	CreateNamedEntityHrz( "", base_name, pos, 0.0f, 0.0f );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* CreateEntityHrz( PyObject* self, PyObject* args )
{
	char *base_name;
	Vector3 pos;
	float ang_h = 0.0f;
	float speed = 0.0f;
	int result = PyArg_ParseTuple( args, "sfff|ff", &base_name, &pos.x, &pos.y, &pos.z,
		                                            &ang_h, &speed );

	CreateNamedEntityHrz( "", base_name, pos, ang_h, speed );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* CreateStaticEntityFromMesh( PyObject* self, PyObject* args, PyObject *keywords )
{
	Py_INCREF( Py_None );

	char *entity_name   = NULL;
	char *mesh_pathname = NULL;
	Vector3 pos( Vector3(0,0,0) );
	float heading = 0, pitch = 0, bank = 0;

	static char *kw_list[] = { "mesh", "name", "position", "heading", "pitch", "bank" , NULL };

	int result = PyArg_ParseTupleAndKeywords( args, keywords, "s|sO&fff", kw_list,
		&mesh_pathname, &entity_name,
		convert_python_to_cpp_Vector3, &pos,
		&heading, &pitch, &bank
		);

	Matrix34 pose( pos, Matrix33RotationHPR_deg(heading,pitch,bank) );

	if( !gs_pTargetStage )
		return Py_None;

	std::shared_ptr<CStage> pStage = gs_pTargetStage->GetWeakPtr().lock();
	if( !pStage )
		return Py_None;

	StageMiscUtility util( pStage );
	EntityHandle<> entity = util.CreateStaticTriangleMeshFromMesh( mesh_pathname, mesh_pathname, pose, "default", entity_name );

	return Py_None;
}


PyObject* CreateEntity( PyObject* self, PyObject* args, PyObject *keywords )
{
	Py_INCREF( Py_None );

	char *base_entity_or_mesh_name = NULL;
	char *entity_name   = NULL;
	Vector3 pos( Vector3(0,0,0) );
	float heading = 0, pitch = 0, bank = 0;

	static char *kw_list[] = { "model", "name", "position", "heading", "pitch", "bank", NULL };

	int result = PyArg_ParseTupleAndKeywords( args, keywords, "s|sO&fff", kw_list,
		&base_entity_or_mesh_name, &entity_name,
		convert_python_to_cpp_Vector3, &pos,
		&heading, &pitch, &bank
		);

	string model_name = base_entity_or_mesh_name;

	Matrix34 pose( pos, Matrix33RotationHPR_deg(heading,pitch,bank) );

	if( !gs_pTargetStage )
		return Py_None;

	std::shared_ptr<CStage> pStage = gs_pTargetStage->GetWeakPtr().lock();
	if( !pStage )
		return Py_None;

//	StageMiscUtility util( pStage );
//	EntityHandle<> entity = util.CreateStaticTriangleMeshFromMesh( mesh_pathname, mesh_pathname, pose, "default", entity_name );

	return Py_None;
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
}


PyObject* CreateNamedEntity( PyObject* self, PyObject* args )
{
	char *entity_name;
	char *base_name;
	Vector3 pos, vel = Vector3(0,0,0);
	int result = PyArg_ParseTuple( args, "ssfff|fff", &entity_name, &base_name,
		                                             &pos.x, &pos.y, &pos.z,
		                                             &vel.x, &vel.y, &vel.z );

//	CreateNamedEntity( entity_name, base_name, pos, vel );

    Py_INCREF( Py_None );
	return Py_None;
}


/**
 * create entity from pos, dir, vel,
 * assuming that the entity is not banking (no rotation along dir)
 * \param dir forward direction
 */
PyObject* CreateEntity( PyObject* self, PyObject* args )
{
	char *base_name;
	Vector3 pos;
	Vector3 dir = Vector3(0,0,1);
	Vector3 vel = Vector3(0,0,0);
	int result = PyArg_ParseTuple( args, "sfff|ffffff", &base_name,
		                                             &pos.x, &pos.y, &pos.z,
		                                             &dir.x, &dir.y, &dir.z,
		                                             &vel.x, &vel.y, &vel.z );

	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient = CreateOrientFromFwdDir( Vec3GetNormalized(dir) );
	CreateNamedEntity( "", base_name, pose, vel );

    Py_INCREF( Py_None );
	return Py_None;
}




PyObject* LoadStaticGeometry( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	char *filename;

	int result = PyArg_ParseTuple( args, "s", &filename );

	LOG_PRINTF_VERBOSE(( "file: %s", filename ? filename : "<None>" ));

	gs_pTargetStage->LoadStaticGeometryFromFile( filename );

	return Py_None;
}



PyObject* LoadSkybox( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	char *base_entity_name;
	char *texture_filename;

	int result = PyArg_ParseTuple( args, "ss", &base_entity_name, &texture_filename );

	CCopyEntity* pSkyboxEntity = CreateNamedEntityHrz( "skybox", base_entity_name, Vector3(0,0,0), 0, 0 );

	if( !pSkyboxEntity
	 || pSkyboxEntity->pBaseEntity->GetArchiveObjectID() != BaseEntity::BE_SKYBOX )
	{
		return Py_None;
	}

	(dynamic_cast<CBE_Skybox *>(pSkyboxEntity->pBaseEntity))->LoadSkyboxTexture( texture_filename );

	return Py_None;
}


static inline void SetAmbientColor( const SFloatRGBAColor& ambient_color )
{
	if( !gs_pTargetStage )
		return;

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetAmbientColor( ambient_color );
}


PyObject* SetAmbientColor( PyObject* self, PyObject* args )
{
	SFloatRGBAColor ambient_color;
	int result = PyArg_ParseTuple( args, "fff", &ambient_color.red, &ambient_color.green, &ambient_color.blue );

	SetAmbientColor( ambient_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetAmbientColorC32( PyObject* self, PyObject* args )
{
	unsigned long src_color;
	int result = PyArg_ParseTuple( args, "k", &src_color );

	SFloatRGBAColor ambient_color;
	ambient_color.SetARGB32( src_color );

	SetAmbientColor( ambient_color );

    Py_INCREF( Py_None );
	return Py_None;
}


static inline void SetFogColor( const SFloatRGBAColor& fog_color )
{
	if( !gs_pTargetStage )
		return;

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogColor( fog_color );
}


PyObject* SetFogColor( PyObject* self, PyObject* args )
{
	SFloatRGBAColor fog_color;
	int result = PyArg_ParseTuple( args, "fff", &fog_color.red, &fog_color.green, &fog_color.blue );

	SetFogColor( fog_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetFogColorC32( PyObject* self, PyObject* args )
{
	unsigned long src_color;
	int result = PyArg_ParseTuple( args, "k", &src_color );

	SFloatRGBAColor fog_color;
	fog_color.SetARGB32( src_color );

	SetFogColor( fog_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetFogStartDist( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	float dist;
	int result = PyArg_ParseTuple( args, "f", &dist );

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogStartDist( dist );

	return Py_None;
}


PyObject* SetFogEndDist( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	float dist;
	int result = PyArg_ParseTuple( args, "f", &dist );

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogEndDist( dist );

	return Py_None;
}


/*
PyObject* LoadStaticGeometryScaled( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *filename;
	float scale_factor = 1.0f;

	int result = PyArg_ParseTuple( args, "sf", &filename, scale_factor );

	gs_pTargetStage->LoadStaticGeometryFromFile( filename, scale_factor );

    Py_INCREF( Py_None );
	return Py_None;
}
*/


PyObject* InitPlayer( PyObject* self, PyObject* args )
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

	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient = CreateOrientFromFwdDir( Vec3GetNormalized(dir) );
	CreateNamedEntity( "", base_name, pose, vel );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* InitPlayerHrz( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *base_name;
	Vector3 pos;
	float ang_h = 0.0f;
	float speed = 0.0f;
//	int result = PyArg_ParseTuple( args, "(sfff|f)", &base_name, &pos.x, &pos.y, &pos.z, &ang_h );	// error
	int result = PyArg_ParseTuple( args, "sfff|ff", &base_name, &pos.x, &pos.y, &pos.z,
		                                            &ang_h, &speed );

	CreateNamedEntityHrz( "", base_name, pos, ang_h, speed );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetCameraEntity( PyObject* self, PyObject* args )
{
    Py_INCREF( Py_None );

	if( !gs_pTargetStage )
		return Py_None;

	char *entity_name;

	int result = PyArg_ParseTuple( args, "s", &entity_name );

	EntityManager *pEntitySet = gs_pTargetStage->GetEntitySet();
	pEntitySet->SetCameraEntity( pEntitySet->GetEntityByName(entity_name) );

	return Py_None;
}


PyObject* GetElapsedTime( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

//	float time = (float)gs_pTargetStage->GetElapsedTime();
//	PyObject *obj = Py_BuildValue( "f", time );
	PyObject *obj = Py_BuildValue( "d", gs_pTargetStage->GetElapsedTime() );

	return obj;
}


PyObject* GetElapsedTimeMS( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	unsigned long time = gs_pTargetStage->GetElapsedTimeMS();
	PyObject *obj = Py_BuildValue( "k", gs_pTargetStage->GetElapsedTimeMS() );

	return obj;
}


PyObject* IsEntityAlive( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *entity_name;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	bool alive = gs_pTargetStage->GetEntitySet()->IsEntityAlive( entity_name );

	PyObject *obj = Py_BuildValue( "i", alive ? 1 : 0 );

	return obj;
}


PyObject* EntityDestroyed( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *entity_name;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	bool destroyed = gs_pTargetStage->GetEntitySet()->EntityDestroyed( entity_name );

	PyObject *obj = Py_BuildValue( "i", destroyed ? 1 : 0 );

	return obj;
}


static PyMethodDef sg_PyModuleStageMethod[] =
{
	{ "CreateEntity",			CreateEntity,			METH_VARARGS, "creates an entity at a position with a direction & a velocity" },
	{ "CreateEntityAt",			CreateEntityAt,			METH_VARARGS, "creates an entity at a given position" },
	{ "CreateNamedEntityHrz",	CreateNamedEntityHrz,	METH_VARARGS, "creates an entity at a given position" },
	{ "CreateEntityHrz",		CreateEntityHrz,		METH_VARARGS, "creates an entity at a given position" },
	{ "CreateStaticEntityFromMesh",	(PyCFunction)CreateStaticEntityFromMesh,	METH_VARARGS | METH_KEYWORDS, "creates an entity at a given position" },

	{ "LoadStaticGeometry",		LoadStaticGeometry,		METH_VARARGS, "loads static geometry with a given filename" },
	{ "LoadSkybox",				LoadSkybox,				METH_VARARGS, "loads a skybox" },
	{ "SetAmbientColor",		SetAmbientColor,		METH_VARARGS, "set ambient color (only for static geometry)" },
	{ "SetAmbientColorC32",		SetAmbientColorC32,		METH_VARARGS, "set ambient color (only for static geometry)" },
	{ "SetFogColor",			SetFogColor,			METH_VARARGS, "set fog color for static geometry (only supported for CStaticGeometryBase::TYPE_FLIGHTGAME)" },
	{ "SetFogColorC32",			SetFogColorC32,			METH_VARARGS, "set fog color for static geometry (only supported for CStaticGeometryBase::TYPE_FLIGHTGAME)" },
	{ "SetFogStartDist",		SetFogStartDist,		METH_VARARGS, "set the z-distance where fog effect starts to take effect (only supported for CStaticGeometryBase::TYPE_FLIGHTGAME)" },
	{ "SetFogEndDist",			SetFogEndDist,			METH_VARARGS, "set the z-distance where fog color completely replaces geometry color (only supported for CStaticGeometryBase::TYPE_FLIGHTGAME)" },

	{ "SetCameraEntity",		SetCameraEntity,		METH_VARARGS, "sets an entity that controls the rendering" },
	{ "GetElapsedTime",			GetElapsedTime,			METH_VARARGS, "returns the elapsed time in stage [sec]" },
	{ "GetElapsedTimeMS",		GetElapsedTimeMS,		METH_VARARGS, "returns the elapsed time in stage [ms]" },
	{ "IsEntityAlive",			IsEntityAlive,			METH_VARARGS, "returns 1 if the entity with the given name currently exists in the stage, otherwise returns 0" },
	{ "EntityDestroyed",		EntityDestroyed,		METH_VARARGS, "returns 1 if the entity with the given name was destroyed in the stage, otherwise returns 0" },
//	{ "CreateEntity",			(PyCFunction)CreateEntity,		METH_VARARGS | METH_KEYWORDS, "creates an entity at a given position" },
//	{ "CreateEntityVec3",		CreateEntityVec3,		METH_VARARGS, "creates an entity at a given position" },
	{NULL, NULL}
};


void RegisterPythonModule_Stage( PythonScriptManager& mgr )
{
	mgr.AddModule( "Stage", sg_PyModuleStageMethod );
}


/*
PyObject* CreateEntityVec3( PyObject* self, PyObject* args )
{
	char *base_name;
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "(sfff)", &base_name, pos );	// does not work

//	gs_pTargetStage->CreateEntity(  );

	return Py_None;
}


PyObject* CreateEntity( PyObject* self, PyObject* args, PyObject *keywds )
{
	char *base_name, *name = NULL;
	Vector3 pos, vel = Vector3(0,0,0);
	float ang_h = 0.0f;
	int group = 0;

//	static char *kwlist[] = { "base_name", "x", "y", "z", "ang_h", "vx", "vy", "vz", "grp", "name" };

//	if( !PyArg_ParseTupleAndKeywords( args, keywds, "sfffffffis", kwlist, 
//                                      &base_name,
//									  &pos.x, &pos.y, &pos.z, &ang_h, &vel.x, &vel.y, &vel.z,
//									  &group, &name ) )
//	{
//		return NULL; 
//	}

	static char *kwlist[] = { "base_name", "name" };

	if( !PyArg_ParseTupleAndKeywords( args, keywds, "ss", kwlist, &base_name,
									  &name ) )
	{
		return NULL; 
	}

//	CCopyEntityDesc desc;
//	gs_pTargetStage->CreateEntity(  );

    Py_INCREF(Py_None);

    return Py_None;
}*/


} // namespace amorphous
