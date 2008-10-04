
#include "PyModule_Stage.h"

#include "3DMath/Vector3.h"
#include "3DCommon/3DGameMath.h"
#include "Support/Vec3_StringAux.h"

#include "Stage.h"
#include "CopyEntityDesc.h"
#include "StaticGeometryFG.h"	// used by SetFogColor()
#include "BE_Skybox.h"	// used by SetFogColor()

using namespace std;


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
	desc.WorldPose.vPosition  = pos;

	desc.SetWorldOrient( CreateOrientFromFwdDir( dir ) );

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




PyObject* LoadStaticGeometry( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *filename;

	int result = PyArg_ParseTuple( args, "s", &filename );

	gs_pTargetStage->LoadStaticGeometryFromFile( filename );

    Py_INCREF( Py_None );
	return Py_None;
}



PyObject* LoadSkybox( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *base_entity_name;
	char *texture_filename;

	int result = PyArg_ParseTuple( args, "ss", &base_entity_name, &texture_filename );

	CCopyEntity* pSkyboxEntity = CreateNamedEntityHrz( "skybox", base_entity_name, Vector3(0,0,0), 0, 0 );

	if( !pSkyboxEntity
	 || pSkyboxEntity->pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_SKYBOX )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	((CBE_Skybox *)pSkyboxEntity->pBaseEntity)->LoadSkyboxTexture( texture_filename );

    Py_INCREF( Py_None );
	return Py_None;
}


static inline void SetAmbientColor( const SFloatRGBAColor& ambient_color )
{
	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetAmbientColor( ambient_color );
}


PyObject* SetAmbientColor( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	SFloatRGBAColor ambient_color;
	int result = PyArg_ParseTuple( args, "fff", &ambient_color.fRed, &ambient_color.fGreen, &ambient_color.fBlue );

	SetAmbientColor( ambient_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetAmbientColorC32( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

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
	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogColor( fog_color );
}


PyObject* SetFogColor( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	SFloatRGBAColor fog_color;
	int result = PyArg_ParseTuple( args, "fff", &fog_color.fRed, &fog_color.fGreen, &fog_color.fBlue );

	SetFogColor( fog_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetFogColorC32( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

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
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	float dist;
	int result = PyArg_ParseTuple( args, "f", &dist );

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogStartDist( dist );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetFogEndDist( PyObject* self, PyObject* args )
{
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	float dist;
	int result = PyArg_ParseTuple( args, "f", &dist );

	CStaticGeometryBase* pStaticGeometry = gs_pTargetStage->GetStaticGeometry();

	if( pStaticGeometry )
		pStaticGeometry->SetFogEndDist( dist );

    Py_INCREF( Py_None );
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

	CreateNamedEntity( "", base_name, pos, dir, vel );

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
	if( !gs_pTargetStage )
	{
	    Py_INCREF( Py_None );
		return Py_None;
	}

	char *entity_name;

	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CEntitySet *pEntitySet = gs_pTargetStage->GetEntitySet();
	pEntitySet->SetCameraEntity( pEntitySet->GetEntityByName(entity_name) );

    Py_INCREF( Py_None );
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


PyMethodDef g_PyModuleStageMethod[] =
{
	{ "CreateEntity",			CreateEntity,			METH_VARARGS, "creates an entity at a position with a direction & a velocity" },
	{ "CreateEntityAt",			CreateEntityAt,			METH_VARARGS, "creates an entity at a given position" },
	{ "CreateNamedEntityHrz",	CreateNamedEntityHrz,	METH_VARARGS, "creates an entity at a given position" },
	{ "CreateEntityHrz",		CreateEntityHrz,		METH_VARARGS, "creates an entity at a given position" },

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
