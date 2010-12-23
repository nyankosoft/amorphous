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

#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"

using namespace std;
using namespace boost;

/**
=========================================================================
 Script Templates
=========================================================================

import gfx
import visual_effect
import stage_util


screen_effect_mgr = visual_effect.GetScreenEffectManager()
lens_flare = screen_effect_mgr.GetLensFlare()


lens_flare.AddTexture( tex_path="textures/lens_flare.jpg", num_segs_x=2, num_segs_y=2 )
lens_flare.AddLensFlareRect( dim=0, scale_factor=1.0, dist_factor=1.0, color=gfx.Color(1,1,1,1), group_index=0, tex_seg_index_x=0, tex_seg_index_y=0 )
lens_flare.AddLensFlareRect( dim=0, scale_factor=1.0, dist_factor=1.0, color=gfx.Color(1,1,1,1), group_index=0, tex_seg_index_x=1, tex_seg_index_y=0 )
lens_flare.AddLensFlareRect( dim=0, scale_factor=1.0, dist_factor=1.0, color=gfx.Color(1,1,1,1), group_index=0, tex_seg_index_x=0, tex_seg_index_y=1 )
lens_flare.AddLensFlareRect( dim=0, scale_factor=1.0, dist_factor=1.0, color=gfx.Color(1,1,1,1), group_index=0, tex_seg_index_x=1, tex_seg_index_y=1 )

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


void (CScreenEffectManager::*SetBlurEffect)( float fBlurFactor, int priority_id )            = &CScreenEffectManager::SetBlurEffect;
void (CScreenEffectManager::*SetBlurEffectIWH)( int width, int height, int priority_id )     = &CScreenEffectManager::SetBlurEffect;
void (CScreenEffectManager::*SetBlurEffectFWH)( float width, float height, int priority_id ) = &CScreenEffectManager::SetBlurEffect;

void SetShadowMapCameraShiftDistance( float shift_distance )
{
//	CDirectionalLightShadowMap::ms_fCameraShiftDistance = shift_distance;
	COrthoShadowMap::ms_fCameraShiftDistance     = shift_distance;
	CSpotlightShadowMap::ms_fCameraShiftDistance = shift_distance;
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
		.def( "AddPlanarReflector",      &CEntityRenderManager::AddPlanarReflector )
		.def( "RemovePlanarReflector",   &CEntityRenderManager::RemovePlanarReflector )
		.def( "EnableSoftShadow",        &CEntityRenderManager::EnableSoftShadow, ( py::arg("softness")=1.0f, py::arg("shadowmap_size") = 512 ) )
		.def( "EnableShadowMap",         &CEntityRenderManager::EnableShadowMap )
		.def( "DisableShadowMap",        &CEntityRenderManager::DisableShadowMap )
	;

	static const int pid = gs_DefaultEffectPriorityIDforScript;

	// SetX -> EnableX
	// ClearX -> DisableX
	class_<CScreenEffectManager, shared_ptr<CScreenEffectManager> >("ScreenEffectManager")
		.def( "FadeOutTo",              &CScreenEffectManager::FadeOutTo )
		.def( "FadeInFrom",             &CScreenEffectManager::FadeInFrom )
//		.def( "SetFadeEffect",          &CScreenEffectManager::SetFadeEffect )

		.def( "SetGlare",               &CScreenEffectManager::SetGlareLuminanceThreshold,   ( py::arg("threshold"), py::arg("priority_id") = pid ) )
		.def( "ClearGlare",             &CScreenEffectManager::ClearGlareLuminanceThreshold, ( py::arg("priority_id") = pid ) )
		.def( "SetMotionBlur",          &CScreenEffectManager::SetMotionBlurWeight,          ( py::arg("motion_blur_weight"), py::arg("priority_id") = pid ) )
		.def( "ClearMotionBlur",        &CScreenEffectManager::ClearMotionBlur,              ( py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffect,                                       ( py::arg("blur_factor"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffectIWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffectFWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "ClearBlur",              &CScreenEffectManager::ClearBlurEffect,              ( py::arg("priority_id") = pid ) )
		.def( "SetMonochrome",          &CScreenEffectManager::SetMonochromeEffect,               ( py::arg("r"), py::arg("g"), py::arg("b"), py::arg("blend_ratio") = 1.0f, py::arg("priority_id") = pid ) )
		.def( "ClearMonochrome",        &CScreenEffectManager::ClearMonochromeEffect,             ( py::arg("priority_id") = pid ) )
//		.def( "SetHDRLightingParams",   &CScreenEffectManager::SetHDRLightingParams )
		.def( "EnableHDRLighting",      &CScreenEffectManager::EnableHDRLighting )
		.def( "GetLensFlare",           &CScreenEffectManager::GetLensFlare )
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
