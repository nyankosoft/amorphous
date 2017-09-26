#include "amorphous/Script/PyModule_stage.hpp"
#include <boost/python.hpp>

#include "../base.hpp"
#include "../Graphics/ShaderHandle.hpp"
#include "../Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "../Stage/Stage.hpp"
#include "../Stage/BE_PointLight.hpp"
#include "../Stage/CopyEntityDesc.hpp"
#include "../Stage/EntityHandle.hpp"
#include "../Stage/LightEntity.hpp"
#include "../Stage/BE_ScriptedCamera.hpp"
#include "../Stage/EntityRenderManager.hpp"
#include "../Stage/ScreenEffectManager.hpp"
#include "../Stage/BaseEntity_Draw.hpp" // needed to create instance of CEntityShaderLightParamsLoader
#include "../Stage/StageUtility.hpp"
#include "../Support/Macro.h"
#include "../Support/Vec3_StringAux.hpp"
#include "../Support/Log/DefaultLog.hpp"


namespace amorphous
{

using std::vector;


void SetShaderToEntity( EntityHandle<>& entity, ShaderHandle& shader, ShaderTechniqueHandle& tech )
{
	shared_ptr<CCopyEntity> pEntity = entity.Get();
	if( !pEntity )
		return;

	if( !pEntity->m_pMeshRenderMethod )
	{
		pEntity->m_pMeshRenderMethod.reset( new MeshContainerRenderMethod );
	}

//	vector<SubsetRenderMethod>& render_methods
//		= pEntity->m_pMeshRenderMethod->m_vecMeshRenderMethod;

//	if( render_methods.empty() )
//		render_methods.resize( 1 );

	SubsetRenderMethod& render_method = pEntity->m_pMeshRenderMethod->PrimaryMeshRenderMethod();

	render_method.m_Shader = shader;
	render_method.m_Technique = tech;

	if( pEntity->GetEntityFlags() & BETYPE_LIGHTING )
	{
		vector< shared_ptr<ShaderParamsLoader> >& params_loaders =
			render_method.m_vecpShaderParamsLoader;

		int params_loader_index = -1;
		for( size_t i=0; i<params_loaders.size(); i++ )
		{
			if( params_loaders[i]
			&& typeid(*params_loaders[i]) == typeid(CEntityShaderLightParamsLoader) )
			{
				params_loader_index = (int)i;
				break;
			}
		}

		if( params_loader_index == -1 )
//		if( params_loaders.empty() )
		{
			// Params loader for lighting was not found - add to the list of params loaders
			params_loaders.push_back( shared_ptr<ShaderParamsLoader>() );
			params_loaders.back()
				= shared_ptr<ShaderParamsLoader>( new CEntityShaderLightParamsLoader(pEntity) );
//			params_loader_index = (int)params_loaders.size() - 1;
		}
	}
}


BOOST_PYTHON_MODULE(stage)
{
	using namespace boost::python;
	namespace py = boost::python;

	class_< EntityHandle<> >("EntityHandle")
		.def( "IsAlive",          &EntityHandle<>::IsAlive )
		.def( "GetWorldPosition", &EntityHandle<>::GetWorldPosition )
		.def( "GetWorldPose",     &EntityHandle<>::GetWorldPose )
		.def( "SetWorldPosition", &EntityHandle<>::SetWorldPosition )
		.def( "SetWorldPose",     &EntityHandle<>::SetWorldPose )
	;

//	class_< LightEntityHandle >("LightEntityHandle")
	class_< LightEntityHandle, bases< EntityHandle<> > >("LightEntityHandle") /// Compiled on vc8, but is this REALLY okay on any platform?
		.def( "SetUpperColor",    &LightEntityHandle::SetUpperColor )
		.def( "SetLowerColor",    &LightEntityHandle::SetLowerColor )
	;

	class_< ScriptedCameraEntityHandle >("ScriptedCameraEntityHandle")
		.def( "SetUniformMotionBlur",     &ScriptedCameraEntityHandle::SetUniformMotionBlur )
		.def( "SetUniformBlur",           &ScriptedCameraEntityHandle::SetUniformBlur )
		.def( "SetUniformCameraShake",    &ScriptedCameraEntityHandle::SetUniformCameraShake )
	;

//	class_< CStaticGeometryEntityHandle >("StaticGeometryEntityHandle")
//		.def( "GetStaticGeometry",        &CStaticGeometryEntityHandle::GetStaticGeometry )
//	;

	def( "SetShaderToEntity",  SetShaderToEntity, ( py::arg("entity"), py::arg("shader"), py::arg("tech") ) );

	def( "SetFloatShaderParamToEntity",  SetFloatShaderParamToEntity, ( py::arg("entity"), py::arg("param_name"), py::arg("value") ) );
//	def( "SetIntShaderParamToEntity",    SetIntShaderParamToEntity,   ( py::arg("entity"), py::arg("param_name"), py::arg("value") ) );
//	def( "SetVec3ShaderParamToEntity",   SetVec3ShaderParamToEntity,  ( py::arg("entity"), py::arg("param_name"), py::arg("value") ) );
	def( "SetColorShaderParamToEntity",  SetColorShaderParamToEntity, ( py::arg("entity"), py::arg("param_name"), py::arg("value") ) );
	def( "SetTextureShaderParamToEntity",SetTextureShaderParamToEntity, ( py::arg("entity"), py::arg("param_name"), py::arg("tex_path") ) );

//		.def()

//	def( "GetStage",  GetStage, ( py::arg("stage_name") = "" ) );

/*
	// error C2248: 'CStage::CStage' : cannot access private member declared in class 'CStage'
	class_< CStage, std::shared_ptr<CStage> >("Stage")
//		.def( "GetEntityByName",          &CStage::GetEntityByName )
		.def( "GetScreenEffectManager",   &CStage::GetScreenEffectManager )
	;*/
}


void RegisterPythonModule_stage()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("stage", initstage) == -1)
	{
		const char *msg = "Failed to add 'stage' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}


} // namespace amorphous
