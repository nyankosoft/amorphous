#ifndef __StageUtility_HPP__
#define __StageUtility_HPP__


#include "Stage/Stage.hpp"
#include "Stage/EntityHandle.hpp"
#include "Stage/BE_ScriptedCamera.hpp"
#include "Physics/fwd.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "3DMath/Matrix34.hpp"
#include "3DMath/AABB3.hpp"


class CStageUtility
{
protected:

	boost::weak_ptr<CStage> m_pStage;

public:

	CStageUtility() {}

	virtual ~CStageUtility() {}

	CStageUtility( boost::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage)
	{}

	CEntityHandle<> CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								physics::CActorDesc *pPhysActorDesc );
};


class CStageCameraUtility : public CStageUtility
{
public:

	CStageCameraUtility() {}

	CStageCameraUtility( boost::weak_ptr<CStage> pStage )
		:
	CStageUtility(pStage)
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


class CStageLightUtility : public CStageUtility
{
//...

public:

	/// default ctor. Added to compile this code with boost::python.
	CStageLightUtility() {}

	CStageLightUtility( boost::shared_ptr<CStage> pStage )
		:
	CStageUtility(pStage)
	{}

	void CreateHSPointLight( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 );

	void CreateHSDirectionalLight( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir );
};


class CStageMiscUtility : public CStageUtility
{
	CEntityHandle<> CStageMiscUtility::CreateBoxEntity( CMeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  float mass );

public:

	/// default ctor. Added to compile this code with boost::python.
	CStageMiscUtility() {}

	CStageMiscUtility( boost::shared_ptr<CStage> pStage )
		:
	CStageUtility(pStage)
	{}

	CEntityHandle<> CreateBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		float mass = 1.0f,
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	CEntityHandle<> CreateBoxFromMesh( const char *mesh_resource_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );
};



#endif /* __StageUtility_HPP__ */
