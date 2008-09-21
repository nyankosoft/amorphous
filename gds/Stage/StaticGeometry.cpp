#include "StaticGeometry.h"

#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/Camera.h"
#include "3DCommon/D3DXMeshObject.h"

#include "Physics/PhysicsEngine.h"
#include "Physics/Stream.h"
#include "Physics/TriangleMeshShapeDesc.h"
#include "Physics/Scene.h"
#include "Physics/Actor.h"

#include "Support/Serialization/BinaryDatabase.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/fnop.h"
#include "Support/macro.h"

using namespace std;
using namespace boost;
using namespace physics;


const char *CStaticGeometryDBKey::Main                    = "Main";
const char *CStaticGeometryDBKey::CollisionGeometryStream = "CollisionGeometryStream";
const char *CStaticGeometryDBKey::GraphicsMeshArchive     = "GraphicsMeshArchive";
//const char *CStaticGeometryDBKey::Shaders               = "Shaders";
//const char *CStaticGeometryDBKey::MeshSubsetTree        = "MeshSubsetTree";



/**
  About m_ShaderIndex & m_ShaderTechniqueIndex
  - Default value -1 indicates that texture is global auxiliary texture, 
    and it is set at the begenning of CStaticGeometry::Render() once

*/
CAuxiliaryTexture::CAuxiliaryTexture()
:
m_ShaderIndex(-1),
m_ShaderTechniqueIndex(-1),
m_TextureStage(-1)
{
}


void CAuxiliaryTexture::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_ShaderIndex;
	ar & m_ShaderTechniqueIndex;
	ar & m_TextureStage;
	ar & m_TextureName;
	ar & m_UsageDesc;
	ar & m_Desc;
}


/*
void UpdateResources()
{
	/// Used during runtime
	/// - Holds indices of nodes to check for visibility
	std::vector<int> m_vecNodesToCheck;
	m_vecNodesToCheck.push_back(0);

	float dist_margin_factor = 1.5f;

	while( 0 < m_vecNodesToCheck.size() )
	{
		const CAABNode& node = mesh_subset_tree.GetNode( m_vecNodesToCheck.back() );
		m_vecNodesToCheck.pop_back();

		float dist = Vec3Length( vMeshCenterPos - rCam.GetPosition() );
		if( dist < ( rCam.GetFarClip() + mesh_radius ) * dist_margin_factor )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				const CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				if( !mesh_is_loaded )
				{
					// load the mesh
					mesh_holder.m_Mesh.LoadAsync( mesh_holder.m_Desc );
				}
				else if( already_loaded )
				{
					CD3DXMeshObjectBase *pMesh = rvecMesh[subset.MeshIndex].m_Mesh.GetMesh().get();
					vSubsetCenterPos = pMesh->GetAABB( subset.vecMaterialIndex ).GetCenterPos();
					dist = Vec3Length( vSubsetCenterPos - rCam.GetPosition() );

					if( dist < ( rCam.GetFarClip() + mesh_radius ) * dist_margin_factor )
					{
						// load the texture
						for( num_textures )
						{
							pMesh->GetMaterial(mat_index).Texture[?].LoadAsync();
						}
					}
				}
			}
		}

		if( !node.IsLeaf() )
		{
			m_vecNodesToCheck.push_back( node.child[0] );
			m_vecNodesToCheck.push_back( node.child[1] );
		}
	}

	return true;
}


void IsReadyToDisplay()
{
	/// Used during runtime
	/// - Holds indices of nodes to check for visibility
	std::vector<int> m_vecNodesToCheck;
	m_vecNodesToCheck.push_back(0);

	float dist_margin_factor = 1.5f;

	while( 0 < m_vecNodesToCheck.size() )
	{
		const CAABNode& node = mesh_subset_tree.GetNode( m_vecNodesToCheck.back() );
		m_vecNodesToCheck.pop_back();

		float dist = Vec3Length( vMeshCenterPos - rCam.GetPosition() );
		if( dist < ( rCam.GetFarClip() + mesh_radius ) * dist_margin_factor )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				const CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				if( mesh_is_not_loaded )
					return false;

				CD3DXMeshObjectBase *pMesh = rvecMesh[subset.MeshIndex].m_Mesh.GetMesh().get();

				vSubsetCenterPos = pMesh->GetAABB( subset.vecMaterialIndex ).GetCenterPos();

				dist = Vec3Length( vSubsetCenterPos - rCam.GetPosition() );

				if( dist < ( rCam.GetFarClip() + mesh_radius ) * dist_margin_factor )
					return false;
			}
		}

		if( !node.IsLeaf() )
		{
			m_vecNodesToCheck.push_back( node.child[0] );
			m_vecNodesToCheck.push_back( node.child[1] );
		}
	}

	return true;
}
*/


CStaticGeometry::CStaticGeometry( CStage *pStage )
	:
CStaticGeometryBase( pStage ),
m_PrevShaderIndex(-1),
m_PrevShaderTechinqueIndex(-1)
{}


bool CStaticGeometry::Render( const CCamera& rCam, const unsigned int EffectFlag )
{
	// reset world transform
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
//		m_Archive.m_vecShaderContainer[i].m_pShaderManager->SetWorldTransform( matWorld );
		CShaderManager *pShaderMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		pShaderMgr->SetWorldTransform( matWorld );
	}

	CNonLeafyAABTree<CMeshSubset>& mesh_subset_tree = m_Archive.m_MeshSubsetTree;

	vector<CStaticGeometryMeshHolder>& rvecMesh = m_Archive.m_vecMesh;

	vector<CShaderContainer>& rvecShaderContainer = m_Archive.m_vecShaderContainer;

	if( m_Archive.m_vecMesh.size() == 0 )
		return false; // nothing to render

	/// Used during runtime
	/// - Holds indices of nodes to check for visibility
	std::vector<int> m_vecNodesToCheck;
	m_vecNodesToCheck.push_back(0);

	while( 0 < m_vecNodesToCheck.size() )
	{
		const CAABNode& node = mesh_subset_tree.GetNode( m_vecNodesToCheck.back() );
		m_vecNodesToCheck.pop_back();

		if( rCam.ViewFrustumIntersectsWith( node.aabb ) )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				const CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				CD3DXMeshObjectBase *pMesh = rvecMesh[subset.MeshIndex].m_Mesh.GetMesh().get();

				CShaderContainer& shader_container = rvecShaderContainer[subset.ShaderIndex];
				CShaderManager &shader_mgr = *shader_container.m_ShaderHandle.GetShaderManager();// *(shader_container.m_pShaderManager.get());

				shader_mgr.SetTechnique( shader_container.m_vecTechniqueHandle[subset.ShaderTechniqueIndex] );

				pMesh->RenderSubsets( shader_mgr, subset.vecMaterialIndex );
			}
		}

		if( !node.IsLeaf() )
		{
			m_vecNodesToCheck.push_back( node.child[0] );
			m_vecNodesToCheck.push_back( node.child[1] );
		}
	}

	return true;
}

bool CStaticGeometry::LoadFromFile( const std::string& db_filename, bool bLoadGraphicsOnly )
{
	// load resources from database

	CBinaryDatabase<string> db;
	bool db_open = db.Open( db_filename );

	if( !db_open )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + db_filename );
		return false;
	}

	m_strFilename = db_filename;

	// load the main archive
	// - most of the data is included in this archive
	db.GetData( CStaticGeometryDBKey::Main, m_Archive );

	bool db_has_collision_mesh;
	if( db.KeyExists( CStaticGeometryArchiveFG::ms_CollisionMeshTreeKey ) )
		db_has_collision_mesh = true;
	else
		db_has_collision_mesh = false;

//	HRESULT hr;

//	m_AABB = archive.GetAABB();

	// load mesh info
//	db.GetData( CStaticGeometryDBKey::GraphicsMeshes, m_vecMesh );

	// load shader info
//	db.GetData( CStaticGeometryDBKey::Shaders, m_vecShaderContainer );

	// close db
	// - can be done later but must be before LoadCollisionMesh()
	//   since phys mgr needs to open and get the data
	db.Close();

	// load resources loaded from database

	// load shaders
	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
		CShaderContainer& container = m_Archive.m_vecShaderContainer[i];
//		container.m_pShaderManager = shared_ptr<CShaderManager>( new CShaderManager() );
//		container.m_pShaderManager->LoadShaderFromFile( container.ShaderFilepath );
		bool shader_loaded = container.Load();
	}

	// load meshes
	if( /*load_all_meshes_at_startup_time == */ true )
	{
		for( size_t i=0; i<m_Archive.m_vecMesh.size(); i++ )
		{
			m_Archive.m_vecMesh[i].Load();
		}
	}

	// register mesh collision to physics simulator
	// loaded from a separate archive in the db
//	if( db_has_collision_mesh )
//		LoadCollisionMesh( archive );

	return true;
}




physics::CActor *CStaticGeometry::CreateCollisionGeometry( physics::CScene& physics_scene )
{
	string db_filename;

	CBinaryDatabase<string> db;
	bool db_open = db.Open( db_filename );

	if( !db_open )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + db_filename );
		return NULL;
	}

	physics::CStream collision_geometry_stream;
	db.GetData( CStaticGeometryDBKey::CollisionGeometryStream, collision_geometry_stream );

	/*
	physics::CTriangleMeshShapeDesc desc;

	desc.pTriangleMesh
		= PhysicsEngine.CreateTriangleMesh( collision_geometry_stream );

	physics::CActorDesc actor_desc;
	
	// create as a static triangle mesh
	actor_desc.BodyDesc.Flags |= BodyFlag::Static;
	actor_desc.vecpShapeDesc.push_back( &desc );

	physics::CActor *pActor = physics_scene.CreateActor( actor_desc );

	return pActor;
*/

	return NULL;
}
