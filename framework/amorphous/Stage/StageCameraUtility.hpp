#ifndef __StageCameraUtility_HPP__
#define __StageCameraUtility_HPP__


#include "StageUtility.hpp"
#include "BE_ScriptedCamera.hpp"


namespace amorphous
{


class StageCameraUtility : public StageUtility
{
public:

	StageCameraUtility() {}

	StageCameraUtility( boost::weak_ptr<CStage> pStage )
		:
	StageUtility(pStage)
	{}

	/// \param cutscene_input_handler_index input index of the handler during cutscene.
	///        Set a navative value to create camera controller not for cutscene.
	///        default: -1 (Don't create the camera controller for cutscene)
	EntityHandle<> CreateCameraController( const std::string& camera_controller_name,
		                                    int cutscene_input_handler_index = -1 );

	ScriptedCameraEntity *CreateScriptedCamera( const std::string& camera_name,
		                                         const std::string& camera_controller_name,
												 CameraParam default_camera_param = CameraParam() );
};


} // namespace amorphous


#endif /* __StageCameraUtility_HPP__ */
