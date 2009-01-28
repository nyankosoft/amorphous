#ifndef __StageUtility_H__
#define __StageUtility_H__


#include "Stage/Stage.h"
#include "Stage/EntityHandle.h"
#include "Stage/BE_ScriptedCamera.h"


class CStageUtility
{
	boost::weak_ptr<CStage> m_pStage;

public:

	CStageUtility( boost::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage)
	{}

	/// \param cutscene_input_handler_index input index of the handler during cutscene.
	///        Set a navative value to create camera controller not for cutscene.
	///        default: -1 (Don't create the camera controller for cutscene)
	CEntityHandle<> CreateCameraController( const std::string& camera_controller_name,
		                                    int cutscene_input_handler_index = -1 );

	CScriptedCameraEntity *CreateScriptedCamera( const std::string& camera_name,
		                                         const std::string& camera_controller_name,
												 CameraParam default_camera_param = CameraParam() );
};


#endif /* __StageUtility_H__ */
