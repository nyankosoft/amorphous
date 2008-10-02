#include "StaticGeometry.h"
#include "BSPTree.h"

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


void CShaderContainer::SetParams()
{
	CShaderManager *pMgr = m_ShaderHandle.GetShaderManager();
	for( size_t i=0; i<m_ParamGroup.m_Float.size(); i++ )
		pMgr->SetParam( m_ParamGroup.m_Float[i] );

	for( size_t i=0; i<m_ParamGroup.m_Texture.size(); i++ )
	{
		if( m_ParamGroup.m_Texture[i].Parameter().m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
			pMgr->SetTexture( (int)i, m_ParamGroup.m_Texture[i].Parameter().m_Handle );
	}
}



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



static int s_MaxDepth = 0;

void BuildLA_r( vector<SNode_f>& vecNode, vector<SPlane>& vecPlane, int index, int depth )
{
	SNode_f& rNode = vecNode[index];

	// at this point, only an aabb has been set to rNode
	// child & plane will be set during the following process

	Vector3 vExtents = rNode.aabb.GetExtents();
	int axis;
	if( vExtents[1] < vExtents[0] )
	{
		if( vExtents[2] < vExtents[0] )
			axis = 0;
		else
			axis = 2;
	}
	else
	{
		if( vExtents[2] < vExtents[1] )
			axis = 1;
		else
			axis = 2;
	}

	SPlane plane;
	plane.dist = rNode.aabb.GetCenterPosition()[axis];
	plane.type = (char)axis;
	plane.normal = Vector3(0,0,0);
	plane.normal[axis] = 1.0f;

	rNode.sPlaneIndex = (short)vecPlane.size();
	vecPlane.push_back( plane );

//	LOG_PRINT( "node[" + to_string(index) + "] - plane index: " + to_string(rNode.sPlaneIndex) );

	if( s_MaxDepth <= depth )
	{
		rNode.sFrontChild = CONTENTS_EMPTY;
		rNode.sBackChild = CONTENTS_EMPTY;
		rNode.sCellIndex = 0;
		return;
	}
	else
	{
		// resurse down to make the rest of the tree

		SNode_f child_node[2];
		child_node[0] = child_node[1] = rNode;

		child_node[0].aabb.vMin[axis] = plane.dist;
		child_node[1].aabb.vMax[axis] = plane.dist;

		short child_index[2];
		child_index[0] = (short)vecNode.size();
		child_index[1] = (short)vecNode.size() + 1;

		rNode.sFrontChild = child_index[0];
		rNode.sBackChild  = child_index[1];

		vecNode.push_back( child_node[0] );
		vecNode.push_back( child_node[1] );

		BuildLA_r( vecNode, vecPlane, child_index[0], depth+1 );
		BuildLA_r( vecNode, vecPlane, child_index[1], depth+1 );
	}
}


void BuildAxisAlignedBSPTree( CBSPTree& rDestBSPTree, const AABB3& rBoundingBox, const int depth )
{
	LOG_PRINT( " - creating an entity tree" );

	vector<SNode_f> vecNode;
	vector<SPlane> vecPlane;

	vecNode.reserve( (size_t)pow( 2.0, depth ) );
	vecPlane.reserve( (size_t)pow( 2.0, depth ) );

	s_MaxDepth = depth;

	SNode_f root_node;
	root_node.aabb = rBoundingBox;

	// set the seed of the tree
	vecNode.push_back( root_node );

	BuildLA_r( vecNode, vecPlane, 0, 1 );

	// copy the tree to the dest buffer
//	rDestBSPTree.m_paNode = new SNode_f [vecNode.size()];
//	for( size_t i=0; i<vecNode.size(); i++ ) rDestBSPTree.m_paNode[i] = vecNode[i];

//	rDestBSPTree.m_paPlane = new SPlane [vecPlane.size()];
//	for( size_t i=0; i<vecPlane.size(); i++ ) rDestBSPTree.m_paPlane[i] = vecPlane[i];

	rDestBSPTree.Init( &vecNode[0], (int)vecNode.size(), &vecPlane[0], (int)vecPlane.size() );

	LOG_PRINT( " - an entity tree has been created (" + to_string((int)vecNode.size()) + " nodes)" );
}



//================================================================================
// CStaticGeometryBase
//================================================================================

void CStaticGeometryBase::MakeEntityTree( CBSPTree& bsptree )
{
	AABB3 aabb = GetAABB();
	aabb.MergeAABB( AABB3( Vector3( -300000.0f,      0.0f, -300000.0f ),
		                   Vector3(  300000.0f,  30000.0f,  300000.0f ) ) );

	BuildAxisAlignedBSPTree( bsptree, aabb, 8 );
}




//================================================================================
// CStaticGeometry
//================================================================================

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

/*
void CStaticGeometry::SetGlobalParams()
{
}
*/

bool CStaticGeometry::Render( const CCamera& rCam, const unsigned int EffectFlag )
{
	// reset world transform
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

//	SetGlobalParams();

	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
		m_Archive.m_vecShaderContainer[i].SetParams();
	

	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
//		m_Archive.m_vecShaderContainer[i].m_pShaderManager->SetWorldTransform( matWorld );
		CShaderManager *pShaderMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		if( pShaderMgr )
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

		if( true )
//		if( rCam.ViewFrustumIntersectsWith( node.aabb ) )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				const CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				if( !rvecMesh[subset.MeshIndex].m_Mesh.IsLoaded() )
					continue;

				CD3DXMeshObjectBase *pMesh = rvecMesh[subset.MeshIndex].m_Mesh.GetMesh().get();

				CShaderContainer& shader_container = rvecShaderContainer[subset.ShaderIndex];
				if( !shader_container.m_ShaderHandle.IsLoaded() )
					continue;

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
