#include "EntitySplitter.hpp"
#include "StageUtility.hpp"
#include "amorphous/Graphics/Mesh/ConvexMeshSplitter.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/Graphics/GraphicsResources.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerator.hpp"
#include "amorphous/Physics/Actor.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using std::shared_ptr;


class EmptyMeshGenerator : public MeshGenerator
{
public:

public:

	EmptyMeshGenerator() {}

	Result::Name Generate() { return Result::SUCCESS; }
};


static void CreateCopyAsEditableMesh( MeshHandle& src, MeshHandle& dest )
{
	bool copy_loaded = false;
	LOG_PRINT_VERBOSE( "Creating a custom mesh." );
	shared_ptr<GraphicsResourceEntry> pEntry = src.GetEntry();
	if( !pEntry )
		return;

	shared_ptr<MeshResource> pMeshResource = pEntry->GetMeshResource();
	const MeshResourceDesc *pMeshDesc = dynamic_cast<const MeshResourceDesc *>( &(pMeshResource->GetDesc()) );
	if( pMeshDesc )
	{
		// Create a copy of mesh desc
		MeshResourceDesc copy_mesh_desc = *pMeshDesc;

		// Change the type to 'editable'
		copy_mesh_desc.LoadOptionFlags |= MeshLoadOption::CUSTOM_MESH;

		copy_loaded = dest.Load( copy_mesh_desc );
	}
}


Result::Name EntitySplitter::CreateEntities(
	shared_ptr<CCopyEntity> pSourceEntity,
	const Plane& split_plane,
	const MeshSplitResults& split_results,
	const EntitySplitterParams& params,
	EntityHandle<>& dest0,
	EntityHandle<>& dest1
	)
{
	CCopyEntity *pSrcEntity = pSourceEntity.get();
	if( !pSrcEntity )
		return Result::UNKNOWN_ERROR;

	float source_entity_mass = 1.0f;
	physics::CActor *pActor = pSrcEntity->GetPrimaryPhysicsActor();
	if( pActor )
		source_entity_mass = (float)pActor->GetMass();

	// How to update AABBs?

	MeshResourceDesc mesh_desc;
	mesh_desc.LoadOptionFlags = MeshLoadOption::CUSTOM_MESH;
	MeshHandle dest_meshes[2];

	shared_ptr<CStage> pStage = pSrcEntity->GetStage()->GetWeakPtr().lock();
	if( !pStage )
		return Result::UNKNOWN_ERROR;

	// Save the properties of the source entity before destroying it
	const Matrix34 source_entity_world_pose = pSrcEntity->GetWorldPose();
	string source_entity_name = pSrcEntity->GetName();
	string source_base_entity = pSrcEntity->GetBaseEntity()->GetName();

	// Terminate the source entity to avoid overlaps with the entities created
	// in front of & behind the split plane.
	if( params.destroy_source_entity )
		pStage->TerminateEntity( pSrcEntity );

	for( int i=0; i<2; i++ )
	{
		// Mesh desc is not valid unless it has either a resource path or a mesh generator,
		// so we just set an arbitrary string to make it valid.
		// Commented out; does not work - the framework tries to load the mesh as a file and fails.
//		mesh_desc.ResourcePath = source_entity_name + ( (i==0) ? "-mesh-front" : "-mesh-back" );

		mesh_desc.pMeshGenerator.reset( new EmptyMeshGenerator );

		bool mesh_loaded = dest_meshes[i].Load( mesh_desc );
		shared_ptr<BasicMesh> pMesh = dest_meshes[i].GetMesh();
		if( !mesh_loaded || !pMesh )
		{
			LOG_PRINT_ERROR( "Failed to create a custom mesh." );
			return Result::UNKNOWN_ERROR;
		}

		shared_ptr<CustomMesh> pDestCustomMesh = GetCustomMesh( pMesh );
		if( pDestCustomMesh )
		{
			// Overwrite the content of the custom mesh
			(*pDestCustomMesh) = (i==0) ? (*split_results.m_pFrontMesh) : (*split_results.m_pBackMesh);
		}
		else
		{
			LOG_PRINT_ERROR( "Failed to get a custom mesh from the created mesh." );
			continue;
		}

		StageMiscUtility stage_util( pStage );
		float mass = source_entity_mass * 0.5f;
		EntityHandle<>& dest = stage_util.CreateEntityFromConvexMesh(
			dest_meshes[i],
			source_entity_world_pose,
			mass,
			"default",
			source_entity_name + ( (i==0) ? params.front_entity_name_suffix : params.back_entity_name_suffix ),
			source_base_entity
			);

//		EntityHandle<> dest;
		if( !dest.Get() )
		{
			LOG_PRINT_ERROR( "Failed to create an entity from a convex mesh." );
			continue;
		}

		if(i==0)
			dest0 = dest;
		else
			dest1 = dest;
	}

	return Result::SUCCESS;
}


Result::Name EntitySplitter::Split(
	EntityHandle<>& src,
	const Plane& split_plane,
	const EntitySplitterParams& params,
	EntityHandle<>& dest0,
	EntityHandle<>& dest1
	)
{
	LOG_PRINT( "Entered" );

	std::shared_ptr<CCopyEntity> pSrc = src.Get();
	if( !pSrc )
	{
		LOG_PRINT_ERROR( "The source entity is invalid." );
		return Result::INVALID_ARGS;
	}

	Result::Name res = Result::SUCCESS;

	CCopyEntity& source_entity = *pSrc;

	LOG_PRINTF(( "Entered (src: %s).", source_entity.GetName().c_str() ));

	// Does source_entity have a mesh?
	std::shared_ptr<BasicMesh> pMesh = source_entity.m_MeshHandle.GetMesh();

	if( !pMesh )
	{
		LOG_PRINTF_ERROR(( "The source entity '%s' does not have a mesh.", source_entity.GetName().c_str() ));
		return Result::INVALID_ARGS;
	}

	std::shared_ptr<CustomMesh> pCustomMesh = GetCustomMesh( pMesh );
//		= boost::dynamic_pointer_cast<CustomMesh,BasicMesh>(pMesh->);

	MeshHandle copy_mesh;
	if( !pCustomMesh )
	{
		CreateCopyAsEditableMesh( source_entity.m_MeshHandle, copy_mesh );
		pCustomMesh = GetCustomMesh( copy_mesh.GetMesh() );
	}

	if( !pCustomMesh )
	{
		// Still no custom mesh - return an error
		LOG_PRINT_ERROR( "The source entity's mesh is no a custom mesh, or failed to create a custom mesh." );
		return Result::UNKNOWN_ERROR;
	}

	CustomMesh& src_custom_mesh = *pCustomMesh;

	// Is source_entity a rigid body?

	// Is source_entity a supported convex shape (i.e. box, sphere, or convex shape)?

//		source_entity.GetProertyTree().Get( "split.split_count" );
//		source_entity.GetProertyTree().Get( "split.num_max_splits" );

	ConvexMeshSplitter convex_mesh_splitter;

	LOG_PRINT_VERBOSE( "Splitting the mesh..." );

	Result::Name split_res = convex_mesh_splitter.SplitMesh( src_custom_mesh, source_entity.GetWorldPose(), split_plane );
	if( split_res != Result::SUCCESS )
	{
		// Either the src mesh is in front of or behind the plane and not crossing it.
//		LOG_PRINT_ERROR( "Failed to split the mesh." );
		return split_res;
	}

	const MeshSplitResults& split_results = convex_mesh_splitter.GetSplitResults();

	if( !split_results.m_pFrontMesh
	 || !split_results.m_pBackMesh )
	{
		LOG_PRINT_ERROR( "Front/back mesh is missing." );
		return Result::UNKNOWN_ERROR;
	}

	LOG_PRINTF_VERBOSE(( "front mesh: %d vertices, %d indices", split_results.m_pFrontMesh->GetNumVertices(), split_results.m_pFrontMesh->GetNumIndices() ));
	LOG_PRINTF_VERBOSE(( "back mesh: %d vertices, %d indices",  split_results.m_pBackMesh->GetNumVertices(),  split_results.m_pBackMesh->GetNumIndices() ));

	split_results.m_pFrontMesh->UpdateAABBs();
	split_results.m_pBackMesh->UpdateAABBs();

	static int s_count = 0;
	src_custom_mesh.DumpToTextFile( fmt_string("./.debug/%d_split_mesh_src.txt",s_count) );
	split_results.m_pFrontMesh->DumpToTextFile( fmt_string("./.debug/%d_split_mesh_dest_front.txt",s_count) );
	split_results.m_pBackMesh->DumpToTextFile(  fmt_string("./.debug/%d_split_mesh_dest_back.txt",s_count) );
	s_count += 1;

	CreateEntities( pSrc, split_plane, split_results, params, dest0, dest1 );

	return res;
}


} // namespace amorphous
