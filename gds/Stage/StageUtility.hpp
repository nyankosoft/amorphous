#ifndef __StageUtility_HPP__
#define __StageUtility_HPP__


#include "gds/Stage/Stage.hpp"
#include "gds/Stage/EntityHandle.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/Graphics/FloatRGBAColor.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/3DMath/AABB3.hpp"

// for CStageCameraUtility
#include "gds/Stage/BE_ScriptedCamera.hpp"

// for CStageLightUtility
#include "gds/Stage/LightEntity.hpp"


namespace amorphous
{


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

	CEntityHandle<> CStageUtility::CreateNamedEntity( CCopyEntityDesc& desc,
									const std::string& base_name );

	CEntityHandle<> CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								physics::CActorDesc *pPhysActorDesc,
								CMeshObjectHandle& mesh );
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

	CLightEntityHandle CreateHSSpotlightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& pos, const Vector3& dir,
		float attenu0, float attenu1, float attenu2,
		float inner_cone_angle = (float)PI * 0.25f, float outer_cone_angle = -1.0f );
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

	CEntityHandle<> CreateCylinderEntity( CMeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor );

	CEntityHandle<> CreateSphereEntity( CMeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  float mass,
								  const std::string& material_name,
								  bool static_actor );

	CEntityHandle<> CreateBoxesEntity( CMeshResourceDesc& mesh_desc,
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

	CEntityHandle<> CreateEntityFromBaseEntity( const char *model,
		const char *name,
		const Matrix34& pose );

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

	CEntityHandle<> CreateBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color,
		const Vector3& pos,
		const float heading = 0.0f,
		const float pitch = 0.0f,
		const float bank = 0.0f,
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

	CEntityHandle<> CreateSphere( float diameter = 1.0f,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		float mass = 1.0f,
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );
/*
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


	CEntityHandle<> CreateCylinderFromMesh( const char *model,
							const char *name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default" );

	CEntityHandle<> CreateCylinderFromMesh( const char *model,
						const char *name,
						const Vector3& position = Vector3(0,0,0),
						float heading = 0.0f,
						float pitch = 0.0f,
						float bank = 0.0f,
						float mass = 1.0f,
						const std::string& material_name = "default" );

	CEntityHandle<> CreateStaticCylinderFromMesh( const char *model,
								const char *name,
								const Matrix34& pose = Matrix34Identity(),
								float mass = 1.0f,
								const std::string& material_name = "default" )
	{
		CMeshResourceDesc mesh_desc;
		mesh_desc.ResourcePath = model;
		Vector3 vel( Vector3(0,0,0) );
//		std::string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__CylinderFromMesh__";
		std::string actual_entity_attributes_name = "__CylinderFromMesh__";

		return CreateCylinderEntity( mesh_desc, name, actual_entity_attributes_name, pose, vel, mass, material_name, true );
	}

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

	CEntityHandle<> CreateEntity(
		const char *model,
		const char *name = "",
		const Vector3& position = Vector3(0,0,0),
		const float heading = 0,
		const float pitch   = 0,
		const float bank    = 0,
		const float mass = 1.0f,
		const char *shape = "",
		bool is_static = false);

	Result::Name GlueEntities(
		const char *entity0_name,
		const char *entity1_name,
		float max_force  = FLT_MAX,
		float max_torque = FLT_MAX );

	Result::Name ConnectEntitiesWithRevoluteJoint(
		const char *entity0_name,
		const char *entity1_name,
		const Vector3& axis,
		const Vector3& anchor,
		float max_force  = FLT_MAX,
		float max_torque = FLT_MAX );

	void CreateSkybox( const std::string& mesh_resource_path, const std::string& texture_resource_path = "BuiltinTexture::ClearSkyGrad" );

//	void CreateSkysphere( const std::string& texture_resource_path = "BuiltinTexture::ClearSkyGrad" );

	CEntityHandle<> CreateStaticGeometry( const std::string& resource_path );

	CEntityHandle<> CreateStaticWater( const std::string& model, const std::string& name, const Vector3& position );

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

	Result::Name SetShader( CEntityHandle<>& entity, const std::string& shader, const std::string& technique, const std::string& subset, int lod = 0 );

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


extern void SetFloatShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, float value );
extern void SetColorShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, const SFloatRGBAColor& value );
extern void SetTextureShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, const char *tex_path );

} // namespace amorphous



#endif /* __StageUtility_HPP__ */
