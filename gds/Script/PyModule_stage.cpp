#include <gds/Script/PyModule_stage.hpp>
#include <boost/python.hpp>

#include <gds/base.hpp>
#include <gds/Graphics/ShaderHandle.hpp>
#include <gds/Graphics/Shader/ShaderTechniqueHandle.hpp>
#include <gds/Stage/Stage.hpp>
#include <gds/Stage/BE_PointLight.hpp>
#include <gds/Stage/CopyEntityDesc.hpp>
#include <gds/Stage/EntityHandle.hpp>
#include <gds/Stage/LightEntity.hpp>
#include <gds/Stage/BE_ScriptedCamera.hpp>
#include <gds/Stage/EntityRenderManager.hpp>
#include <gds/Stage/ScreenEffectManager.hpp>
#include <gds/Stage/BaseEntity_Draw.hpp> // needed to create instance of CEntityShaderLightParamsLoader
#include <gds/Stage/StageUtility.hpp>
#include <gds/Support/Macro.h>
#include <gds/Support/Vec3_StringAux.hpp>
#include <gds/Support/Log/DefaultLog.hpp>


namespace amorphous
{

using std::vector;
using namespace boost;


void SetShaderToEntity( CEntityHandle<>& entity, CShaderHandle& shader, CShaderTechniqueHandle& tech )
{
	shared_ptr<CCopyEntity> pEntity = entity.Get();
	if( !pEntity )
		return;

	if( !pEntity->m_pMeshRenderMethod )
	{
		pEntity->m_pMeshRenderMethod.reset( new CMeshContainerRenderMethod );
	}

//	vector<CSubsetRenderMethod>& render_methods
//		= pEntity->m_pMeshRenderMethod->m_vecMeshRenderMethod;

//	if( render_methods.empty() )
//		render_methods.resize( 1 );

	CSubsetRenderMethod& render_method = pEntity->m_pMeshRenderMethod->PrimaryMeshRenderMethod();

	render_method.m_Shader = shader;
	render_method.m_Technique = tech;

	if( pEntity->GetEntityFlags() & BETYPE_LIGHTING )
	{
		vector< shared_ptr<CShaderParamsLoader> >& params_loaders =
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
			params_loaders.push_back( shared_ptr<CShaderParamsLoader>() );
			params_loaders.back()
				= shared_ptr<CShaderParamsLoader>( new CEntityShaderLightParamsLoader(pEntity) );
//			params_loader_index = (int)params_loaders.size() - 1;
		}
	}
}


BOOST_PYTHON_MODULE(stage)
{
	using namespace boost::python;
	namespace py = boost::python;

	class_< CEntityHandle<> >("EntityHandle")
		.def( "IsAlive",          &CEntityHandle<>::IsAlive )
		.def( "GetWorldPosition", &CEntityHandle<>::GetWorldPosition )
		.def( "GetWorldPose",     &CEntityHandle<>::GetWorldPose )
		.def( "SetWorldPosition", &CEntityHandle<>::SetWorldPosition )
		.def( "SetWorldPose",     &CEntityHandle<>::SetWorldPose )
	;

//	class_< CLightEntityHandle >("LightEntityHandle")
	class_< CLightEntityHandle, bases< CEntityHandle<> > >("LightEntityHandle") /// Compiled on vc8, but is this REALLY okay on any platform?
		.def( "SetUpperColor",    &CLightEntityHandle::SetUpperColor )
		.def( "SetLowerColor",    &CLightEntityHandle::SetLowerColor )
	;

	class_< CScriptedCameraEntityHandle >("ScriptedCameraEntityHandle")
		.def( "SetUniformMotionBlur",     &CScriptedCameraEntityHandle::SetUniformMotionBlur )
		.def( "SetUniformBlur",           &CScriptedCameraEntityHandle::SetUniformBlur )
		.def( "SetUniformCameraShake",    &CScriptedCameraEntityHandle::SetUniformCameraShake )
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
	class_< CStage, boost::shared_ptr<CStage> >("Stage")
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
