#include "amorphous/Script/PyModule_StageUtility.hpp"
#include "amorphous/Stage/EntityMotionPathRequest.hpp"
#include "amorphous/Stage/BE_ScriptedCamera.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using std::shared_ptr;
using namespace physics;


EntityHandle<> (StageMiscUtility::*CreateBox_Pose)  ( Vector3 edge_lengths, SFloatRGBAColor diffuse_color, const Matrix34& pose,                                       float mass, const std::string& material_name, const std::string& entity_name, const std::string& entity_attributes_name ) = &StageMiscUtility::CreateBox;
EntityHandle<> (StageMiscUtility::*CreateBox_PosHPB)( Vector3 edge_lengths, SFloatRGBAColor diffuse_color, const Vector3& pos, float heading, float pitch, float bank, float mass, const std::string& material_name, const std::string& entity_name, const std::string& entity_attributes_name ) = &StageMiscUtility::CreateBox;
Result::Name (StageMiscUtility::*GlueEntitiesEH)  ( EntityHandle<>& entity0, EntityHandle<>& entity1, float max_force, float max_torque ) = &StageMiscUtility::GlueEntities;
Result::Name (StageMiscUtility::*GlueEntitiesEN)  ( const char *entity0_name, const char *entity1_name, float max_force, float max_torque ) = &StageMiscUtility::GlueEntities;


namespace stage_util
{

//CameraParam sg_DefaultCameraParam;


BOOST_PYTHON_MODULE(stage_util)
{
	using namespace boost;
	using namespace boost::python;
	namespace py = boost::python;

	class_< StageCameraUtility/*, shared_ptr<StageCameraUtility>*/ >("StageCameraUtility")
		.def( "CreateCameraController",     &StageCameraUtility::CreateCameraController,     ( py::arg("camera_controller_name") = "", py::arg("cutscene_input_handler_index") = -1 ) )
//		.def( "CreateScriptedCamera",       &StageCameraUtility::CreateScriptedCamera,       ( py::arg("camera_name") = "", py::arg("camera_controller_name") = "", py::arg("default_camera_param") = sg_DefaultCameraParam ) )
	;

	class_< StageLightUtility/*, shared_ptr<StageLightUtility>*/ >("StageLightUtility")
		.def( "CreateHSPointLightEntity",        &StageLightUtility::CreateHSPointLightEntity,       ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("pos") = Vector3(0,0,0), py::arg("attenu0") = 1.0f, py::arg("attenu1") = 1.0f, py::arg("attenu2") = 1.0f ) )
		.def( "CreateHSDirectionalLightEntity",  &StageLightUtility::CreateHSDirectionalLightEntity, ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("dir") = Vector3(0,-1,0) ) )
		.def( "CreateHSSpotlightEntity",         &StageLightUtility::CreateHSSpotlightEntity,        ( py::arg("name") = "", py::arg("upper_color") = SFloatRGBAColor::White(), py::arg("lower_color") = SFloatRGBAColor::Black(), py::arg("intensity") = 1.0f, py::arg("dir") = Vector3(0,-1,0), py::arg("attenu0") = 1.0f, py::arg("attenu1") = 1.0f, py::arg("attenu2") = 1.0f, py::arg("inner_cone_angle") = 1.0f, py::arg("outer_cone_angle") = 1.0f ) )
//		.def( "CreateTriPointLight",             &StageLightUtility::CreateTriPointLight )       // NOT IMPLEMENTED
//		.def( "CreateTriDirectionalLight",       &StageLightUtility::CreateTriDirectionalLight ) // NOT IMPLEMENTED
//		.def( "CreateTriSpotlightEntity",        &StageLightUtility::CreateTriSpotlightEntity )  // NOT IMPLEMENTED
	;

	class_< StageMiscUtility/*, shared_ptr<StageMiscUtility>*/ >("StageMiscUtility")
//		.def( "CreateBox",               &StageMiscUtility::CreateBox,              ( py::arg("pos"), python::arg("edge_lengths"), python::arg("diffuse_color") ) )
		.def( "CreateBox",               CreateBox_Pose,                             ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose"),                                                                                          py::arg("mass")=1.0f, py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateBox",               CreateBox_PosHPB,                           ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("position")=Vector3(0,0,0), py::arg("heading")=0.0f, py::arg("pitch")=0.0f, py::arg("bank")=0.0f, py::arg("mass")=1.0f, py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateStaticBox",         &StageMiscUtility::CreateStaticBox,        ( py::arg("edge_lengths") = Vector3(1,1,1), py::arg("diffuse_color") = SFloatRGBAColor::White(), py::arg("pose")=Matrix34Identity(),                                                                                             py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
//		.def( "CreateCylinderFromMesh",  &StageMiscUtility::CreateCylinderFromMesh, ( py::arg("model"), py::arg("name")="", py::arg("position")=Vector3(0,0,0), py::arg("heading")=0.0f, py::arg("pitch")=0.0f, py::arg("bank")=0.0f, py::arg("mass")=1.0f, py::arg("material_name")="default" ) )
		.def( "CreateBoxFromMesh",       &StageMiscUtility::CreateBoxFromMesh,      ( py::arg("mesh_path"),                                                                          py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("material_name")="default", py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateTriangleMeshFromMesh",        &StageMiscUtility::CreateTriangleMeshFromMesh,       ( py::arg("mesh_path"), py::arg("collision_mesh_path")="",         py::arg("pose")=Matrix34Identity(), py::arg("mass")=1.0f, py::arg("material_name")="",        py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateStaticTriangleMeshFromMesh",  &StageMiscUtility::CreateStaticTriangleMeshFromMesh, ( py::arg("mesh_path"), py::arg("collision_mesh_path")="",         py::arg("pose")=Matrix34Identity(),                       py::arg("material_name")="",        py::arg("entity_name")="", py::arg("entity_attributes_name")="" ) )
		.def( "CreateEntity",                      &StageMiscUtility::CreateEntity, ( py::arg("model"), py::arg("name")="", py::arg("position")=Vector3(0,0,0), py::arg("heading")=0.0f, py::arg("pitch")=0.0f, py::arg("bank")=0.0f, py::arg("mass")=1.0f, py::arg("shape")="", py::arg("is_static")=0 ) )
		.def( "CreateSkybox",                      &StageMiscUtility::CreateSkybox, ( py::arg("mesh_path")="", py::arg("texture_path")="" ) )
		.def( "CreateStaticGeometry",              &StageMiscUtility::CreateStaticGeometry, ( py::arg("resource_path") ) )
		.def( "CreateStaticWater",                 &StageMiscUtility::CreateStaticWater,    ( py::arg("model"), py::arg("name")="", py::arg("position") = Vector3(0,0,0) ) )
		.def( "FixEntity",                         &StageMiscUtility::FixEntity,            ( py::arg("entity"), py::arg("max_force") = FLT_MAX, py::arg("max_torque") = FLT_MAX ) )
		.def( "GlueEntities",                      GlueEntitiesEH,                          ( py::arg("entity0"), py::arg("entity1"), py::arg("max_force") = FLT_MAX, py::arg("max_torque") = FLT_MAX ) )
		.def( "GlueEntities",                      GlueEntitiesEN,                          ( py::arg("entity0_name"), py::arg("entity1_name"), py::arg("max_force") = FLT_MAX, py::arg("max_torque") = FLT_MAX ) )
	;

	class_< StageEntityUtility/*, shared_ptr<StageEntityUtility>*/ >("StageEntityUtility")
		.def( "SetShader",          &StageEntityUtility::SetShader,         ( py::arg("entity"), py::arg("shader"), py::arg("technique")="Default", py::arg("subset")="" ) )
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


} // namespace amorphous
