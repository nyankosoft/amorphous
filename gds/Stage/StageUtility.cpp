#include "StageUtility.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Shader/ShaderVariableLoader.hpp"
#include "Physics/ActorDesc.hpp"
#include "Physics/BoxShapeDesc.hpp"
#include "Physics/TriangleMeshDesc.hpp"
#include "Physics/Scene.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Physics/Preprocessor.hpp"
#include "Physics/Enums.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/GameMessage.hpp"
#include "Stage/Stage.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/StaticGeometry.hpp"
#include "Stage/BE_StaticGeometry.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;


bool resources_exists( const std::string& resource_path )
{
	return boost::filesystem::exists( resource_path );
}


CEntityHandle<> CStageUtility::CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								physics::CActorDesc *pPhysActorDesc )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CEntityHandle<>();

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_name.c_str() );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = entity_name;
	desc.WorldPose = pose;
	desc.pPhysActorDesc = pPhysActorDesc;
//	desc.SetWorldOrient( CreateOrientFromFwdDir( dir ) );

	desc.vVelocity = vel;
	desc.fSpeed = Vec3Length(vel);

	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	if( !pEntity )
		return CEntityHandle<>();

	CBE_MeshObjectProperty& mesh_property = pEntity->pBaseEntity->MeshProperty();
	if( 0 < mesh_property.m_ShaderTechnique.size_x() )
	{
/*		pEntity->m_pMeshRenderMethod
			= shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod() );

		pEntity->m_pMeshRenderMethod->MeshRenderMethod().push_back( CSubsetRenderMethod() );
*/

		pEntity->m_pMeshRenderMethod = mesh_property.m_pMeshRenderMethod;
	}

	return pEntity ? CEntityHandle<>( pEntity->Self() ) : CEntityHandle<>();
}



//========================================================================================
// CreateCameraController
//========================================================================================

CEntityHandle<> CStageCameraUtility::CreateCameraController( const std::string& camera_controller_name,
													         int cutscene_input_handler_index )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CEntityHandle<>();

	char *base_entity_name = NULL;
	if( 0 <= cutscene_input_handler_index )
		base_entity_name = "CutsceneCameraController";
	else
		base_entity_name = "CameraController";

//	int result = PyArg_ParseTuple( args, "s|s", &camera_controller_name, &base_entity_name );

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( base_entity_name );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_controller_name;
	desc.WorldPose.Identity();

	CCopyEntity* pEntity = pStage->CreateEntity( desc );
	if( !pEntity )
	{
		LOG_PRINT_WARNING( fmt_string("Failed to create camera controller: '%s'", camera_controller_name.c_str()) );
		return CEntityHandle<>();
	}

	return CEntityHandle<>( pEntity->Self() );
}


CScriptedCameraEntity *CStageCameraUtility::CreateScriptedCamera( const std::string& camera_name,
														          const std::string& camera_controller_name,
															      CameraParam default_camera_param )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return NULL;

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( "ScriptedCamera" );

	CCopyEntityDesc desc;
	desc.SetDefault();
	desc.TypeID = CCopyEntityTypeID::SCRIPTED_CAMERA_ENTITY;
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.strName = camera_name;
	desc.WorldPose.vPosition = Vector3(0,0,0);

	CCopyEntity *pCameraController = pStage->GetEntitySet()->GetEntityByName( camera_controller_name.c_str() );
	if( !IsValidEntity(pCameraController) )
	{
		LOG_PRINT_ERROR( fmt_string(" Cannot find camera controller entity: '%s'", camera_controller_name.c_str()) );
		return NULL;
	}

	desc.pParent = pCameraController;

	CCopyEntity* pCameraEntity = pStage->CreateEntity( desc );
	if( !pCameraEntity )
	{
		LOG_PRINT_ERROR( fmt_string(" Cannot create camera entity: '%s'", camera_name.c_str()) );
		return NULL;
	}

	// set default camera params
	SGameMessage msg( GM_SET_DEFAULT_CAMERA_PARAM );
	msg.pUserData = &default_camera_param;
	SendGameMessageTo( msg, pCameraEntity );

//	return CEntityHandle<CScriptedCameraEntity>( pCameraEntity->??? )
	return dynamic_cast<CScriptedCameraEntity *>(pCameraEntity);
}



//========================================================================================
// CStageLightUtility
//========================================================================================

#include "Stage/LightEntity.hpp"
#include "Graphics/3DGameMath.hpp"

CLightEntityHandle ReturnLightEntityHandle( CCopyEntity *pEntity )
{
	if( pEntity )
	{
		shared_ptr<CLightEntity> pLightEntity
			= dynamic_pointer_cast<CLightEntity,CCopyEntity>( pEntity->Self().lock() );

		return CLightEntityHandle( pLightEntity );
	}
	else
		return CLightEntityHandle();
}


CLightEntityHandle CStageLightUtility::CreateHSPointLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CLightEntityHandle();

	CLightEntityDesc desc( CLight::HEMISPHERIC_POINT );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.aColor[0]  = upper_color;
	desc.aColor[1]  = lower_color;
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	desc.afAttenuation[0] = attenu0;
	desc.afAttenuation[1] = attenu1;
	desc.afAttenuation[2] = attenu2;

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericPointLight__" );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose.vPosition = pos;
	desc.WorldPose.matOrient = Matrix33Identity();

//	int shadow_for_light = 1; // true(1) by default
//	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}


CLightEntityHandle CStageLightUtility::CreateHSDirectionalLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CLightEntityHandle();

	CLightEntityDesc desc( CLight::HEMISPHERIC_DIRECTIONAL );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.SetHSUpperColor( upper_color );
	desc.SetHSLowerColor( lower_color );
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericDirectionalLight__" );
	desc.pBaseEntityHandle = &basehandle;

	Vector3 checked_dir;
	Vec3Normalize( checked_dir, dir );
	if( Vec3Length( checked_dir ) < 0.001f )
		checked_dir = Vector3(0,-1,0);

	desc.WorldPose.matOrient = CreateOrientFromFwdDir( checked_dir );
	desc.WorldPose.vPosition = Vector3(0,0,0);

//	int shadow_for_light = 1; // true(1) by default

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}


CLightEntityHandle CStageLightUtility::CreateHSSpotlightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& pos, const Vector3& dir,
		float attenu0, float attenu1, float attenu2,
		float inner_cone_angle, float outer_cone_angle )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CLightEntityHandle();

	CLightEntityDesc desc( CLight::HEMISPHERIC_SPOTLIGHT );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.SetHSUpperColor( upper_color );
	desc.SetHSLowerColor( lower_color );
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	desc.afAttenuation[0] = attenu0;
	desc.afAttenuation[1] = attenu1;
	desc.afAttenuation[2] = attenu2;

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericSpotlight__" );
	desc.pBaseEntityHandle = &basehandle;

	Vector3 checked_dir;
	Vec3Normalize( checked_dir, dir );
	if( Vec3Length( checked_dir ) < 0.001f )
		checked_dir = Vector3(0,-1,0);

	desc.WorldPose.matOrient = CreateOrientFromFwdDir( checked_dir );
	desc.WorldPose.vPosition = pos;

	desc.fInnerConeAngle = inner_cone_angle;
	desc.fOuterConeAngle = (0.0f < outer_cone_angle) ? outer_cone_angle : inner_cone_angle;

//	int shadow_for_light = 1; // true(1) by default

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}



//========================================================================================
// CStageMiscUtility
//========================================================================================

#include "Stage/BE_Skybox.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"
#include "Graphics/MeshModel/MeshArchiveToGeneral3DMeshConverer.hpp"
#include "Graphics/ShapeDetector.hpp"
#include "Physics/ConvexShapeDesc.hpp"
#include "Physics/ConvexMesh.hpp"

using namespace physics;


Result::Name SetBoxShapeDesc( CMeshObjectHandle& mesh_handle, CBoxShapeDesc& box_desc )
{
	shared_ptr<CBasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return Result::UNKNOWN_ERROR;

//	const AABB3 aabb = pMesh->GetAABB();
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	box_desc.vSideLength = aabb.vMax;

	// TODO: let client code specify a material by name
//	box_desc.MaterialName = material_name;
	box_desc.MaterialIndex = 1;

	return Result::SUCCESS;
}


Result::Name SetCylinderConvexShapeDesc( CMeshObjectHandle& mesh_handle, CConvexShapeDesc& convex_desc )
{
	shared_ptr<CBasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return Result::UNKNOWN_ERROR;

	// pMesh (above) == graphical representation of the cylinder and may contain redundant vertex unweldings and polygon subdivisions
	// pCylinderMesh (below) == 

	shared_ptr<CGeneral3DMesh> pCylinderMesh( new CGeneral3DMesh );

//	const AABB3 aabb = pMesh->GetAABB();
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	static const int cylinder_mesh_side_subdivisions = 16;

	// Assumes that the cylinder mesh is upright position in model space
	CCylinderDesc cylinder_desc;
	cylinder_desc.height    = aabb.vMax.y - aabb.vMin.y;
	cylinder_desc.radii[0]  = aabb.vMax.x - aabb.vMin.x;
	cylinder_desc.radii[1]  = aabb.vMax.z - aabb.vMin.z;
	cylinder_desc.num_sides = cylinder_mesh_side_subdivisions;
//	cylinder_desc.vertices_welding = WELDED;
	CreateCylinderMesh( cylinder_desc, *pCylinderMesh );

	CTriangleMeshDesc convex_mesh_desc;
	physics::CStream convex_mesh_stream;
	Result::Name res = physics::Preprocessor().CreateConvexMeshStream( convex_mesh_desc, convex_mesh_stream );

	CConvexMesh *pConvexMesh = physics::PhysicsEngine().CreateConvexMesh( convex_mesh_stream );
	if( !pConvexMesh )
		return Result::UNKNOWN_ERROR;

	convex_desc.pConvexMesh = pConvexMesh;

	// Where can I release pConvexMesh?

	if( pCylinderMesh->GetPolygonBuffer().empty() )
		return Result::UNKNOWN_ERROR;

	// TODO: let client code specify a material by name
//	convex_desc.MaterialName = material_name;
	convex_desc.MaterialIndex = 1;

	return Result::SUCCESS;
}


CEntityHandle<> CStageMiscUtility::CreatePhysicsEntity( CMeshResourceDesc& mesh_desc,
						      //CActorDesc& actor_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  vector<CShapeDesc *>& vecpShapeDesc,
							  float mass,
//							  const std::string& material_name,
							  bool static_actor )
{
	CMeshObjectHandle mesh_handle;
	bool mesh_loaded = mesh_handle.Load( mesh_desc );
	if( !mesh_loaded )
		return CEntityHandle<>();

	shared_ptr<CBasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return CEntityHandle<>();

	// assumes that the box mesh consists of just one material
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	// actor and shape descriptors


	CActorDesc actor_desc;
	actor_desc.vecpShapeDesc = vecpShapeDesc;
//	actor_desc.vecpShapeDesc.push_back( &box_desc );

	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
	actor_desc.BodyDesc.fMass = mass;

	CEntityHandle<> entity = CreateNamedEntity( entity_name, entity_attributes_name, pose, vel, &actor_desc );
	CCopyEntity *pEntityRawPtr = entity.GetRawPtr();
	if( !pEntityRawPtr )
		return CEntityHandle<>();

	pEntityRawPtr->m_MeshHandle = mesh_handle;
	pEntityRawPtr->local_aabb = aabb;
	pEntityRawPtr->world_aabb.TransformCoord( aabb, pEntityRawPtr->GetWorldPose().vPosition );

	return entity;
}


CEntityHandle<> CStageMiscUtility::CreateBoxEntity( CMeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );
	if( !loaded )
		return CEntityHandle<>();

	CBoxShapeDesc box_desc;
	Result::Name res = SetBoxShapeDesc( mesh, box_desc );
	if( res != Result::SUCCESS )
		return CEntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &box_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}

/*
void CreateOBBTreeFrom3DMeshModelArchive( C3DMeshModelArchive& mesh_archive, COBBTree& tree, int level )
{
	bool created = tree.Create(
		mesh_archive.GetVertexSet().vecPosition,
		mesh_archive.GetVertexIndex(),
		level
		);

	if( !created )
		LOG_PRINT_ERROR( " Failed to create an OBB tree from a mesh archive." );
}


CEntityHandle<> CStageMiscUtility::CreateBoxesEntity( CMeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
//	CMeshObjectHandle mesh;
//	bool loaded = mesh.Load( mesh_desc );
//	if( !loaded )
//		return CEntityHandle<>();

//	COBBTree obb_tree;
	C3DMeshModelArchive mesh_archive;
	bool mesh_archive_loaded = mesh_archive.LoadFromFile( mesh_desc.ResourcePath );
	if( !mesh_archive_loaded )
		return CEntityHandle<>();

	int obb_tree_level = 3;
//	bool created = CreateOBBTreeFrom3DMeshModelArchive( mesh_archive, obb_tree, obb_tree_level );

	vector<OBB3> obbs;
//	obb_tree.GetLeafOBBs( obbs );
	if( obbs.empty() )
		return CEntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.reserve( obbs.size() );

	vector<CBoxShapeDesc> box_descs;
	box_descs.resize( obbs.size() );
	for( size_t i=0; i<obbs.size(); i++ )
	{
		box_descs[i].LocalPose   = obbs[i].center;
		box_descs[i].vSideLength = obbs[i].radii;

		vecpShapeDesc.push_back( &box_descs[i] );
	}

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}
*/

CEntityHandle<> CStageMiscUtility::CreateCylinderEntity( CMeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
	CMeshObjectHandle mesh;
	bool loaded = mesh.Load( mesh_desc );
	if( !loaded )
		return CEntityHandle<>();

	CConvexShapeDesc convex_shape_desc;
	Result::Name res = SetCylinderConvexShapeDesc( mesh, convex_shape_desc );
	if( res != Result::SUCCESS )
		return CEntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &convex_shape_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}


CEntityHandle<> CStageMiscUtility::CreateBox( Vector3 edge_lengths,
											  SFloatRGBAColor diffuse_color,
											  const Matrix34& pose,
											  const float mass,
					                          const std::string& material_name,
					                          const std::string& entity_name,
					                          const std::string& entity_attributes_name
					                          )
{
//	printf( "Creating box at %s - size: %s, color: %s\n",
//		to_string(pos).c_str(), to_string(edge_lengths).c_str(), to_string(diffuse_color).c_str() );
/*
	Matrix34 pose;
	pose.vPosition = pos;
	pose.matOrient
		= Matrix33RotationY( deg_to_rad(heading) )
		* Matrix33RotationX( deg_to_rad(pitch) )
		* Matrix33RotationZ( deg_to_rad(bank) );
*/

	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	pBoxMeshGenerator->SetDiffuseColor( diffuse_color );
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


CEntityHandle<> CStageMiscUtility::CreateStaticBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color,
		const Matrix34& pose,
		const std::string& material_name,
		const std::string& entity_name,
		const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	pBoxMeshGenerator->SetDiffuseColor( diffuse_color );
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, 0.0f, material_name, true );
}


CEntityHandle<> CStageMiscUtility::CreateBoxFromMesh( const char *mesh_resource_path,//const std::string& mesh_resource_path,
					    const Matrix34& pose,
						const float mass,
					    const std::string& material_name,
					    const std::string& entity_name,
					    const std::string& entity_attributes_name
						)
{
//	printf( "Creating box from mesh '%s' at %s\n", mesh_resource_path/*.c_str()*/,
//		to_string(pose.vPosition).c_str() );

	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromMesh__";

	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_path;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


CEntityHandle<> CStageMiscUtility::CreateCylinderFromMesh( const char *mesh_resource_name,
						const Matrix34& pose,
						float mass,
						const std::string& material_name,
						const std::string& entity_name,
						const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__CylinderFromMesh__";

	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_name;

	Vector3 vel = Vector3(0,0,0);

	return CreateCylinderEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


Result::Name CStageMiscUtility::SetTriangleMeshShapeDesc( const char *collision_mesh_name,
														  CTriangleMeshShapeDesc& trimeshshapedesc )
{
	CTriangleMeshDesc trimeshdesc;

	C3DMeshModelArchive coll_mesh;
	bool coll_mesh_loaded = coll_mesh.LoadFromFile( collision_mesh_name );
	if( coll_mesh_loaded )
	{
		// copy indices
		size_t num_indices = coll_mesh.GetVertexIndex().size();
		trimeshdesc.m_vecIndex.resize( num_indices );
		for( size_t i=0; i<num_indices; i++ )
			trimeshdesc.m_vecIndex[i] = (int)coll_mesh.GetVertexIndex()[i];

		// copy vertices
		trimeshdesc.m_vecVertex = coll_mesh.GetVertexSet().vecPosition;

//		trimeshdesc.m_vecMaterialIndex = ???
		int mat_index = 0;
		trimeshdesc.m_vecMaterialIndex.resize( num_indices / 3, mat_index );
	}

	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
	{
		return Result::UNKNOWN_ERROR;
	}

	physics::CStream trimeshstream;
	physics::Preprocessor().CreateTriangleMeshStream( trimeshdesc, trimeshstream );
	trimeshstream.m_Buffer.seek_pos( 0 );

//	CTriangleMesh *pTriMesh = pStage->GetPhysicsScene()->CreateTriangleMesh( trimeshdesc );
	CTriangleMesh *pTriMesh = PhysicsEngine().CreateTriangleMesh( trimeshstream );
	if( !pTriMesh )
		return Result::UNKNOWN_ERROR;

//	CTriangleMeshShapeDesc trimeshshapedesc;
	trimeshshapedesc.pTriangleMesh = pTriMesh;

	return Result::SUCCESS;
}


CEntityHandle<> CStageMiscUtility::CreateTriangleMeshEntityFromMesh( const char *mesh_resource_path,
						const char *collision_mesh_name,
						const Matrix34& pose,
						float mass,
						const std::string& material_name,
						const std::string& entity_name,
						const std::string& entity_attributes_name,
						bool static_actor )
{
	if( !collision_mesh_name || strlen(collision_mesh_name) == 0 )
	{
		if( mesh_resource_path && 0 < strlen(mesh_resource_path) )
		{
			// Use the graphisc mesh as the collision geometry
			collision_mesh_name = mesh_resource_path;
		}
	}

	// create shape desc for collision mesh
	CTriangleMeshShapeDesc trimeshshapedesc;
	SetTriangleMeshShapeDesc( collision_mesh_name, trimeshshapedesc );
	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &trimeshshapedesc );

	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_path;

	return CreatePhysicsEntity(
		mesh_desc,
		entity_name,
		entity_attributes_name,
		pose,
		Vector3(0,0,0),
		vecpShapeDesc,
		mass,
		static_actor );
}


CEntityHandle<> CStageMiscUtility::CreateStaticTriangleMeshFromMesh( const char *mesh_resource_path,
						const char *collision_mesh_name,
						const Matrix34& pose,
						const std::string& material_name,
						const std::string& entity_name,
						const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__TriangleMeshFromMesh__";

	if( !resources_exists( mesh_resource_path ) )
	{
		return CEntityHandle<>();
	}

	return CreateTriangleMeshEntityFromMesh(
		mesh_resource_path,
		collision_mesh_name,
		pose,
		0.0f,
		material_name,
		entity_name,
		actual_entity_attributes_name,
		true );
/*
	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_name;
	CMeshHandle mesh;
	bool mesh_loaded = mesh.Load( mesh_resource_name );
	if( !mesh_loaded )
		return CEntityHandle<>();
*/
/*	CActorDesc actordesc;
	actordesc.vecpShapeDesc.push_back( &trimeshshapedesc );
	actordesc.BodyDesc.fMass = mas;*/

//	CreatePhysicsEntity( mesh, vecpShapeDesc, 

	return CEntityHandle<>();
}


/// Creates a triangle mesh actor from a graphics mesh file
CEntityHandle<> CStageMiscUtility::CreateTriangleMeshFromMesh( const char *mesh_resource_path,
						const char *collision_mesh_name,
						const Matrix34& pose,
						float mass,
						const std::string& material_name,
						const std::string& entity_name,
						const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__TriangleMeshFromMesh__";

	return CreateTriangleMeshEntityFromMesh(
		mesh_resource_path,
		collision_mesh_name,
		pose,
		mass,
		material_name,
		entity_name,
		actual_entity_attributes_name,
		false );

	return CEntityHandle<>();
}


CEntityHandle<> CStageMiscUtility::CreateEntity(
		const char *model,
		const char *name,
		const Vector3& position,
		const float heading,
		const float pitch,
		const float bank,
		const float mass,
		const char *shape,
		bool is_static )
{
	Matrix34 pose;
	pose.matOrient
		= Matrix33RotationY(deg_to_rad(heading))
		* Matrix33RotationX(deg_to_rad(pitch))
		* Matrix33RotationZ(deg_to_rad(bank));

	pose.vPosition = position;

	CMeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = model;

	const string shape_name = shape ? shape : "";
	if( shape_name == "box" )
	{
		return CreateBoxEntity(      mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
	}/*
//	else if( shape_name == "boxes" )
//	{
//		return CreateBoxesEntity(    mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
//	}
//	else if( shape_name == "capsule" )
//	{
//		return CreateCapsuleEntity(  mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
//	}
	else if( shape_name == "cylinder" )
	{
		return CreateCylinderEntity( mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
	}*/
//	else if( shape_name == "sphere" )
//	{
//		CreateSphereEntity( mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
//	}
	else if( shape_name == "mesh" )
	{
		return CreateTriangleMeshEntityFromMesh( model, model, pose, mass, "", name, "", is_static );
	}
//	else if( shape_name == "convex" )
//	{
//		return CreateTriangleMeshEntityFromMesh( model, model, pose, mass, "", name, "", is_static );
//	}
	else
	{
		string shape_desc_file = model;
		lfs::change_ext( shape_desc_file, "sd" );
		if( lfs::file_exists(shape_desc_file) )
		{
			LOG_PRINT_ERROR( " Shape desc file is not supported yet." );
			return CEntityHandle<>();

//			CShapeContainerGroup scg;
//			bool loaded = scg.LoadArchiveFromFile( shape_desc_file );
			// Load shape(s) from the shape desc file
		}
		else
		{
			// Guess shape(s) from the model
			C3DMeshModelArchive ma;
			bool ma_loaded = ma.LoadFromFile( model );
			boost::shared_ptr<CGeneral3DMesh> pMesh = CreateGeneral3DMesh();
			if( !pMesh )
				return CEntityHandle<>();
			CMeshArchiveToGeneral3DMeshConverer converter;
			Result::Name res = converter.Convert( ma, *pMesh );
			CShapeDetector shape_detector;
			CShapeDetectionResults results;
			bool shape_detected = shape_detector.DetectShape( *pMesh, results );
			if( !shape_detected )
				return CEntityHandle<>();
//			switch( sdr.shape )
		}

		return CEntityHandle<>();
	}
}


void CStageMiscUtility::CreateSkybox( const std::string& mesh_resource_path, const std::string& texture_resource_path )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return;// CEntityHandle<>();

	if( 0 < mesh_resource_path.length() )
	{
		// Load mesh
	}

	CCopyEntityDesc desc;

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "skybox" );//"__DefaultSkybox__" );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose = Matrix34Identity();

	CCopyEntity *pSkyboxEntity = pStage->CreateEntity( desc );
	if( !pSkyboxEntity )
		return;

	CBE_Skybox *pSkyboxBaseEntity = dynamic_cast<CBE_Skybox *>(pSkyboxEntity->pBaseEntity);
	if( pSkyboxBaseEntity )
		pSkyboxBaseEntity->LoadSkyboxTexture( texture_resource_path );
}

/*
void CStageMiscUtility::CreateSkysphere( const std::string& texture_resource_path )
{
	CCopyEntityDesc desc;

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__DefaultSkysphere__" );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose = Matrix34Identity();

	CSphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 5.0f;
	sphere_desc.num_segments = 6;
	sphere_desc.num_sides    = 12;
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new CSphereMeshGenerator(sphere_desc) );
	desc.MeshObjectHandle.Load( sphere_desc );

	CCopyEntity *pEntity = pStage->CreateEntity( desc );
}
*/


CEntityHandle<> CStageMiscUtility::CreateStaticGeometry( const std::string& resource_path )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return CEntityHandle<>();

	CEntityHandle<> entity = pStage->LoadStaticGeometryFromFile( resource_path );

	return entity;
}


// Creates a particle generator
// The particle generator keeps generating smoke particles
CEntityHandle<> CStageMiscUtility::CreateStaticSmokeSource( const Vector3& pos,
			const SFloatRGBAColor& color, float diameter, float rise_speed, float thickness, float density,
			const std::string& entity_attributes_name )
{
/*	CCopyEntityDesc desc;

	if( entity_attributes_name.length() == 0 )
		entity_attributes_name = "__StaticSmokeSource__";
//		entity_attributes_name = "__DefaultSmokeTrail__";

	CBaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( entity_attributes_name );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose.vPosition = pos;

	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	if( pEntity )
		return CEntityHandle<>( pEntity->pSelf );
	else*/
		return CEntityHandle<>();
}



//=============================================================================
// CStageEntityUtility
//=============================================================================

#include "Graphics/MeshContainerRenderMethod.hpp"


Result::Name CStageEntityUtility::SetShader( CEntityHandle<>& entity, const std::string& shader_name, const std::string& subset_name, int lod )
{
	CSubsetRenderMethod subset_render_method;
	size_t pos = shader_name.find( "::" );

	shared_ptr<CCopyEntity> pEntity = entity.Get();
	if( !pEntity )
		return Result::INVALID_ARGS;

	const int max_shader_lod = 16;
	if( lod < 0 || max_shader_lod <= lod )
		return Result::INVALID_ARGS;

	if( pos != string::npos )
	{
		string filepath = "";
		string technique_name = "";

		subset_render_method.m_ShaderDesc.ResourcePath = filepath;
		subset_render_method.m_Technique.SetTechniqueName( technique_name.c_str() );
	}
	else
	{
		subset_render_method.m_ShaderDesc.ResourcePath = shader_name;
	}

	if( !pEntity->m_pMeshRenderMethod )
	{
		pEntity->m_pMeshRenderMethod
			= shared_ptr<CMeshContainerRenderMethod>( new CMeshContainerRenderMethod );
	}

	if( 0 == subset_name.length() )
	{
		vector<CSubsetRenderMethod>& vecRenderMethod
			= pEntity->m_pMeshRenderMethod->MeshRenderMethod();

		while( (int)vecRenderMethod.size() <= lod )
			vecRenderMethod.push_back( CSubsetRenderMethod() );

			vecRenderMethod[lod] = subset_render_method;
	}
	else
	{
		vector< map<string,CSubsetRenderMethod> >& mapRenderMethodMap
			= pEntity->m_pMeshRenderMethod->SubsetRenderMethodMaps();

		while( (int)mapRenderMethodMap.size() <= lod )
			mapRenderMethodMap.push_back( map<string,CSubsetRenderMethod>() );

		mapRenderMethodMap[lod][subset_name] = subset_render_method;
	}

	return Result::SUCCESS;
}



static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    CShaderParameter<float> param )
{
	pStaticGeometry->SetFloatShaderParam( param );
}


static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    CShaderParameter<SFloatRGBAColor> param )
{
	pStaticGeometry->SetColorShaderParam( param );
}


static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    CShaderParameter<CTextureParam> param )
{
	pStaticGeometry->SetTextureShaderParam( param );
}


template<typename T>
static void SetShaderParamToStaticGeometryEntity( CCopyEntity *pEntity,
												  const char *parameter_name,
												  T value )
{
	CBE_StaticGeometry *pBaseEntity
		= dynamic_cast<CBE_StaticGeometry *>(pEntity->pBaseEntity);

	CStaticGeometryBase *pStaticGeometry
		= pBaseEntity->GetStaticGeometry(pEntity);

	if( !pStaticGeometry )
		return;

	CShaderParameter<T> param(parameter_name);
	param.Parameter() = value;

	SetShaderParamToStaticGeometry( pStaticGeometry, param );
}


template<typename T>
static void SetShaderParamLoaderToEntity( CEntityHandle<>& entity, const char *parameter_name, T value )
{
	CCopyEntity *pEntity = entity.GetRawPtr();
	if( !pEntity )
		return;

	if( pEntity->pBaseEntity->GetArchiveObjectID() == CBaseEntity::BE_STATICGEOMETRY )
	{
		SetShaderParamToStaticGeometryEntity( pEntity, parameter_name, value );
	}

	shared_ptr<CMeshContainerRenderMethod> pRenderMethod
		= pEntity->m_pMeshRenderMethod;

	if( !pRenderMethod )
	{
		return;
	}

	bool found_param_loader = false;
	for( size_t i=0; i<pRenderMethod->m_vecMeshRenderMethod.size(); i++ )
	{
		CSubsetRenderMethod& subset_render_method = pRenderMethod->m_vecMeshRenderMethod[i];

		for( size_t j=0; j<subset_render_method.m_vecpShaderParamsLoader.size(); j++ )
		{
			if( typeid(*subset_render_method.m_vecpShaderParamsLoader[j]) != typeid(CShaderVariableLoader<T>) )
				continue;

			// found a shader variable loader
			CShaderVariableLoader<T> *pLoader
				= dynamic_cast< CShaderVariableLoader<T> *>( subset_render_method.m_vecpShaderParamsLoader[j].get() );

			if( pLoader
			 && pLoader->GetParamName() == parameter_name )
			{
				pLoader->SetParamValue( value );
				found_param_loader = true;
			}
		}

		pRenderMethod->m_vecMeshRenderMethod.size();
	}

	if( !found_param_loader )
	{
		// no shader variable loader for the parameter with parameter_name was found
		// - register a new variable loader
		CShaderParameter<T> param(parameter_name);
		param.Parameter() = value;

		shared_ptr< CShaderVariableLoader<T> > pVarLoader( new CShaderVariableLoader<T>(param) );

		pRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pVarLoader );
	}
}


/**
 Useful if the entity has a single shader, technique, and surface.
*/
void SetFloatShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, float value )
{
	SetShaderParamLoaderToEntity( entity, parameter_name, value );
}


void SetColorShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, const SFloatRGBAColor& value )
{
	SetShaderParamLoaderToEntity( entity, parameter_name, value );
}


void SetTextureShaderParamToEntity( CEntityHandle<> entity, const char *parameter_name, const char *tex_path )
{
//	CShaderParameter<CTextureParam> tex_param( parameter_name );
//	tex_param.Parameter().m_Desc.ResourcePath = tex_path;
	CTextureParam tex_param;
	tex_param.m_Desc.ResourcePath = tex_path;
	bool tex_loaded = tex_param.m_Handle.Load( tex_param.m_Desc );
	SetShaderParamLoaderToEntity( entity, parameter_name, tex_param );
}
