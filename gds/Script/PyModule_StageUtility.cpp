#include <gds/Script/PyModule_StageUtility.hpp>
#include <gds/Graphics/3DGameMath.hpp>
#include <gds/Stage/EntityMotionPathRequest.hpp>
#include <gds/Support/Log/DefaultLog.hpp>

using namespace std;
using namespace boost;

using namespace physics;

namespace stage_util
{


BOOST_PYTHON_MODULE(stage_util)
{
	using namespace boost;
	using namespace boost::python;
	namespace py = boost::python;

	class_< CStageLightUtility, shared_ptr<CStageLightUtility> >("StageLightUtility")
		.def( "CreateHSPointLight",        &CStageLightUtility::CreateHSPointLight )
		.def( "CreateHSDirectionalLight",  &CStageLightUtility::CreateHSDirectionalLight )
//		.def( "CreateTriPointLight",       &CStageLightUtility::CreateTriPointLight ) // NOT IMPLEMENTED
//		.def( "CreateTriirectionalLight",  &CStageLightUtility::CreateTriirectionalLight ) // NOT IMPLEMENTED
	;

	class_< CStageMiscUtility, shared_ptr<CStageMiscUtility> >("StageMiscUtility")
//		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( py::arg("pos"), python::arg("edge_lengths"), python::arg("diffuse_color") ) )
		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateStaticBox",    &CStageMiscUtility::CreateStaticBox,   ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose")=Matrix34Identity(),                       py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateBoxFromMesh",  &CStageMiscUtility::CreateBoxFromMesh, ( py::arg("mesh_resource_path"),                                                                 py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
	;

	class_< CStageEntityUtility, shared_ptr<CStageEntityUtility> >("StageEntityUtility")
		.def( "SetShader",          &CStageEntityUtility::SetShader,         ( py::arg("entity"), py::arg("shader_name"), py::arg("subset_name")="" ) )
	;

	def( "CreateStageLightUtility",   CreateStageLightUtility );
	def( "CreateStageMiscUtility",    CreateStageMiscUtility );
	def( "CreateStageEntityUtility",  CreateStageEntityUtility );

	class_<KeyPose>("KeyPose")
		.def_readwrite( "time", &KeyPose::time )
		.def_readwrite( "pose", &KeyPose::pose )
		.def(init<float,Matrix34>())
	;

	class_< CBEC_MotionPath, shared_ptr<CBEC_MotionPath> >("EntityMotionPath")
		.def( "AddKeyPose", &CBEC_MotionPath::AddKeyPose )
	;
}


void RegisterPythonModule_stage_util()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("stage_util", initstage_util) == -1)
	{
		const char *msg = "Failed to add stage_util to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}


} // stage_util
