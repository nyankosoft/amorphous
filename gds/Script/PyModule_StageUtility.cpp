#include <gds/Script/PyModule_StageUtility.hpp>
#include <gds/Graphics/3DGameMath.hpp>
#include <gds/Support/Log/DefaultLog.hpp>

using namespace std;
using namespace boost;

using namespace physics;
using namespace stage_util;


BOOST_PYTHON_MODULE(stage_util)
{
	using namespace boost;
	using namespace boost::python;

	class_< CStageLightUtility, shared_ptr<CStageLightUtility> >("StageLightUtility")
		.def( "CreateHSPointLight",        &CStageLightUtility::CreateHSPointLight )
		.def( "CreateHSDirectionalLight",  &CStageLightUtility::CreateHSDirectionalLight )
//		.def( "CreateTriPointLight",       &CStageLightUtility::CreateTriPointLight ) // NOT IMPLEMENTED
//		.def( "CreateTriirectionalLight",  &CStageLightUtility::CreateTriirectionalLight ) // NOT IMPLEMENTED
	;

	class_< CStageMiscUtility, shared_ptr<CStageMiscUtility> >("StageMiscUtility")
//		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( python::arg("pos"), python::arg("edge_lengths"), python::arg("diffuse_color") ) )
		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( python::arg("edge_lengths") = Vector3(1,1,1), python::arg("diffuse_color") = SFloatRGBAColor::White(), python::arg("pose")=Matrix34Identity(), python::arg("mass")=1.0f, python::arg("entity_name")="", python::arg("entity_attributes_name")="" ) )
		.def( "CreateBoxFromMesh",  &CStageMiscUtility::CreateBoxFromMesh, ( python::arg("mesh_resource_path"),                                                                     python::arg("pose")=Matrix34Identity(), python::arg("mass")=1.0f, python::arg("entity_name")="", python::arg("entity_attributes_name")="" ) )
	;

	def( "CreateStageLightUtility", CreateStageLightUtility );
	def( "CreateStageMiscUtility",  CreateStageMiscUtility );

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
