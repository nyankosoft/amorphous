#ifndef __StageUtility_HPP__
#define __StageUtility_HPP__


#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/EntityHandle.hpp"
#include "amorphous/Physics/fwd.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/3DMath/AABB3.hpp"


namespace amorphous
{


class StageUtility
{
protected:

	boost::weak_ptr<CStage> m_pStage;

public:

	StageUtility() {}

	virtual ~StageUtility() {}

	StageUtility( boost::weak_ptr<CStage> pStage )
		:
	m_pStage(pStage)
	{}

	EntityHandle<> StageUtility::CreateNamedEntity( CCopyEntityDesc& desc,
									const std::string& base_name );

	EntityHandle<> CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								physics::CActorDesc *pPhysActorDesc,
								MeshHandle& mesh );
};


class StageMiscUtility : public StageUtility
{
	EntityHandle<> CreatePhysicsEntity( MeshHandle& mesh_handle,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  std::vector<physics::CShapeDesc *>& vecpShapeDesc,
								  float mass,
								  bool static_actor );

	EntityHandle<> CreatePhysicsEntity( MeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  std::vector<physics::CShapeDesc *>& vecpShapeDesc,
								  float mass,
								  bool static_actor );

	EntityHandle<> CreateBoxEntity( MeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  float mass,
								  const std::string& material_name,
								  bool static_actor );

	EntityHandle<> CreateCylinderEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor );

	EntityHandle<> CreateSphereEntity( MeshResourceDesc& mesh_desc,
								  const std::string& entity_name,
								  const std::string& entity_attributes_name,
								  const Matrix34& pose,
								  const Vector3& vel,
								  float mass,
								  const std::string& material_name,
								  bool static_actor );

	EntityHandle<> CreateBoxesEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor );

	EntityHandle<> CreateTriangleMeshEntityFromMesh( const char *mesh_resource_name,
							const char *collision_mesh_name,
							const Matrix34& pose,
							float mass,
							const std::string& material_name,
							const std::string& entity_name,
							const std::string& entity_attributes_name,
							bool static_actor );

	Result::Name SetTriangleMeshShapeDesc( const char *collision_mesh_name,
		physics::CTriangleMeshShapeDesc& trimeshshapedesc );

	EntityHandle<> CreateEntityFromBaseEntity( const char *model,
		const char *name,
		const Matrix34& pose );

public:

	/// default ctor. Added to compile this code with boost::python.
	StageMiscUtility() {}

	StageMiscUtility( boost::shared_ptr<CStage> pStage )
		:
	StageUtility(pStage)
	{}

	EntityHandle<> CreateBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		float mass = 1.0f,
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color,
		const Vector3& pos,
		const float heading = 0.0f,
		const float pitch = 0.0f,
		const float bank = 0.0f,
		float mass = 1.0f,
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateStaticBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateBoxFromMesh( const char *mesh_resource_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateSphere( float diameter = 1.0f,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		float mass = 1.0f,
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );
/*
	EntityHandle<> CreateStaticBall( float radius,
		SFloatRGBAColor diffuse_color = SFloatRGBAColor(1,1,1,1),
		const Matrix34& pose = Matrix34Identity(),
		const std::string& material_name = "default",
		const std::string& entity_name = "",
		const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateBallFromMesh( const char *mesh_resource_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );
*/


	EntityHandle<> CreateCylinderFromMesh( const char *model,
							const char *name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default" );

	EntityHandle<> CreateCylinderFromMesh( const char *model,
						const char *name,
						const Vector3& position = Vector3(0,0,0),
						float heading = 0.0f,
						float pitch = 0.0f,
						float bank = 0.0f,
						float mass = 1.0f,
						const std::string& material_name = "default" );

	EntityHandle<> CreateStaticCylinderFromMesh( const char *model,
								const char *name,
								const Matrix34& pose = Matrix34Identity(),
								float mass = 1.0f,
								const std::string& material_name = "default" )
	{
		MeshResourceDesc mesh_desc;
		mesh_desc.ResourcePath = model;
		Vector3 vel( Vector3(0,0,0) );
//		std::string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__CylinderFromMesh__";
		std::string actual_entity_attributes_name = "__CylinderFromMesh__";

		return CreateCylinderEntity( mesh_desc, name, actual_entity_attributes_name, pose, vel, mass, material_name, true );
	}

	/// Creates a static triangle mesh actor from a graphics mesh file
	EntityHandle<> CreateStaticTriangleMeshFromMesh( const char *mesh_resource_path,
		                    const char *collision_mesh_name,
							const Matrix34& pose = Matrix34Identity(),
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	/// Creates a triangle mesh actor from a graphics mesh file
	EntityHandle<> CreateTriangleMeshFromMesh( const char *mesh_resource_path,
		                    const char *collision_mesh_name,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateEntityFromConvexMesh( MeshHandle& convex_mesh,
							const Matrix34& pose = Matrix34Identity(),
							float mass = 1.0f,
							const std::string& material_name = "default",
							const std::string& entity_name = "",
							const std::string& entity_attributes_name = "" );

	EntityHandle<> CreateEntity(
		const char *model,
		const char *name = "",
		const Vector3& position = Vector3(0,0,0),
		const float heading = 0,
		const float pitch   = 0,
		const float bank    = 0,
		const float mass = 1.0f,
		const char *shape = "",
		bool is_static = false);

	Result::Name FixEntity(
		EntityHandle<>& entity,
		float max_force  = FLT_MAX,
		float max_torque = FLT_MAX );

	Result::Name GlueEntities(
		EntityHandle<>& entity0,
		EntityHandle<>& entity1,
		float max_force  = FLT_MAX,
		float max_torque = FLT_MAX );

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

	EntityHandle<> CreateStaticGeometry( const std::string& resource_path );

	EntityHandle<> CreateStaticWater( const std::string& model, const std::string& name, const Vector3& position );

	EntityHandle<> CreateStaticSmokeSource( const Vector3& pos,
			const SFloatRGBAColor& color, float diameter, float rise_speed, float thickness, float density,
			const std::string& entity_attributes_name = "" );

//	void CreateSmokeTrailSource( const SFloatRGBAColor& color, const string& target_entity_name );

	EntityHandle<> CreateNonCollidableEntityFromMesh(
		MeshHandle& mesh,
		const Matrix34& pose,
		const std::string& entity_name
		);

	EntityHandle<> CreateNonCollidableEntityFromMesh(
		const char *mesh_resource_path,
		const Matrix34& pose,
		const std::string& entity_name
		);
};


class StageEntityUtility : public StageUtility
{
public:

	StageEntityUtility() {}

	StageEntityUtility( boost::shared_ptr<CStage> pStage )
		:
	StageUtility(pStage)
	{}

	Result::Name SetShader( EntityHandle<>& entity, const std::string& shader, const std::string& technique, const std::string& subset, int lod = 0 );

	Result::Name RemoveAllShaders( EntityHandle<>& entity );
};


/*
class CStageEffectUtility : public StageUtility
{
public:

	/// default ctor. Added to compile this code with boost::python.
	CStageEffectUtility() {}

	CStageEffectUtility( boost::shared_ptr<CStage> pStage )
		:
	StageUtility(pStage)

	void Rain( float fall_speed ) {}

	void Snow( float fall_speed ) {}

	void SetWind( const AABB3& world_aabb, const Vector3 vForce ) {}

	void AddLensFlareElement( LightEntityHandle directional_light_entity, const std::string& texture_filepath, float size, float dist ) {}
};
*/


extern void SetFloatShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, float value );
extern void SetColorShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, const SFloatRGBAColor& value );
extern void SetTextureShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, const char *tex_path );

} // namespace amorphous



#endif /* __StageUtility_HPP__ */
