#include "PyModule_Light.h"
#include "PyModule_Stage.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"
#include "3DCommon/FloatRGBColor.h"

#include "BE_DirectionalLight.h"
#include "BE_PointLight.h"
#include "Stage.h"
#include "EntitySet.h"

#include "CopyEntityDesc.h"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "../base.h"

using namespace std;



/// row:    holds light groups
/// column: holds object groups (lighting groups)
C2DArray<char> m_LightToObject;

int init_light_groups()
{
	const int num_light_groups = 16;
	const int num_ligting_groups = 16;
	m_LightToObject.resize( num_light_groups, num_ligting_groups );

	return 0;
}


#define RETURN_PYNONE_IF_NO_STAGE()	 if( !GetStageForScriptCallback() )        { Py_INCREF( Py_None );	return Py_None;	}
#define RETURN_PYNONE_IF_NO_TARGET() if( !IsValidEntity(GetEntityForLight()) ) { Py_INCREF( Py_None );	return Py_None;	}


CCopyEntity *gs_pEntityForLight = NULL;

inline CCopyEntity *GetEntityForLight() { return gs_pEntityForLight; }


void gsf::py::light::SetEntityForLight( CCopyEntity* pEntity )
{
	gs_pEntityForLight = pEntity;
}


using namespace gsf::py::light;

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
	if( GetStageForScriptCallback() )
        return GetStageForScriptCallback()->GetEntitySet()->GetEntityByName(entity_name);
	else
        return NULL;
}



PyObject* gsf::py::light::CreateDirectionalLight( PyObject* self, PyObject* args )
{
	char *light_name;
	char *light_type = "dynamic"; // "dynamic" or "static";
	int light_group = 0;
	int shadow_for_light = 1; // true(1) by default

	int result = PyArg_ParseTuple( args, "s|sds", &light_name, &light_type, &light_group, &shadow_for_light );

	const Vector3 vDefaultDir = Vector3(0,-1,0);

	CCopyEntity *pLightEntity
	= CreateNamedEntity( light_name, "StaticPointLight",
		Vector3(0,0,0), // position
		vDefaultDir,    // direction
		Vector3(0,0,0)  // velocity
		);

	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreatePointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateHSDirectionalLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateHSPointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateHSSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriDirectionalLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriPointLight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::CreateTriSpotlight( PyObject* self, PyObject* args )
{
	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetAttenuationFactors( PyObject* self, PyObject* args )
{
	g_Log.Print( "Light.SetAttenuFactors() - called" );

	float a[3];

	int result = PyArg_ParseTuple( args, "fff", &a[0], &a[1], &a[2] );

	RETURN_PYNONE_IF_NO_TARGET()
	RETURN_PYNONE_IF_NO_STAGE()

	CLightEntity *pLightEntity
		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
		pLightEntity->SetAttenuationFactors( a[0], a[1], a[2] );

	CBE_Light::SetAttenuationFactors( GetEntityForLight(), a[0], a[1], a[2] );

    Py_INCREF( Py_None );
	return Py_None;
}


/**
 \param index 0:upper hemisphere color, 1:??? color, 2: lower hemisphere color


*/
static void SetLightColor( int index, const SFloatRGBColor& color )
{
	if( !GetStageForScriptCallback() )
		return;

	CLightEntity *pLightEntity
		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
		pLightEntity->SetColor( index, color );

	const Vector3 vColor = Vector3( color.fRed, color.fGreen, color.fBlue );

	switch( index )
	{
	case 0: GetEntityForLight()->v1 = vColor; break;
	case 1: GetEntityForLight()->v2 = vColor; break;
	case 2: GetEntityForLight()->v3 = vColor; break;
	default:
		g_Log.Print( WL_ERROR, "SetColor() - invalid light color index: %d. Must be [0,2]" );
	}
}


PyObject* gsf::py::light::SetColorU32( PyObject* self, PyObject* args )
{
	int index;
	U32 color;	// 32-bit ARGB color (8 bits for each channel)
	int result = PyArg_ParseTuple( args, "ik", &index, &color );

	SFloatRGBColor dest_color;
	dest_color.SetARGB32( color );

	RETURN_PYNONE_IF_NO_TARGET()

	SetLightColor( index, dest_color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetColor( PyObject* self, PyObject* args )
{
	int index;
	SFloatRGBColor color;
	int result = PyArg_ParseTuple( args, "ifff", &index, &color.fRed, &color.fGreen, &color.fBlue );

	RETURN_PYNONE_IF_NO_TARGET()

	SetLightColor( index, color );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetTargetEntity( PyObject* self, PyObject* args )
{
	g_Log.Print( "Light.SetTargetEntity() - called" );

	RETURN_PYNONE_IF_NO_STAGE()

	char *entity_name;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CCopyEntity* pEntityForLight = GetEntityByName( entity_name );

	// TODO: what if the focus target entity is created later
	if( IsValidEntity(pEntityForLight) )
	{
		SetEntityForLight( pEntityForLight );
	}
	else
	{
//		g_Log.Print( "" );
		Py_INCREF( Py_None );
		return Py_None;
	}


	Py_INCREF( Py_None );
	return Py_None;
}


PyObject* gsf::py::light::SetPosition( PyObject* self, PyObject* args )
{
	Vector3 pos;
	int result = PyArg_ParseTuple( args, "fff", &pos.x, &pos.y, &pos.z );

	RETURN_PYNONE_IF_NO_TARGET()
	RETURN_PYNONE_IF_NO_STAGE()

	CLightEntity *pLightEntity
		= GetStageForScriptCallback()->GetEntitySet()->GetLightEntity( GetEntityForLight()->iExtraDataIndex );

	if( pLightEntity )
        pLightEntity->SetPosition( pos );
	GetEntityForLight()->SetPosition( pos );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef gsf::py::light::g_PyModuleLightMethod[] =
{
	{ "CreatePointLight",          gsf::py::light::CreatePointLight,             METH_VARARGS, "" },
	{ "CreateDirectionalLight",    gsf::py::light::CreateDirectionalLight,       METH_VARARGS, "" },
	{ "CreateSpotlight",           gsf::py::light::CreateSpotlight,              METH_VARARGS, "" },
	{ "CreateHSPointLight",        gsf::py::light::CreateHSPointLight,           METH_VARARGS, "" },
	{ "CreateHSDirectionalLight",  gsf::py::light::CreateHSDirectionalLight,     METH_VARARGS, "" },
	{ "CreateHSSpotlight",         gsf::py::light::CreateHSSpotlight,            METH_VARARGS, "" },
	{ "CreateTriPointLight",       gsf::py::light::CreateTriPointLight,          METH_VARARGS, "" },
	{ "CreateTriDirectionalLight", gsf::py::light::CreateTriDirectionalLight,    METH_VARARGS, "" },
	{ "CreateTriSpotlight",        gsf::py::light::CreateTriSpotlight,           METH_VARARGS, "" },

	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetTargetEntity",        gsf::py::light::SetTargetEntity, METH_VARARGS, "" },
	{ "SetColor",               gsf::py::light::SetColor,        METH_VARARGS, "" },
	{ "SetColorU32",            gsf::py::light::SetColorU32,     METH_VARARGS, "" },
	{ "SetPosition",            gsf::py::light::SetPosition,	 METH_VARARGS, "" },
//	{ "SetDirection",           gsf::py::light::SetDirection,	 METH_VARARGS, "" },
	{ "SetAttenuationFactors",  SetAttenuationFactors,	         METH_VARARGS, "set attenuation factors for point lights" },
//	{ "SetDirection",      gsf::py::light::SetDirection,	METH_VARARGS, "" },
//	{ "",       SetTarget,				METH_VARARGS, "" },
	{NULL, NULL}
};


/*
PyObject* EndCameraScript( PyObject* self, PyObject* args )
{
	RETURN_PYNONE_IF_NO_STAGE()

	char *camera_entity_name;
	int result = PyArg_ParseTuple( args, "s", &camera_entity_name );

	// set motion path (pos & orientation)
	SGameMessage msg( GM_SET_MOTION_PATH );
	msg.pUserData = &g_EntityMotionPathRequest;

	g_Log.Print( "EndCameraScript() - sending motion path to camera entity (%d key poses)", (int)g_EntityMotionPathRequest.vecKeyPose.size() );

	SendGameMessageTo( msg, g_EntityMotionPathRequest.pTargetEntity );

	// set camera-related effects
	// camera property: fov, nearclip, farclip, etc.
	// effect property: blur, glare, etc.
	msg = SGameMessage( GM_SET_SCRIPTCAMERAKEYFRAMES );
	msg.pUserData = &gs_ScriptCameraKeyFrames;

	g_Log.Print( "EndCameraScript() - sending camera effects to script camera entity" );

	SendGameMessageTo( msg, g_EntityMotionPathRequest.pTargetEntity );

    Py_INCREF( Py_None );
	return Py_None;
}*/