#include "gds/Script/PyModule_StageUtility.hpp"
#include "gds/Graphics/3DGameMath.hpp"
#include "gds/Stage/EntityMotionPathRequest.hpp"
#include "gds/Stage/BE_ScriptedCamera.hpp"
#include "gds/Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;

using namespace physics;

namespace stage_util
{

//CameraParam sg_DefaultCameraParam;


BOOST_PYTHON_MODULE(stage_util)
{
	using namespace boost;
	using namespace boost::python;
	namespace py = boost::python;

	class_< CStageCameraUtility, shared_ptr<CStageCameraUtility> >("StageCameraUtility")
		.def( "CreateCameraController",     &CStageCameraUtility::CreateCameraController,     ( py::arg("camera_controller_name") = "", py::arg("cutscene_input_handler_index") = -1 ) )
//		.def( "CreateScriptedCamera",       &CStageCameraUtility::CreateScriptedCamera,       ( py::arg("camera_name") = "", py::arg("camera_controller_name") = "", py::arg("default_camera_param") = sg_DefaultCameraParam ) )
	;

	class_< CStageLightUtility, shared_ptr<CStageLightUtility> >("StageLightUtility")
		.def( "CreateHSPointLightEntity",        &CStageLightUtility::CreateHSPointLightEntity,       ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("pos") = Vector3(0,0,0), py::arg("attenu0") = 1.0f, py::arg("attenu1") = 1.0f, py::arg("attenu2") = 1.0f ) )
		.def( "CreateHSDirectionalLightEntity",  &CStageLightUtility::CreateHSDirectionalLightEntity, ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("dir") = Vector3(0,-1,0) ) )
		.def( "CreateHSSpotlightEntity",         &CStageLightUtility::CreateHSSpotlightEntity,        ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("dir") = Vector3(0,-1,0), py::arg("attenu0") = 1.0f, py::arg("attenu1") = 1.0f, py::arg("attenu2") = 1.0f, py::arg("inner_cone_angle") = 1.0f, py::arg("outer_cone_angle") = 1.0f ) )
//		.def( "CreateTriPointLight",             &CStageLightUtility::CreateTriPointLight )       // NOT IMPLEMENTED
//		.def( "CreateTriDirectionalLight",       &CStageLightUtility::CreateTriDirectionalLight ) // NOT IMPLEMENTED
//		.def( "CreateTriSpotlightEntity",        &CStageLightUtility::CreateTriSpotlightEntity )  // NOT IMPLEMENTED
	;

	class_< CStageMiscUtility, shared_ptr<CStageMiscUtility> >("StageMiscUtility")
//		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( py::arg("pos"), python::arg("edge_lengths"), python::arg("diffuse_color") ) )
		.def( "CreateBox",          &CStageMiscUtility::CreateBox,         ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateStaticBox",    &CStageMiscUtility::CreateStaticBox,   ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose")=Matrix34Identity(),                       py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateBoxFromMesh",  &CStageMiscUtility::CreateBoxFromMesh, ( py::arg("mesh_path"),                                                                          py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateTriangleMeshFromMesh",        &CStageMiscUtility::CreateTriangleMeshFromMesh,       ( py::arg("mesh_path"), py::arg("collision_mesh_path")="",         py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("material_name")="",        py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateStaticTriangleMeshFromMesh",  &CStageMiscUtility::CreateStaticTriangleMeshFromMesh, ( py::arg("mesh_path"), py::arg("collision_mesh_path")="",         py::arg("pose")=Matrix34Identity(),                       py::arg("material_name")="",        py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateEntity",                      &CStageMiscUtility::CreateEntity, ( py::arg("model"), py::arg("name")="", py::arg("position")=Vector3(0,0,0),               py::arg("heading")=0.0f, py::arg("pitch")=0.0f, py::arg("bank")=0.0f, py::arg("mass")=1.0f, py::arg("shape")="", py::arg("is_static")=0 ) )
		.def( "CreateSkybox",                      &CStageMiscUtility::CreateSkybox, ( py::arg("mesh_path")="", py::arg("texture_path")="" ) )
		.def( "CreateStaticGeometry",              &CStageMiscUtility::CreateStaticGeometry, ( py::arg("resource_path") ) )
	;

	class_< CStageEntityUtility, shared_ptr<CStageEntityUtility> >("StageEntityUtility")
		.def( "SetShader",          &CStageEntityUtility::SetShader,         ( py::arg("entity"), py::arg("shader_name"), py::arg("subset_name")="" ) )
	;

	def( "CreateStageCameraUtility",  CreateStageCameraUtility );
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
