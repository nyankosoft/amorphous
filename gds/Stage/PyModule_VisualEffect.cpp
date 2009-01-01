
#include "PyModule_VisualEffect.h"
#include "PyModule_Stage.h"

#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"
#include "Graphics/FloatRGBColor.h"

#include "BE_DirectionalLight.h"
#include "BE_PointLight.h"
#include "CopyEntityDesc.h"
#include "Stage.h"
#include "EntitySet.h"
#include "EntityRenderManager.h"
#include "ScreenEffectManager.h"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/DefaultLog.h"

#include "../base.h"

using namespace std;

/**
 - enable shadow mapping

	import VisualEffect

	def run():
		VisualEffect.EnableShadowMap()
		VisualEffect.SetShadowMapLightPosition( 2.0, 6.0, 1.0 )
		VisualEffect.SetShadowMapLightDirection( -0.1, -1.0, -0.2 )
		VisualEffect.SetOverrideShadowMapLight( True )

		Entity.SetShadowCaster( "(name)", True )
		Entity.SetShadowReceiver( "(name)", True )
		...

		return 1



*/


#define RETURN_PYNONE_IF_NO_STAGE()	if( !GetStageForScriptCallback() )	{	Py_INCREF( Py_None );	return Py_None;	}



static const int gs_DefaultEffectPriorityIDforScript = CScreenEffectManager::MAX_EFFECT_PRIORITY_ID;


using namespace gsf::py::ve;


PyObject* EnableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->EnableEnvironmentMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->DisableEnvironmentMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* AddEnvMapTarget( PyObject* self, PyObject* args )
{
	char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();

	CCopyEntity *pEntity = pStage->GetEntitySet()->GetEntityByName( target_entity_name );

	bool res = false;
	if( IsValidEntity(pEntity) )
	{
		pEntity->EntityFlag |= BETYPE_ENVMAPTARGET;
		res = pStage->GetEntitySet()->GetRenderManager()->AddEnvMapTarget( pEntity );
	}

    Py_INCREF( Py_None );
	return Py_None;
}


/// target_entity_name is ignored.
/// TODO: support target_entity_name, or multiple envmap textures
PyObject* SaveEnvMapTextureToFile( PyObject* self, PyObject* args )
{
	char *target_entity_name;
	char *output_image_filename;
	int result = PyArg_ParseTuple( args, "ss", &target_entity_name, &output_image_filename );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();

	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();
	if( pRenderMgr )
	{
		pRenderMgr->SaveEnvMapTextureToFile( output_image_filename );
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* RemoveEnvMapTarget( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();

	CCopyEntity *pEntity = pStage->GetEntitySet()->GetEntityByName( target_entity_name );

	bool res = false;
	if( IsValidEntity(pEntity) )
	{
		pEntity->EntityFlag &= (~BETYPE_ENVMAPTARGET);
		res = pStage->GetEntitySet()->GetRenderManager()->RemoveEnvMapTarget( pEntity );
	}

    Py_INCREF( Py_None );
	return Py_None;
}





PyObject* EnableSoftShadow( PyObject* self, PyObject* args )
{
	float softness = 1.0f;
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "|fi", &softness, &shadow_map_size );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->EnableSoftShadow( softness, shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* EnableSoftShadowForLight( PyObject* self, PyObject* args )
{
	int light_id;
	float softness = 1.0f;
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "i|fi", &light_id, &softness, &shadow_map_size );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

//	pRenderMgr->EnableSoftShadowForLight( softness, shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}



PyObject* EnableShadowMap( PyObject* self, PyObject* args )
{
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "|i", &shadow_map_size );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->EnableShadowMap( shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableShadowMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->DisableShadowMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetOverrideShadowMapLight( PyObject* self, PyObject* args )
{
	int override_light;
	int result = PyArg_ParseTuple( args, "|i", &override_light );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->SetOverrideShadowMapLight( override_light != 0 ? true : false );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightPosition( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	pRenderMgr->SetOverrideShadowMapLightPosition( Vector3(x,y,z) );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightDirection( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	Vector3 vDir =  Vector3(x,y,z);
	Vec3Normalize( vDir, vDir );
	pRenderMgr->SetOverrideShadowMapLightDirection( vDir );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetGlare( PyObject* self, PyObject* args )
{
	float threshold;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "f|i", &threshold, &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->SetGlareLuminanceThreshold( threshold, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearGlare( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->ClearGlareLuminanceThreshold( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetMotionBlur( PyObject* self, PyObject* args )
{
	float blur_strength;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "f|i", &blur_strength, &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->SetMotionBlurWeight( blur_strength, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearMotionBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->ClearMotionBlur( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetBlur( PyObject* self, PyObject* args )
{
	float blur_x, blur_y;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "ff|i", &blur_x, &blur_y, &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->SetBlurEffect( blur_x, blur_y, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CScreenEffectManager* pEffectMgr = pStage->GetScreenEffectManager();

	pEffectMgr->ClearBlurEffect( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}



PyMethodDef gsf::py::ve::g_PyModuleVisualEffectMethod[] =
{
	{ "EnableEnvMap",               EnableEnvMap,               METH_VARARGS, "" },
	{ "DisableEnvMap",              DisableEnvMap,              METH_VARARGS, "" },
	{ "AddEnvMapTarget",            AddEnvMapTarget,            METH_VARARGS, "" },
	{ "RemoveEnvMapTarget",         RemoveEnvMapTarget,         METH_VARARGS, "" },
	{ "SaveEnvMapTextureToFile",    SaveEnvMapTextureToFile,    METH_VARARGS, "" },
	{ "EnableShadowMap",            EnableShadowMap,            METH_VARARGS, "" },
	{ "DisableShadowMap",           DisableShadowMap,           METH_VARARGS, "" },
	{ "EnableSoftShadow",           EnableSoftShadow,           METH_VARARGS, "" },
	{ "SetOverrideShadowMapLight",  SetOverrideShadowMapLight,  METH_VARARGS, "" },
	{ "SetShadowMapLightPosition",  SetShadowMapLightPosition,  METH_VARARGS, "" },
	{ "SetShadowMapLightDirection", SetShadowMapLightDirection, METH_VARARGS, "" },
	{ "SetGlare",                   SetGlare,                   METH_VARARGS, "" },
	{ "ClearGlare",                 ClearGlare,                 METH_VARARGS, "" },
	{ "SetMotionBlur",              SetMotionBlur,              METH_VARARGS, "" },
	{ "ClearMotionBlur",            ClearMotionBlur,            METH_VARARGS, "" },
	{ "SetBlur",                    SetBlur,                    METH_VARARGS, "" },
	{ "ClearBlur",                  ClearBlur,                  METH_VARARGS, "" },
	{NULL, NULL}
};



/*
PyObject* SetEnvMap( PyObject* self, PyObject* args )
{
	int enable_env_mapping;
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "i|s", &enable_env_mapping, &target_entity_name );

	RETURN_PYNONE_IF_NO_STAGE()

	CStage *pStage = GetStageForScriptCallback();
	CEntityRenderManager *pRenderMgr = pStage->GetEntitySet()->GetRenderManager();

	if( enable_env_mapping != 0 )
	{
		pRenderMgr->CreateEnvironmentMapTexture( true );
	}
	else
	{
		//
	}

    Py_INCREF( Py_None );
	return Py_None;
}*/

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