#include "PyModule_visual_effect.hpp"
#include "PyModule_StageUtility.hpp"
#include <boost/python.hpp>

#include "3DMath/Matrix34.hpp"
#include "Graphics/ShadowMaps.hpp"
#include "Stage/BaseEntity_Draw.hpp"
#include "Stage/BE_DirectionalLight.hpp"
#include "Stage/BE_PointLight.hpp"
#include "Stage/Stage.hpp"
#include "Stage/EntityRenderManager.hpp"
#include "Stage/ScreenEffectManager.hpp"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"

using namespace std;
using namespace boost;

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


static const int gs_DefaultEffectPriorityIDforScript = CScreenEffectManager::MAX_EFFECT_PRIORITY_ID;



inline static shared_ptr<CScreenEffectManager> GetScreenEffectManager()
{

	shared_ptr<CStage> pStage = stage_util::GetStageForScript();
	if( pStage )
		return pStage->GetScreenEffectManager();
	else
		return shared_ptr<CScreenEffectManager>();
}


inline static shared_ptr<CEntityRenderManager> GetEntityRenderManager()
{
	shared_ptr<CStage> pStage = stage_util::GetStageForScript();
	if( pStage )
		return pStage->GetEntitySet()->GetRenderManager();
	else
		return shared_ptr<CEntityRenderManager>();
}


/*
PyObject* EnableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->EnableEnvironmentMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
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
		pEntity->RaiseEntityFlags( BETYPE_ENVMAPTARGET );
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

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SaveEnvMapTextureToFile( output_image_filename );

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
		pEntity->ClearEntityFlags( BETYPE_ENVMAPTARGET );
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

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
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

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
//	if( pRenderMgr )
//		pRenderMgr->EnableSoftShadowForLight( softness, shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}



PyObject* EnableShadowMap( PyObject* self, PyObject* args )
{
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "|i", &shadow_map_size );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->EnableShadowMap( shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableShadowMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->DisableShadowMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetLightForShadow( PyObject* self, PyObject* args )
{
	const char *target_light_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_light_name );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetLightForShadow( target_light_name );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetOverrideShadowMapLight( PyObject* self, PyObject* args )
{
	int override_light;
	int result = PyArg_ParseTuple( args, "|i", &override_light );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetOverrideShadowMapLight( override_light != 0 ? true : false );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightPosition( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetOverrideShadowMapLightPosition( Vector3(x,y,z) );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightDirection( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	CEntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
	{
		Vector3 vDir =  Vector3(x,y,z);
		Vec3Normalize( vDir, vDir );
		pRenderMgr->SetOverrideShadowMapLightDirection( vDir );
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetGlare( PyObject* self, PyObject* args )
{
	float threshold;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "f|i", &threshold, &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetGlareLuminanceThreshold( threshold, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearGlare( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->ClearGlareLuminanceThreshold( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetMotionBlur( PyObject* self, PyObject* args )
{
	float blur_strength;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "f|i", &blur_strength, &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetMotionBlurWeight( blur_strength, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearMotionBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->ClearMotionBlur( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetBlur( PyObject* self, PyObject* args )
{
	float blur_x, blur_y;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "ff|i", &blur_x, &blur_y, &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetBlurEffect( blur_x, blur_y, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->ClearBlurEffect( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetMonochrome( PyObject* self, PyObject* args )
{
	float r, g, b;
	float blend_ratio = 1.0f;
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "fff|fi", &r, &g, &b, &blend_ratio, &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetMonochromeEffect( r, g, b, blend_ratio, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearMonochrome( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	CScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->ClearMonochromeEffect( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}
*/


void (CScreenEffectManager::*SetBlurEffect)( float fBlurFactor, int priority_id )            = &CScreenEffectManager::SetBlurEffect;
void (CScreenEffectManager::*SetBlurEffectIWH)( int width, int height, int priority_id )     = &CScreenEffectManager::SetBlurEffect;
void (CScreenEffectManager::*SetBlurEffectFWH)( float width, float height, int priority_id ) = &CScreenEffectManager::SetBlurEffect;

void SetShadowMapCameraShiftDistance( float shift_distance )
{
	CDirectionalLightShadowMap::ms_fCameraShiftDistance = shift_distance;
}


BOOST_PYTHON_MODULE(visual_effect)
{
	using namespace boost::python;
	namespace py = boost::python;

	def( "SetShadowMapCameraShiftDistance", SetShadowMapCameraShiftDistance );

	def( "GetScreenEffectManager", GetScreenEffectManager );
	def( "GetEntityRenderManager", GetEntityRenderManager );

	class_<CEntityRenderManager, shared_ptr<CEntityRenderManager> >("EntityRenderManager")
		.def( "EnableEnvMap",            &CEntityRenderManager::EnableEnvironmentMap )
		.def( "DisableEnvMap",           &CEntityRenderManager::DisableEnvironmentMap )
		.def( "AddEnvMapTarget",         &CEntityRenderManager::AddEnvMapTarget )
		.def( "SaveEnvMapTextureToFile", &CEntityRenderManager::SaveEnvMapTextureToFile )
		.def( "RemoveEnvMapTarget",      &CEntityRenderManager::RemoveEnvMapTarget )
		.def( "EnableSoftShadow",        &CEntityRenderManager::EnableSoftShadow, ( py::arg("softness")=1.0f, py::arg("shadowmap_size") = 512 ) )
		.def( "EnableShadowMap",         &CEntityRenderManager::EnableShadowMap )
		.def( "DisableShadowMap",        &CEntityRenderManager::DisableShadowMap )
	;

	static const int pid = gs_DefaultEffectPriorityIDforScript;

	// SetX -> EnableX
	// ClearX -> DisableX
	class_<CScreenEffectManager, shared_ptr<CScreenEffectManager> >("ScreenEffectManager")
		.def( "SetGlare",           &CScreenEffectManager::SetGlareLuminanceThreshold,   ( py::arg("threshold"), py::arg("priority_id") = pid ) )
		.def( "ClearGlare",         &CScreenEffectManager::ClearGlareLuminanceThreshold, ( py::arg("priority_id") = pid ) )
		.def( "SetMotionBlur",      &CScreenEffectManager::SetMotionBlurWeight,          ( py::arg("motion_blur_weight"), py::arg("priority_id") = pid ) )
		.def( "ClearMotionBlur",    &CScreenEffectManager::ClearMotionBlur,              ( py::arg("priority_id") = pid ) )
		.def( "SetBlur",            SetBlurEffect,                                       ( py::arg("blur_factor"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",            SetBlurEffectIWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",            SetBlurEffectFWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "ClearBlur",          &CScreenEffectManager::ClearBlurEffect,              ( py::arg("priority_id") = pid ) )
		.def( "SetMonochrome",      &CScreenEffectManager::SetMonochromeEffect,               ( py::arg("r"), py::arg("g"), py::arg("b"), py::arg("blend_ratio") = 1.0f, py::arg("priority_id") = pid ) )
		.def( "ClearMonochrome",    &CScreenEffectManager::ClearMonochromeEffect,             ( py::arg("priority_id") = pid ) )
	;
}


void RegisterPythonModule_visual_effect()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("visual_effect", initvisual_effect) == -1)
	{
		const char *msg = "Failed to add 'visual_effect' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}

/*
PyMethodDef gsf::py::ve::g_PyModuleVisualEffectMethod[] =
{
	{ "EnableEnvMap",                    EnableEnvMap,                    METH_VARARGS, "" },
	{ "DisableEnvMap",                   DisableEnvMap,                   METH_VARARGS, "" },
	{ "AddEnvMapTarget",                 AddEnvMapTarget,                 METH_VARARGS, "" },
	{ "RemoveEnvMapTarget",              RemoveEnvMapTarget,              METH_VARARGS, "" },
	{ "SaveEnvMapTextureToFile",         SaveEnvMapTextureToFile,         METH_VARARGS, "" },

	{ "EnableShadowMap",                 EnableShadowMap,                 METH_VARARGS, "" },
	{ "DisableShadowMap",                DisableShadowMap,                METH_VARARGS, "" },
	{ "EnableSoftShadow",                EnableSoftShadow,                METH_VARARGS, "" },
//	{ "SetNumMaxLightsForShadow",        SetNumMaxLightsForShadow,        METH_VARARGS, "" },
	{ "SetLightForShadow",               SetLightForShadow,               METH_VARARGS, "" },

	{ "SetShadowMapCameraShiftDistance", SetShadowMapCameraShiftDistance, METH_VARARGS, "" },

	// removed
	{ "SetOverrideShadowMapLight",       SetOverrideShadowMapLight,       METH_VARARGS, "" },
	{ "SetShadowMapLightPosition",       SetShadowMapLightPosition,       METH_VARARGS, "" },
	{ "SetShadowMapLightDirection",      SetShadowMapLightDirection,      METH_VARARGS, "" },

	{ "SetGlare",                        SetGlare,                        METH_VARARGS, "" },
	{ "ClearGlare",                      ClearGlare,                      METH_VARARGS, "" },
	{ "SetMotionBlur",                   SetMotionBlur,                   METH_VARARGS, "" },
	{ "ClearMotionBlur",                 ClearMotionBlur,                 METH_VARARGS, "" },
	{ "SetBlur",                         SetBlur,                         METH_VARARGS, "" },
	{ "ClearBlur",                       ClearBlur,                       METH_VARARGS, "" },
	{ "SetMonochrome",                   SetMonochrome,                   METH_VARARGS, "" },
	{ "ClearMonochrome",                 ClearMonochrome,                 METH_VARARGS, "" },
	{NULL, NULL}
};
*/
