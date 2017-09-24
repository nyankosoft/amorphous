#include "PyModule_visual_effect.hpp"
#include "PyModule_StageUtility.hpp"
#include <boost/python.hpp>

#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Graphics/ShadowMaps.hpp"
#include "amorphous/Stage/BaseEntity_Draw.hpp"
#include "amorphous/Stage/BE_DirectionalLight.hpp"
#include "amorphous/Stage/BE_PointLight.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/EntityRenderManager.hpp"
#include "amorphous/Stage/ScreenEffectManager.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"

#include "amorphous/base.hpp"


namespace amorphous
{

using boost::shared_ptr;

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


static const int gs_DefaultEffectPriorityIDforScript = ScreenEffectManager::MAX_EFFECT_PRIORITY_ID;



inline static shared_ptr<ScreenEffectManager> GetScreenEffectManager()
{

	shared_ptr<CStage> pStage = stage_util::GetStageForScript();
	if( pStage )
		return pStage->GetScreenEffectManager();
	else
		return shared_ptr<ScreenEffectManager>();
}


inline static shared_ptr<EntityRenderManager> GetEntityRenderManager()
{
	shared_ptr<CStage> pStage = stage_util::GetStageForScript();
	if( pStage )
		return pStage->GetEntitySet()->GetRenderManager();
	else
		return shared_ptr<EntityRenderManager>();
}


void (ScreenEffectManager::*SetBlurEffect)( float fBlurFactor, int priority_id )            = &ScreenEffectManager::SetBlurEffect;
void (ScreenEffectManager::*SetBlurEffectIWH)( int width, int height, int priority_id )     = &ScreenEffectManager::SetBlurEffect;
void (ScreenEffectManager::*SetBlurEffectFWH)( float width, float height, int priority_id ) = &ScreenEffectManager::SetBlurEffect;

void SetShadowMapCameraShiftDistance( float shift_distance )
{
//	DirectionalLightShadowMap::ms_fCameraShiftDistance = shift_distance;
	OrthoShadowMap::ms_fCameraShiftDistance     = shift_distance;
	SpotlightShadowMap::ms_fCameraShiftDistance = shift_distance;
}


BOOST_PYTHON_MODULE(visual_effect)
{
	using namespace boost::python;
	namespace py = boost::python;

	def( "SetShadowMapCameraShiftDistance", SetShadowMapCameraShiftDistance );

	def( "GetScreenEffectManager", GetScreenEffectManager );
	def( "GetEntityRenderManager", GetEntityRenderManager );

	class_<EntityRenderManager, shared_ptr<EntityRenderManager> >("EntityRenderManager")
		.def( "EnableEnvMap",            &EntityRenderManager::EnableEnvironmentMap )
		.def( "DisableEnvMap",           &EntityRenderManager::DisableEnvironmentMap )
		.def( "AddEnvMapTarget",         &EntityRenderManager::AddEnvMapTarget )
		.def( "SaveEnvMapTextureToFile", &EntityRenderManager::SaveEnvMapTextureToFile )
		.def( "RemoveEnvMapTarget",      &EntityRenderManager::RemoveEnvMapTarget )
		.def( "AddPlanarReflector",      &EntityRenderManager::AddPlanarReflector )
		.def( "RemovePlanarReflector",   &EntityRenderManager::RemovePlanarReflector )
		.def( "EnableSoftShadow",        &EntityRenderManager::EnableSoftShadow, ( py::arg("softness")=1.0f, py::arg("shadowmap_size") = 512 ) )
		.def( "EnableShadowMap",         &EntityRenderManager::EnableShadowMap )
		.def( "DisableShadowMap",        &EntityRenderManager::DisableShadowMap )
	;

	static const int pid = gs_DefaultEffectPriorityIDforScript;

	// SetX -> EnableX
	// ClearX -> DisableX
	class_<ScreenEffectManager, shared_ptr<ScreenEffectManager> >("ScreenEffectManager")
		.def( "FadeOutTo",              &ScreenEffectManager::FadeOutTo )
		.def( "FadeInFrom",             &ScreenEffectManager::FadeInFrom )
//		.def( "SetFadeEffect",          &ScreenEffectManager::SetFadeEffect )

		.def( "SetGlare",               &ScreenEffectManager::SetGlareLuminanceThreshold,   ( py::arg("threshold"), py::arg("priority_id") = pid ) )
		.def( "ClearGlare",             &ScreenEffectManager::ClearGlareLuminanceThreshold, ( py::arg("priority_id") = pid ) )
		.def( "SetMotionBlur",          &ScreenEffectManager::SetMotionBlurWeight,          ( py::arg("motion_blur_weight"), py::arg("priority_id") = pid ) )
		.def( "ClearMotionBlur",        &ScreenEffectManager::ClearMotionBlur,              ( py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffect,                                       ( py::arg("blur_factor"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffectIWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "SetBlur",                SetBlurEffectFWH,                                    ( py::arg("width"), py::arg("height"), py::arg("priority_id") = pid ) )
		.def( "ClearBlur",              &ScreenEffectManager::ClearBlurEffect,              ( py::arg("priority_id") = pid ) )
		.def( "SetMonochrome",          &ScreenEffectManager::SetMonochromeEffect,               ( py::arg("r"), py::arg("g"), py::arg("b"), py::arg("blend_ratio") = 1.0f, py::arg("priority_id") = pid ) )
		.def( "ClearMonochrome",        &ScreenEffectManager::ClearMonochromeEffect,             ( py::arg("priority_id") = pid ) )
//		.def( "SetHDRLightingParams",   &ScreenEffectManager::SetHDRLightingParams )
		.def( "EnableHDRLighting",      &ScreenEffectManager::EnableHDRLighting )
		.def( "GetLensFlare",           &ScreenEffectManager::GetLensFlare )
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
PyMethodDef amorphous::py::ve::g_PyModuleVisualEffectMethod[] =
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


} // namespace amorphous
