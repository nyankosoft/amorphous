#include "PyModule_VisualEffect.hpp"
#include "PyModule_Stage.hpp"

#include "amorphous/Graphics/FloatRGBColor.hpp"
#include "amorphous/Graphics/ShadowMaps.hpp"

#include "BaseEntity_Draw.hpp"
#include "Stage.hpp"
#include "EntitySet.hpp"
#include "EntityRenderManager.hpp"
#include "ScreenEffectManager.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"

#include "amorphous/Script/PythonScriptManager.hpp"

#include "amorphous/base.hpp"


namespace amorphous
{

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



static const int gs_DefaultEffectPriorityIDforScript = ScreenEffectManager::MAX_EFFECT_PRIORITY_ID;


using namespace amorphous::py::ve;


inline static ScreenEffectManager *GetScreenEffectManager()
{
	CStage *pStage = GetStageForScriptCallback();
	if( pStage )
		return pStage->GetScreenEffectManager().get();
	else
		return NULL;
}


inline static EntityRenderManager *GetEntityRenderManager()
{
	CStage *pStage = GetStageForScriptCallback();
	if( pStage )
		return pStage->GetEntitySet()->GetRenderManager().get();
	else
		return NULL;
}


PyObject* EnableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->EnableEnvironmentMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableEnvMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
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

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
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


PyObject* AddPlanarReflector( PyObject* self, PyObject* args )
{
	const char *entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CStage *pStage = GetStageForScriptCallback();

	CCopyEntity *pEntity = pStage->GetEntitySet()->GetEntityByName( entity_name );

	if( IsValidEntity(pEntity) )
	{
//		EntityRenderManager *pRenderMgr = GetEntityRenderManager();
//		if( pRenderMgr )
//		{
//			pEntity->RaiseEntityFlags( BETYPE_PLANAR_REFLECTOR );
//			pRenderMgr->AddPlanarReflector( EntityHandle<>(pEntity->Self()) );
//		}
		std::shared_ptr<BasicMesh> pMesh = pEntity->m_MeshHandle.GetMesh();
		int target_subset_index = 0;
//		if( pMesh )
//			RegisterAsPlanarMirror( *pEntity, *pMesh, target_subset_index );
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* RemovePlanarReflector( PyObject* self, PyObject* args )
{
	const char *entity_name = NULL;
	int result = PyArg_ParseTuple( args, "s", &entity_name );

	CStage *pStage = GetStageForScriptCallback();

	CCopyEntity *pEntity = pStage->GetEntitySet()->GetEntityByName( entity_name );

	if( IsValidEntity(pEntity) )
	{
		EntityRenderManager *pRenderMgr = GetEntityRenderManager();
		if( pRenderMgr )
		{
			bool remove_planar_refelection_group = true;
			pEntity->ClearEntityFlags( BETYPE_PLANAR_REFLECTOR );
			pRenderMgr->RemovePlanarReflector( EntityHandle<>(pEntity->Self()), remove_planar_refelection_group );
		}
	}

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* EnableSoftShadow( PyObject* self, PyObject* args )
{
	float softness = 1.0f;
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "|fi", &softness, &shadow_map_size );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
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

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
//	if( pRenderMgr )
//		pRenderMgr->EnableSoftShadowForLight( softness, shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}



PyObject* EnableShadowMap( PyObject* self, PyObject* args )
{
	int shadow_map_size = 512;
	int result = PyArg_ParseTuple( args, "|i", &shadow_map_size );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->EnableShadowMap( shadow_map_size );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* DisableShadowMap( PyObject* self, PyObject* args )
{
	const char *target_entity_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_entity_name );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->DisableShadowMap();

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetLightForShadow( PyObject* self, PyObject* args )
{
	const char *target_light_name = NULL;
	int result = PyArg_ParseTuple( args, "|s", &target_light_name );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetLightForShadow( target_light_name );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapCameraShiftDistance( PyObject* self, PyObject* args )
{
	float shift;
	int result = PyArg_ParseTuple( args, "f", &shift );

	LOG_PRINT_ERROR( " This is a deprecated function and no longer supported." );
//	CDirectionalLightShadowMap::ms_fCameraShiftDistance = shift;

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetOverrideShadowMapLight( PyObject* self, PyObject* args )
{
	int override_light;
	int result = PyArg_ParseTuple( args, "|i", &override_light );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetOverrideShadowMapLight( override_light != 0 ? true : false );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightPosition( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
	if( pRenderMgr )
		pRenderMgr->SetOverrideShadowMapLightPosition( Vector3(x,y,z) );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* SetShadowMapLightDirection( PyObject* self, PyObject* args )
{
	float x,y,z;
	int result = PyArg_ParseTuple( args, "fff", &x, &y, &z );

	EntityRenderManager *pRenderMgr = GetEntityRenderManager();
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

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetGlareLuminanceThreshold( threshold, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearGlare( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
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

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetMotionBlurWeight( blur_strength, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearMotionBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
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

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetBlurEffect( blur_x, blur_y, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearBlur( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
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

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->SetMonochromeEffect( r, g, b, blend_ratio, priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}


PyObject* ClearMonochrome( PyObject* self, PyObject* args )
{
	int priority_id = gs_DefaultEffectPriorityIDforScript;
	int result = PyArg_ParseTuple( args, "|i", &priority_id );

	ScreenEffectManager* pEffectMgr = GetScreenEffectManager();
	if( pEffectMgr )
		pEffectMgr->ClearMonochromeEffect( priority_id );

    Py_INCREF( Py_None );
	return Py_None;
}



static PyMethodDef sg_PyModuleVisualEffectMethod[] =
{
	{ "EnableEnvMap",                    EnableEnvMap,                    METH_VARARGS, "" },
	{ "DisableEnvMap",                   DisableEnvMap,                   METH_VARARGS, "" },
	{ "AddEnvMapTarget",                 AddEnvMapTarget,                 METH_VARARGS, "" },
	{ "RemoveEnvMapTarget",              RemoveEnvMapTarget,              METH_VARARGS, "" },
	{ "SaveEnvMapTextureToFile",         SaveEnvMapTextureToFile,         METH_VARARGS, "" },
	{ "AddPlanarReflector",              AddPlanarReflector,              METH_VARARGS, "" },
	{ "RemovePlanarReflector",           RemovePlanarReflector,           METH_VARARGS, "" },

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


void py::ve::RegisterPythonModule_VisualEffect( PythonScriptManager& mgr )
{
	mgr.AddModule( "VisualEffect", sg_PyModuleVisualEffectMethod );
}


} // namespace amorphous
