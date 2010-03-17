#ifndef __StageUtility_HPP__
#define __StageUtility_HPP__


#include "Stage/Stage.hpp"
#include "Stage/EntityHandle.hpp"
#include "Physics/fwd.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "3DMath/Matrix34.hpp"
#include "3DMath/AABB3.hpp"

// for CStageCameraUtility
#include "Stage/BE_ScriptedCamera.hpp"

// for CStageLightUtility
#include "Stage/LightEntity.hpp"


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

	CLightEntityHandle CreateHSPointLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 );

	CLightEntityHandle CreateHSDirectionalLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir );
};


class CStageMiscUtility : public CStageUtility
{
	CEntityHandle<> CreatePhysicsEntity( CMeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  std::vector<physics::CShapeDesc *>& vecpShapeDesc,
								  float mass,
								  bool static_actor );

	CEntityHandle<> CreateBoxEntity( CMeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  float mass,
								  const std::string& material_name,
								  bool static_actor );

	CEntityHandle<> CreateTriangleMeshEntityFromMesh( const char *mesh_resource_name,
							const char *collision_mesh_name,
							const Matrix34& pose,
							float mass,
							const std::string& material_name,
							const std::string& entity_name,
							const std::string& entity_attributes_name,
							bool static_actor );

	Result::Name SetTriangleMeshShapeDesc( const char *collision_mesh_name,
		physics::CTriangleMeshShapeDesc& trimeshshapedesc );
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
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	CEntityHandle<> CreateStaticBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	CEntityHandle<> CreateBoxFromMesh( const char *mesh_resource_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );
/*
	CEntityHandle<> CreateBall( float radius,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		float mass = 1.0f,
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	CEntityHandle<> CreateStaticBall( float radius,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	CEntityHandle<> CreateBallFromMesh( const char *mesh_resource_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );
*/
	/// Creates a static triangle mesh actor from a graphics mesh file
	CEntityHandle<> CreateStaticTriangleMeshFromMesh( const char *mesh_resource_path,
		                    const char *collision_mesh_name,
							const Matrix34& pose = Matrix34Identity(),
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	/// Creates a triangle mesh actor from a graphics mesh file
	CEntityHandle<> CreateTriangleMeshFromMesh( const char *mesh_resource_path,
		                    const char *collision_mesh_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	void CreateSkybox( const std::string& mesh_resource_path, const std::string& texture_resource_path = "BuiltinTexture::ClearSkyGrad" );

//	void CreateSkysphere( const std::string& texture_resource_path = "BuiltinTexture::ClearSkyGrad" );

	void CreateStaticGeometry( const std::string& resource_path );

	CEntityHandle<> CreateStaticSmokeSource( const Vector3& pos,
			const SFloatRGBAColor& color, float diameter, float rise_speed, float thickness, float density,
			const std::string& entity_attributes_name = "" );

//	void CreateSmokeTrailSource( const SFloatRGBAColor& color, const string& target_entity_name );
};


class CStageEntityUtility : public CStageUtility
{
public:

	CStageEntityUtility() {}

	CStageEntityUtility( boost::shared_ptr<CStage> pStage )
		:
	CStageUtility(pStage)
	{}

	Result::Name SetShader( CEntityHandle<>& entity, const std::string& shader_name, const std::string& subset_name, int lod = 0 );

	Result::Name RemoveAllShaders( CEntityHandle<>& entity );
};


/*
class CStageEffectUtility : public CStageUtility
{
public:

	/// default ctor. Added to compile this code with boost::python.
	CStageEffectUtility() {}

	CStageEffectUtility( boost::shared_ptr<CStage> pStage )
		:
	CStageUtility(pStage)

	void Rain( float fall_speed ) {}

	void Snow( float fall_speed ) {}

	void SetWind( const AABB3& world_aabb, const Vector3 vForce ) {}

	void AddLensFlareElement( CLightEntityHandle directional_light_entity, const std::string& texture_filepath, float size, float dist ) {}
};
*/


extern void SetShaderFloatParamToEntity( CEntityHandle<> entity, const char *parameter_name, float value );
extern void SetShaderColorParamToEntity( CEntityHandle<> entity, const char *parameter_name, const SFloatRGBAColor& value );
extern void SetShaderTextureParamToEntity( CEntityHandle<> entity, const char *parameter_name, const char *tex_path );


#endif /* __StageUtility_HPP__ */
