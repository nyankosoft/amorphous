#include <gds/Script/PyModule_stage.hpp>
#include <boost/python.hpp>

#include <gds/base.hpp>
//#include <gds/3DMath/Matrix34.hpp>
#include <gds/Stage/Stage.hpp>
#include <gds/Stage/BE_PointLight.hpp>
#include <gds/Stage/CopyEntityDesc.hpp>
#include <gds/Stage/EntityHandle.hpp>
#include <gds/Stage/LightEntity.hpp>
#include <gds/Stage/BE_ScriptedCamera.hpp>
#include <gds/Stage/EntityRenderManager.hpp>
#include <gds/Stage/ScreenEffectManager.hpp>
#include <gds/Support/Macro.h>
#include <gds/Support/Vec3_StringAux.hpp>
#include <gds/Support/Log/DefaultLog.hpp>


using namespace std;
using namespace boost;


BOOST_PYTHON_MODULE(stage)
{
	using namespace boost::python;
	namespace py = boost::python;

	class_< CLightEntityHandle >("LightEntityHandle")
		.def( "SetUpperColor",    &CLightEntityHandle::SetUpperColor )
		.def( "SetLowerColor",    &CLightEntityHandle::SetLowerColor )
	;

	class_< CScriptedCameraEntityHandle >("ScriptedCameraEntityHandle")
		.def( "SetUniformMotionBlur",     &CScriptedCameraEntityHandle::SetUniformMotionBlur )
		.def( "SetUniformBlur",           &CScriptedCameraEntityHandle::SetUniformBlur )
		.def( "SetUniformCameraShake",    &CScriptedCameraEntityHandle::SetUniformCameraShake )
	;

	class_< CEntityHandle<> >("EntityHandle")
		.def( "IsAlive",          &CEntityHandle<>::IsAlive )
		.def( "GetWorldPosition", &CEntityHandle<>::GetWorldPosition )
		.def( "GetWorldPose",     &CEntityHandle<>::GetWorldPose )
		.def( "SetWorldPosition", &CEntityHandle<>::SetWorldPosition )
		.def( "SetWorldPose",     &CEntityHandle<>::SetWorldPose )
	;
//		.def()

//	class_< CStage, boost::shared_ptr<CStage> >("Stage")
//		.def( "GetEntityByName",   &CStage::GetEntityByName )
//	;
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
