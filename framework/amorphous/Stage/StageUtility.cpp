#include "StageUtility.hpp"
#include "amorphous/3DMath/3DGameMath.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Shader/ShaderVariableLoader.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Physics/ActorDesc.hpp"
#include "amorphous/Physics/BoxShapeDesc.hpp"
#include "amorphous/Physics/TriangleMeshDesc.hpp"
#include "amorphous/Physics/Scene.hpp"
#include "amorphous/Physics/PhysicsEngine.hpp"
#include "amorphous/Physics/Preprocessor.hpp"
#include "amorphous/Physics/Enums.hpp"
#include "amorphous/Physics/FixedJointDesc.hpp"
#include "amorphous/Physics/MeshConvenienceFunctions.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/GameMessage.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Stage/CopyEntityDesc.hpp"
#include "amorphous/Stage/StaticGeometry.hpp"
#include "amorphous/Stage/BE_StaticGeometry.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Utilities/Physics/MeshConversions.hpp"


namespace amorphous
{

using namespace std;
using std::map;


bool resources_exists( const std::string& resource_path )
{
	return lfs::path_exists( resource_path );
}


EntityHandle<> StageUtility::CreateNamedEntity( CCopyEntityDesc& desc,
								const std::string& base_name )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return EntityHandle<>();

	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	if( !pEntity )
		return EntityHandle<>();

	SharedMeshContainer& mesh_container = pEntity->pBaseEntity->MeshProperty();
	if( 0 < mesh_container.m_ShaderTechnique.size_x() )
	{
		pEntity->m_pMeshRenderMethod = mesh_container.m_pMeshRenderMethod;

		pEntity->pBaseEntity->InitEntityGraphics( *pEntity,
			mesh_container.m_ShaderHandle,
			mesh_container.m_ShaderTechnique(0,0) );
	}
	else
	{
		pEntity->pBaseEntity->InitEntityGraphics( *pEntity );
	}

	return pEntity ? EntityHandle<>( pEntity->Self() ) : EntityHandle<>();
}


EntityHandle<> StageUtility::CreateNamedEntity( const std::string& entity_name,
								const std::string& base_name,
								const Matrix34& pose,
								const Vector3& vel,
								physics::CActorDesc *pPhysActorDesc,
								MeshHandle& mesh
								)
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
	{
		LOG_PRINT_ERROR( "pStage is NULL." );
		return EntityHandle<>();
	}

	BaseEntityHandle baseentity_handle;
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

	desc.MeshObjectHandle = mesh;

	desc.sGroupID = 0;

	return CreateNamedEntity( desc, base_name );

//	return pEntity ? EntityHandle<>( pEntity->Self() ) : EntityHandle<>();
}


} // namespace amorphous


//========================================================================================
// StageMiscUtility
//========================================================================================

#include "BE_Skybox.hpp"
#include "amorphous/Graphics/MeshModel/General3DMesh.hpp"
#include "amorphous/Graphics/MeshModel/MeshArchiveToGeneral3DMeshConverer.hpp"
#include "amorphous/Graphics/ShapeDetector.hpp"
#include "amorphous/Physics/ConvexShapeDesc.hpp"
#include "amorphous/Physics/ConvexMesh.hpp"


namespace amorphous
{

using namespace physics;


Result::Name SetBoxShapeDesc( MeshHandle& mesh_handle, CBoxShapeDesc& box_desc )
{
	shared_ptr<BasicMesh> pMesh = mesh_handle.GetMesh();
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


Result::Name SetCylinderConvexShapeDesc( MeshHandle& mesh_handle, CConvexShapeDesc& convex_desc )
{
	shared_ptr<BasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return Result::UNKNOWN_ERROR;

	// pMesh (above) == graphical representation of the cylinder and may contain redundant vertex unweldings and polygon subdivisions
	// pCylinderMesh (below) == 

	shared_ptr<General3DMesh> pCylinderMesh( new General3DMesh );

//	const AABB3 aabb = pMesh->GetAABB();
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	static const int cylinder_mesh_side_subdivisions = 16;

	// Assumes that the cylinder mesh is upright position in model space
	CylinderDesc cylinder_desc;
	cylinder_desc.height    = aabb.vMax.y - aabb.vMin.y;
	cylinder_desc.radii[0]  = aabb.vMax.x - aabb.vMin.x;
	cylinder_desc.radii[1]  = aabb.vMax.z - aabb.vMin.z;
	cylinder_desc.num_sides = cylinder_mesh_side_subdivisions;
//	cylinder_desc.vertices_welding = WELDED;
	CreateCylinderMesh( cylinder_desc, *pCylinderMesh );

	CTriangleMeshDesc convex_mesh_desc;
	convex_mesh_desc.m_vecIndex;
	convex_mesh_desc.m_vecVertex;
	General3DMeshToTriangleMeshDesc( *pCylinderMesh, convex_mesh_desc );

	bool res = SetConvexShapeDesc( convex_mesh_desc, convex_desc );

	if( !res )
	{
		LOG_PRINT_ERROR( "SetConvexShapeDesc() failed."  );
		return Result::UNKNOWN_ERROR;
	}

	// Where can I release pConvexMesh?

	if( pCylinderMesh->GetPolygonBuffer().empty() )
		return Result::UNKNOWN_ERROR;

	// TODO: let client code specify a material by name
//	convex_desc.MaterialName = material_name;
	convex_desc.MaterialIndex = 1;

	return Result::SUCCESS;
}


Result::Name SetSphereShapeDesc( MeshHandle& mesh_handle, CSphereShapeDesc& sphere_desc )
{
	shared_ptr<BasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return Result::UNKNOWN_ERROR;

//	const AABB3 aabb = pMesh->GetAABB();
	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
		aabb.MergeAABB( pMesh->GetAABB(i) );

	sphere_desc.Radius = aabb.vMax.z;

	// TODO: let client code specify a material by name
//	sphere_desc.MaterialName = material_name;
	sphere_desc.MaterialIndex = 1;

	return Result::SUCCESS;
}


EntityHandle<> StageMiscUtility::CreatePhysicsEntity( MeshHandle& mesh_handle,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  vector<CShapeDesc *>& vecpShapeDesc,
							  float mass,
							  bool static_actor )
{
	shared_ptr<BasicMesh> pMesh = mesh_handle.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
		return EntityHandle<>();

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

	EntityHandle<> entity = CreateNamedEntity( entity_name, entity_attributes_name, pose, vel, &actor_desc, mesh_handle );
	CCopyEntity *pEntityRawPtr = entity.GetRawPtr();
	if( !pEntityRawPtr )
		return EntityHandle<>();

	pEntityRawPtr->m_MeshHandle = mesh_handle;
	pEntityRawPtr->local_aabb = aabb;
	pEntityRawPtr->world_aabb.TransformCoord( aabb, pEntityRawPtr->GetWorldPose().vPosition );

	return entity;
}


EntityHandle<> StageMiscUtility::CreatePhysicsEntity( MeshResourceDesc& mesh_desc,
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
	MeshHandle mesh_handle;
	bool mesh_loaded = mesh_handle.Load( mesh_desc );
	if( !mesh_loaded )
	{
		LOG_PRINT_ERROR( " Failed to load the mesh." );
		return EntityHandle<>();
	}

	return CreatePhysicsEntity( mesh_handle, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}


EntityHandle<> StageMiscUtility::CreateBoxEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );
	if( !loaded )
		return EntityHandle<>();

	CBoxShapeDesc box_desc;
	Result::Name res = SetBoxShapeDesc( mesh, box_desc );
	if( res != Result::SUCCESS )
		return EntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &box_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}

/*
void CreateOBBTreeFrom3DMeshModelArchive( C3DMeshModelArchive& mesh_archive, OBBTree& tree, int level )
{
	bool created = tree.Create(
		mesh_archive.GetVertexSet().vecPosition,
		mesh_archive.GetVertexIndex(),
		level
		);

	if( !created )
		LOG_PRINT_ERROR( " Failed to create an OBB tree from a mesh archive." );
}


EntityHandle<> StageMiscUtility::CreateBoxesEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
//	MeshHandle mesh;
//	bool loaded = mesh.Load( mesh_desc );
//	if( !loaded )
//		return EntityHandle<>();

//	OBBTree obb_tree;
	C3DMeshModelArchive mesh_archive;
	bool mesh_archive_loaded = mesh_archive.LoadFromFile( mesh_desc.ResourcePath );
	if( !mesh_archive_loaded )
		return EntityHandle<>();

	int obb_tree_level = 3;
//	bool created = CreateOBBTreeFrom3DMeshModelArchive( mesh_archive, obb_tree, obb_tree_level );

	vector<OBB3> obbs;
//	obb_tree.GetLeafOBBs( obbs );
	if( obbs.empty() )
		return EntityHandle<>();

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

EntityHandle<> StageMiscUtility::CreateCylinderEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );
	if( !loaded )
		return EntityHandle<>();

	CConvexShapeDesc convex_shape_desc;
	Result::Name res = SetCylinderConvexShapeDesc( mesh, convex_shape_desc );
	if( res != Result::SUCCESS )
		return EntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &convex_shape_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}


EntityHandle<> StageMiscUtility::CreateSphereEntity( MeshResourceDesc& mesh_desc,
							  const std::string& entity_name,
							  const std::string& entity_attributes_name,
							  const Matrix34& pose,
							  const Vector3& vel,
							  float mass,
							  const std::string& material_name,
							  bool static_actor )
{
	MeshHandle mesh;
	bool loaded = mesh.Load( mesh_desc );
	if( !loaded )
		return EntityHandle<>();

	CSphereShapeDesc sphere_desc;
	Result::Name res = SetSphereShapeDesc( mesh, sphere_desc );
	if( res != Result::SUCCESS )
		return EntityHandle<>();

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &sphere_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	return CreatePhysicsEntity( mesh_desc, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}


EntityHandle<> StageMiscUtility::CreateBox( Vector3 edge_lengths,
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

	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	pBoxMeshGenerator->SetDiffuseColor( diffuse_color );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


EntityHandle<> StageMiscUtility::CreateBox( Vector3 edge_lengths,
											  SFloatRGBAColor diffuse_color,
											  const Vector3& pos,
											  const float heading,
											  const float pitch,
											  const float bank,
											  const float mass,
					                          const std::string& material_name,
					                          const std::string& entity_name,
					                          const std::string& entity_attributes_name
					                          )
{
	return CreateBox(
		edge_lengths,
		diffuse_color,
		Matrix34( pos, Matrix33RotationHPR_deg( heading, pitch, bank ) ),
		mass,
		material_name,
		entity_name,
		entity_attributes_name
		);
}


EntityHandle<> StageMiscUtility::CreateStaticBox( Vector3 edge_lengths,
		SFloatRGBAColor diffuse_color,
		const Matrix34& pose,
		const std::string& material_name,
		const std::string& entity_name,
		const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	pBoxMeshGenerator->SetDiffuseColor( diffuse_color );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, 0.0f, material_name, true );
}


EntityHandle<> StageMiscUtility::CreateBoxFromMesh( const char *mesh_resource_path,//const std::string& mesh_resource_path,
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

	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_path;

	Vector3 vel = Vector3(0,0,0);

	return CreateBoxEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


EntityHandle<> StageMiscUtility::CreateSphere( float diameter,
											  SFloatRGBAColor diffuse_color,
											  const Matrix34& pose,
											  const float mass,
					                          const std::string& material_name,
					                          const std::string& entity_name,
					                          const std::string& entity_attributes_name
					                          )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__BoxFromDimension__";

	SphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = diameter * 0.5f;
	shared_ptr<SphereMeshGenerator> pSphereMeshGenerator( new SphereMeshGenerator(sphere_desc) );
	pSphereMeshGenerator->SetDiffuseColor( diffuse_color );
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pSphereMeshGenerator;

	Vector3 vel = Vector3(0,0,0);

	return CreateSphereEntity( mesh_desc, entity_name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


EntityHandle<> StageMiscUtility::CreateCylinderFromMesh( const char *model,
						const char *name,
						const Matrix34& pose,
						float mass,
						const std::string& material_name )
{
//	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__CylinderFromMesh__";
	string actual_entity_attributes_name = "__CylinderFromMesh__";

	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = model;

	Vector3 vel = Vector3(0,0,0);

	return CreateCylinderEntity( mesh_desc, name, actual_entity_attributes_name, pose, vel, mass, material_name, false );
}


EntityHandle<> StageMiscUtility::CreateCylinderFromMesh( const char *model,
						const char *name,
						const Vector3& position,
						float heading,
						float pitch,
						float bank,
						float mass,
						const std::string& material_name )
{
	return CreateCylinderFromMesh(
		model,
		name,
		Matrix34( position, Matrix33RotationHPR_deg( heading, pitch, bank ) ),
		mass,
		material_name
		);
}


Result::Name StageMiscUtility::SetTriangleMeshShapeDesc( const char *collision_mesh_name,
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
	else
	{
		LOG_PRINTF_WARNING(( "Failed to load the collision mesh: %s", collision_mesh_name ));
		return Result::UNKNOWN_ERROR;
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


EntityHandle<> StageMiscUtility::CreateTriangleMeshEntityFromMesh( const char *mesh_resource_path,
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
			// Use the graphics mesh as the collision geometry
			collision_mesh_name = mesh_resource_path;
		}
	}

	// create shape desc for collision mesh
	CTriangleMeshShapeDesc trimeshshapedesc;
	SetTriangleMeshShapeDesc( collision_mesh_name, trimeshshapedesc );
	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &trimeshshapedesc );

	MeshResourceDesc mesh_desc;
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


EntityHandle<> StageMiscUtility::CreateStaticTriangleMeshFromMesh( const char *mesh_resource_path,
						const char *collision_mesh_name,
						const Matrix34& pose,
						const std::string& material_name,
						const std::string& entity_name,
						const std::string& entity_attributes_name )
{
	string actual_entity_attributes_name = 0 < entity_attributes_name.length() ? entity_attributes_name : "__TriangleMeshFromMesh__";

	if( !resources_exists( mesh_resource_path ) )
	{
		LOG_PRINTF_WARNING(( " The specified mesh resource '%s' was not found.", mesh_resource_path ));
		return EntityHandle<>();
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
	MeshResourceDesc mesh_desc;
	mesh_desc.ResourcePath = mesh_resource_name;
	CMeshHandle mesh;
	bool mesh_loaded = mesh.Load( mesh_resource_name );
	if( !mesh_loaded )
		return EntityHandle<>();
*/
/*	CActorDesc actordesc;
	actordesc.vecpShapeDesc.push_back( &trimeshshapedesc );
	actordesc.BodyDesc.fMass = mas;*/

//	CreatePhysicsEntity( mesh, vecpShapeDesc, 

	return EntityHandle<>();
}


/// Creates a triangle mesh actor from a graphics mesh file
EntityHandle<> StageMiscUtility::CreateTriangleMeshFromMesh( const char *mesh_resource_path,
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

	return EntityHandle<>();
}


EntityHandle<> StageMiscUtility::CreateEntityFromConvexMesh(
	MeshHandle& convex_mesh,
	const Matrix34& pose,
	float mass,
	const std::string& material_name,
	const std::string& entity_name,
	const std::string& entity_attributes_name
	)
{
	shared_ptr<BasicMesh> pMesh = convex_mesh.GetMesh();
	if( !pMesh || pMesh->GetNumMaterials() == 0 )
	{
		LOG_PRINT_ERROR( "An invalid mesh."  );
		return EntityHandle<>();
	}

	BasicMesh& mesh = *pMesh;

	CConvexShapeDesc convex_shape_desc;

	// Fill out the convex mesh desc
	CTriangleMeshDesc convex_mesh_desc;
	mesh.GetVertexPositions( convex_mesh_desc.m_vecVertex );
	vector<uint> vertex_indices;
	mesh.GetVertexIndices( vertex_indices );

	convex_mesh_desc.m_vecIndex.resize( vertex_indices.size() );
	for( size_t i=0; i<vertex_indices.size(); i++ )
		convex_mesh_desc.m_vecIndex[i] = (uint)vertex_indices[i];

	convex_mesh_desc.m_vecMaterialIndex.resize( vertex_indices.size() / 3 );

	bool res = SetConvexShapeDesc( convex_mesh_desc, convex_shape_desc );

	if( !res )
	{
		LOG_PRINT_ERROR( "SetConvexShapeDesc() failed."  );
		return EntityHandle<>();
	}

	convex_shape_desc.MaterialIndex = 1;

	vector<CShapeDesc *> vecpShapeDesc;
	vecpShapeDesc.push_back( &convex_shape_desc );

//	actor_desc.BodyDesc.Flags |= static_actor ? BodyFlag::Static : 0;
//	actor_desc.BodyDesc.fMass = mass;

	bool static_actor = false;
	Vector3 vel = Vector3(0,0,0);
	return CreatePhysicsEntity( convex_mesh, entity_name, entity_attributes_name, pose, vel, vecpShapeDesc, mass, static_actor );
}


EntityHandle<> StageMiscUtility::CreateEntityFromBaseEntity( 
	const char *model,
	const char *name,
	const Matrix34& pose )
{
	std::shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return EntityHandle<>();

	if( !model
	 || strlen(model) == 0 )
	{
		return EntityHandle<>();
	}

	CCopyEntityDesc entity_desc;
	BaseEntityHandle base_entity( model );
	entity_desc.pBaseEntityHandle = &base_entity;
	if( name )
		entity_desc.strName = name;
	entity_desc.WorldPose = pose;

	CCopyEntity *pEntity = pStage->CreateEntity( entity_desc );
	if( pEntity )
		return EntityHandle<>( pEntity->Self() );
	else
		return EntityHandle<>();
}


EntityHandle<> StageMiscUtility::CreateEntity(
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
	Matrix34 pose( position, Matrix33RotationHPR_deg( heading, pitch, bank ) );

	if( !model
	 || strlen(model) == 0 )
	{
		return EntityHandle<>();
	}

	if( string(model).rfind(".msh") != string(model).length() - 4 )
	{
		// 'model' is not a pathname of a mesh file. Consider this as a base entity name
		return CreateEntityFromBaseEntity( model, name, pose );
	}
	
	MeshResourceDesc mesh_desc;
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
//	}*/
	else if( shape_name == "cylinder" )
	{
		return CreateCylinderEntity( mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
	}
	else if( shape_name == "sphere" )
	{
		CreateSphereEntity( mesh_desc, name, "", pose, Vector3(0,0,0), mass, "", is_static );
	}
	else if( shape_name == "mesh" )
	{
		return CreateTriangleMeshEntityFromMesh( model, model, pose, mass, "", name, "__TriangleMeshFromMesh__", is_static );
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
			return EntityHandle<>();

//			ShapeContainerGroup scg;
//			bool loaded = scg.LoadArchiveFromFile( shape_desc_file );
			// Load shape(s) from the shape desc file
		}
		else
		{
			// Guess shape(s) from the model
			C3DMeshModelArchive ma;
			bool ma_loaded = ma.LoadFromFile( model );
			std::shared_ptr<General3DMesh> pMesh = CreateGeneral3DMesh();
			if( !pMesh )
				return EntityHandle<>();
			CMeshArchiveToGeneral3DMeshConverer converter;
			Result::Name res = converter.Convert( ma, *pMesh );
			ShapeDetector shape_detector;
			ShapeDetectionResults results;
			bool shape_detected = shape_detector.DetectShape( *pMesh, results );
			if( !shape_detected )
				return EntityHandle<>();
//			switch( sdr.shape )
		}

		return EntityHandle<>();
	}
}


static CCopyEntity *GetEntityFromEntityName( const char *entity_name, CStage& stage )
{
	if( !entity_name
	 || strlen(entity_name) == 0 )
	{
		return NULL;
	}

	return stage.GetEntitySet()->GetEntityByName( entity_name );
}


static physics::CActor *GetPrimaryActor( CCopyEntity& entity )
{
	if( entity.m_vecpPhysicsActor.empty() )
		return NULL;

	return entity.m_vecpPhysicsActor[0];
}


Result::Name StageMiscUtility::FixEntity(
		EntityHandle<>& entity,
		float max_force,
		float max_torque )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CCopyEntity> pEntity = entity.Get();

	if( !pEntity )
	{
		LOG_PRINT_ERROR( "An invalid entity." );
		return Result::INVALID_ARGS;
	}

	if( pEntity->m_vecpPhysicsActor.empty() )
	{
		LOG_PRINT_ERROR( "The entity does not have a physics actor." );
		return Result::INVALID_ARGS;
	}

	physics::CActor *pActor = pEntity->m_vecpPhysicsActor[0];

	if( !pActor )
	{
		LOG_PRINT_ERROR( "The entity's actor in NULL." );
		return Result::INVALID_ARGS;
	}

	physics::CFixedJointDesc fixed_joint_desc;
	fixed_joint_desc.pActor[0] = pActor;
	fixed_joint_desc.pActor[1] = NULL;
	fixed_joint_desc.MaxForce  = max_force;
	fixed_joint_desc.MaxTorque = max_torque;

	physics::CScene *pScene = pStage->GetPhysicsScene();
	if( !pScene )
		return Result::UNKNOWN_ERROR;

	physics::CJoint *pJoint = pScene->CreateJoint( fixed_joint_desc );

	if( !pJoint )
	{
		LOG_PRINT_ERROR( "Failed to create a fixed joint." );
		return Result::UNKNOWN_ERROR;
	}

	pEntity->m_vecpPhysicsJoint.push_back( pJoint );

	return Result::SUCCESS;
}


Result::Name StageMiscUtility::GlueEntities(
		EntityHandle<>& entity0,
		EntityHandle<>& entity1,
		float max_force,
		float max_torque )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CCopyEntity> pEntity0 = entity0.Get();
	shared_ptr<CCopyEntity> pEntity1 = entity1.Get();

	if( !pEntity0 || pEntity0->m_vecpPhysicsActor.empty()
	 || !pEntity1 || pEntity1->m_vecpPhysicsActor.empty() )
	{
		return Result::INVALID_ARGS;
	}

	physics::CActor *pActor0 = pEntity0->m_vecpPhysicsActor[0];
	physics::CActor *pActor1 = pEntity1->m_vecpPhysicsActor[0];

	if( !pActor0 || !pActor1 )
		return Result::UNKNOWN_ERROR;

	physics::CFixedJointDesc fixed_joint_desc;
	fixed_joint_desc.pActor[0] = pActor0;
	fixed_joint_desc.pActor[1] = pActor1;
	fixed_joint_desc.MaxForce  = max_force;
	fixed_joint_desc.MaxTorque = max_torque;

	physics::CScene *pScene = pStage->GetPhysicsScene();
	if( !pScene )
		return Result::UNKNOWN_ERROR;

	physics::CJoint *pJoint = pScene->CreateJoint( fixed_joint_desc );

	if( !pJoint )
		return Result::UNKNOWN_ERROR;

	pEntity0->m_vecpPhysicsJoint.push_back( pJoint );

	return Result::SUCCESS;
}


Result::Name StageMiscUtility::GlueEntities(
		const char *entity0_name,
		const char *entity1_name,
		float max_force,
		float max_torque )
{
	if( !entity0_name || strlen(entity0_name) == 0
	 || !entity1_name || strlen(entity1_name) == 0 )
	{
		return Result::INVALID_ARGS;
	}

	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return Result::UNKNOWN_ERROR;

	CCopyEntity *pEntity0 = pStage->GetEntitySet()->GetEntityByName( entity0_name );
	CCopyEntity *pEntity1 = pStage->GetEntitySet()->GetEntityByName( entity1_name );

	weak_ptr<CCopyEntity> p0 = pEntity0->Self();
	weak_ptr<CCopyEntity> p1 = pEntity1->Self();

	return GlueEntities( EntityHandle<>(p0), EntityHandle<>(p1), max_force, max_torque );
}


Result::Name StageMiscUtility::ConnectEntitiesWithRevoluteJoint(
		const char *entity0_name,
		const char *entity1_name,
		const Vector3& axis,
		const Vector3& anchor,
		float max_force,
		float max_torque )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return Result::UNKNOWN_ERROR;

	CCopyEntity *pEntity0 = GetEntityFromEntityName( entity0_name, *pStage );
	CCopyEntity *pEntity1 = GetEntityFromEntityName( entity1_name, *pStage );

	if( !pEntity0 || !pEntity1 )
		return Result::UNKNOWN_ERROR;

	physics::CActor *pActor0 = GetPrimaryActor( *pEntity0 );
	physics::CActor *pActor1 = GetPrimaryActor( *pEntity1 );

	if( !pActor0 || !pActor1 )
		return Result::UNKNOWN_ERROR;

	return Result::SUCCESS;
}


void StageMiscUtility::CreateSkybox( const std::string& mesh_resource_path, const std::string& texture_resource_path )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return;// EntityHandle<>();

	if( 0 < mesh_resource_path.length() )
	{
		// Load mesh
	}

	CCopyEntityDesc desc;

	BaseEntityHandle basehandle;
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
void StageMiscUtility::CreateSkysphere( const std::string& texture_resource_path )
{
	CCopyEntityDesc desc;

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__DefaultSkysphere__" );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose = Matrix34Identity();

	SphereDesc sphere_desc;
	sphere_desc.radii[0] = sphere_desc.radii[1] = sphere_desc.radii[2] = 5.0f;
	sphere_desc.num_segments = 6;
	sphere_desc.num_sides    = 12;
	MeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator.reset( new SphereMeshGenerator(sphere_desc) );
	desc.MeshObjectHandle.Load( sphere_desc );

	CCopyEntity *pEntity = pStage->CreateEntity( desc );
}
*/


EntityHandle<> StageMiscUtility::CreateStaticGeometry( const std::string& resource_path )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return EntityHandle<>();

	EntityHandle<> entity = pStage->LoadStaticGeometryFromFile( resource_path );

	return entity;
}


EntityHandle<> StageMiscUtility::CreateStaticWater( const string& model, const string& name, const Vector3& position )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return EntityHandle<>();

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__StaticLiquidWater__" );

	CCopyEntityDesc desc;
	desc.strName           = name;
	desc.WorldPose         = Matrix34Identity();
	desc.pBaseEntityHandle = &basehandle;

	CCopyEntity *pStaticWater = pStage->CreateEntity( desc );
	if( !pStaticWater )
		return EntityHandle<>();

	EntityHandle<> entity( pStaticWater->Self() );

	return entity;
}


// Creates a particle generator
// The particle generator keeps generating smoke particles
EntityHandle<> StageMiscUtility::CreateStaticSmokeSource( const Vector3& pos,
			const SFloatRGBAColor& color, float diameter, float rise_speed, float thickness, float density,
			const std::string& entity_attributes_name )
{
/*	CCopyEntityDesc desc;

	if( entity_attributes_name.length() == 0 )
		entity_attributes_name = "__StaticSmokeSource__";
//		entity_attributes_name = "__DefaultSmokeTrail__";

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( entity_attributes_name );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose.vPosition = pos;

	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	if( pEntity )
		return EntityHandle<>( pEntity->pSelf );
	else*/
		return EntityHandle<>();
}


EntityHandle<> StageMiscUtility::CreateNonCollidableEntityFromMesh(
	MeshHandle& mesh,
	const Matrix34& pose,
	const std::string& entity_name
	)
{
	const char *entity_attributes_name = "__NonCollidable__";

	Vector3 vel = Vector3(0,0,0);

	EntityHandle<> entity = CreateNamedEntity( entity_name, entity_attributes_name, pose, vel, NULL, mesh );
	CCopyEntity *pEntityRawPtr = entity.GetRawPtr();
	if( !pEntityRawPtr )
		return EntityHandle<>();

	const std::shared_ptr<BasicMesh> pMesh = mesh.GetMesh();
	if( !pMesh )
	{
		return EntityHandle<>();
	}

	AABB3 aabb = pMesh->GetAABB();

	pEntityRawPtr->m_MeshHandle = mesh;
	pEntityRawPtr->local_aabb = aabb;
	pEntityRawPtr->world_aabb.TransformCoord( aabb, pEntityRawPtr->GetWorldPose().vPosition );

	return entity;
}


EntityHandle<> StageMiscUtility::CreateNonCollidableEntityFromMesh(
	const char *mesh_resource_path,
	const Matrix34& pose,
	const std::string& entity_name
	)
{
	MeshHandle mesh;
	bool mesh_loaded = mesh.Load( mesh_resource_path );

	if( !mesh_resource_path )
		return EntityHandle<>();

	return CreateNonCollidableEntityFromMesh(
		mesh,
		pose,
		entity_name
		);
}


} // namespace amorphous



//=============================================================================
// StageEntityUtility
//=============================================================================

#include "amorphous/Graphics/MeshContainerRenderMethod.hpp"


namespace amorphous
{

Result::Name StageEntityUtility::SetShader( EntityHandle<>& entity, const std::string& shader, const std::string& technique, const std::string& subset, int lod )
{
	shared_ptr<CCopyEntity> pEntity = entity.Get();
	if( !pEntity )
		return Result::INVALID_ARGS;

	const int max_shader_lod = 16;
	if( lod < 0 || max_shader_lod <= lod )
		return Result::INVALID_ARGS;

	SubsetRenderMethod subset_render_method;
	subset_render_method.m_ShaderDesc.ResourcePath = shader;
	subset_render_method.m_Technique.SetTechniqueName( technique.c_str() );

	if( !pEntity->m_pMeshRenderMethod )
		pEntity->m_pMeshRenderMethod.reset( new MeshContainerRenderMethod );

	if( 0 == subset.length() )
	{
//		vector<SubsetRenderMethod>& vecRenderMethod
//			= pEntity->m_pMeshRenderMethod->MeshRenderMethod();
//
//		while( (int)vecRenderMethod.size() <= lod )
//			vecRenderMethod.push_back( SubsetRenderMethod() );
//
//			vecRenderMethod[lod] = subset_render_method;
		pEntity->m_pMeshRenderMethod->PrimaryMeshRenderMethod() = subset_render_method;
	}
	else
	{
		vector< map<string,SubsetRenderMethod> >& mapRenderMethodMap
			= pEntity->m_pMeshRenderMethod->SubsetRenderMethodMaps();

		while( (int)mapRenderMethodMap.size() <= lod )
			mapRenderMethodMap.push_back( map<string,SubsetRenderMethod>() );

		mapRenderMethodMap[lod][subset] = subset_render_method;
	}

	return Result::SUCCESS;
}



static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    ShaderParameter<float> param )
{
	pStaticGeometry->SetFloatShaderParam( param );
}


static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    ShaderParameter<SFloatRGBAColor> param )
{
	pStaticGeometry->SetColorShaderParam( param );
}


static void SetShaderParamToStaticGeometry( CStaticGeometryBase *pStaticGeometry,
										    ShaderParameter<TextureParam> param )
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

	ShaderParameter<T> param(parameter_name);
	param.Parameter() = value;

	SetShaderParamToStaticGeometry( pStaticGeometry, param );
}


template<typename T>
static void SetShaderParamLoaderToEntity( EntityHandle<>& entity, const char *parameter_name, T value )
{
	CCopyEntity *pEntity = entity.GetRawPtr();
	if( !pEntity )
		return;

	if( pEntity->pBaseEntity->GetArchiveObjectID() == BaseEntity::BE_STATICGEOMETRY )
	{
		SetShaderParamToStaticGeometryEntity( pEntity, parameter_name, value );
	}

	shared_ptr<MeshContainerRenderMethod> pRenderMethod
		= pEntity->m_pMeshRenderMethod;

	if( !pRenderMethod )
	{
		return;
	}

	bool found_param_loader = false;
	for( size_t i=0; i<pRenderMethod->RenderMethodsAndSubsetIndices().size(); i++ )
	{
		SubsetRenderMethod& subset_render_method = pRenderMethod->RenderMethodsAndSubsetIndices()[i].first;

		for( size_t j=0; j<subset_render_method.m_vecpShaderParamsLoader.size(); j++ )
		{
			if( typeid(*subset_render_method.m_vecpShaderParamsLoader[j]) != typeid(ShaderVariableLoader<T>) )
				continue;

			// found a shader variable loader
			ShaderVariableLoader<T> *pLoader
				= dynamic_cast< ShaderVariableLoader<T> *>( subset_render_method.m_vecpShaderParamsLoader[j].get() );

			if( pLoader
			 && pLoader->GetParamName() == parameter_name )
			{
				pLoader->SetParamValue( value );
				found_param_loader = true;
			}
		}
	}

	if( !found_param_loader )
	{
		// no shader variable loader for the parameter with parameter_name was found
		// - register a new variable loader
		ShaderParameter<T> param(parameter_name);
		param.Parameter() = value;

		shared_ptr< ShaderVariableLoader<T> > pVarLoader( new ShaderVariableLoader<T>(param) );

		pRenderMethod->SetShaderParamsLoaderToAllMeshRenderMethods( pVarLoader );
	}
}


/**
 Useful if the entity has a single shader, technique, and surface.
*/
void SetFloatShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, float value )
{
	SetShaderParamLoaderToEntity( entity, parameter_name, value );
}


void SetColorShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, const SFloatRGBAColor& value )
{
	SetShaderParamLoaderToEntity( entity, parameter_name, value );
}


void SetTextureShaderParamToEntity( EntityHandle<> entity, const char *parameter_name, const char *tex_path )
{
//	ShaderParameter<TextureParam> tex_param( parameter_name );
//	tex_param.Parameter().m_Desc.ResourcePath = tex_path;
	TextureParam tex_param;
	tex_param.m_Desc.ResourcePath = tex_path;
	bool tex_loaded = tex_param.m_Handle.Load( tex_param.m_Desc );
	SetShaderParamLoaderToEntity( entity, parameter_name, tex_param );
}

} // namespace amorphous
