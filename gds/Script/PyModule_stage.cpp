#include "PyModule_stage.hpp"
#include <boost/python.hpp>

#include "3DMath/Matrix34.hpp"
#include "Stage/Stage.hpp"
#include "Stage/BE_PointLight.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/EntityHandle.hpp"
#include "Stage/LightEntity.hpp"
#include "Stage/EntityRenderManager.hpp"
#include "Stage/ScreenEffectManager.hpp"

#include "Support/Macro.h"
#include "Support/Vec3_StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"

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
