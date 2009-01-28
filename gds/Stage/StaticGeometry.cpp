#include "StaticGeometry.h"
#include "BSPTree.h"
#include "Stage.h" // used in CStaticGeometry::CreateCollisionGeometry() and dtor

#include "Graphics/Shader/ShaderManager.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/Camera.h"
#include "Graphics/D3DXMeshObject.h"

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



//================================================================================
// CShaderContainer
//================================================================================

bool CShaderContainer::Load()
{
	m_ShaderHandle.Load( m_Desc );

	// load textures specified as texture params
	for( size_t i=0; i<m_ParamGroup.m_Texture.size(); i++ )
	{
		CTextureParam& tex_param = m_ParamGroup.m_Texture[i].Parameter();
		tex_param.m_Handle.Load( tex_param.m_Desc );
	}

	return true;
}


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



//================================================================================
// Some Global Functions for Tree Construction
//================================================================================

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


void CStaticGeometry::UpdateResources( const CCamera& rCam )
{
	CNonLeafyAABTree<CMeshSubset>& mesh_subset_tree = m_Archive.m_MeshSubsetTree;

	float dist_margin_factor = 1.5f;

	// compute a sphere that contains camera with some margin
	float r = rCam.GetFarClip() / cos( rCam.GetFOV() * 0.5f ) * dist_margin_factor;
	Sphere cam_sphere = Sphere(rCam.GetPosition(),r);

	/// Used during runtime
	/// - Holds indices of nodes to check for visibility
	m_vecNodesToCheckRU.push_back(0);

	while( 0 < m_vecNodesToCheckRU.size() )
	{
		const CAABNode& node = mesh_subset_tree.GetNode( m_vecNodesToCheckRU.back() );
		m_vecNodesToCheckRU.pop_back();

		const float node_radius = node.aabb.CreateBoundingSphere().radius;
		const Vector3 vNodeCenterPos = node.aabb.GetCenterPosition();
		float dist = Vec3Length( vNodeCenterPos - rCam.GetPosition() );
		if( dist < ( cam_sphere.radius + node_radius ) * dist_margin_factor )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				const CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				CStaticGeometryMeshHolder& mesh_holder = m_Archive.m_vecMesh[subset.MeshIndex];

				GraphicsResourceState::Name state = mesh_holder.m_Mesh.GetEntryState();
				if( state == GraphicsResourceState::RELEASED )
				{
					// load the mesh
//					mesh_holder.m_Mesh.LoadAsync( mesh_holder.m_Desc );
				}
				else if( state == GraphicsResourceState::LOADED )
				{
					CD3DXMeshObjectBase *pMesh = mesh_holder.m_Mesh.GetMesh().get();
					if( !pMesh )
						continue;

					for( size_t j=0; j<subset.vecMaterialIndex.size(); j++ )
					{
						// mesh is loaded
						// see if the textures on the mesh are loaded

						const AABB3& mesh_subset_aabb = pMesh->GetAABB( subset.vecMaterialIndex[j] );
						const float mesh_subset_radius = mesh_subset_aabb.CreateBoundingSphere().radius;
						Vector3 vMeshSubsetCenterPos = mesh_subset_aabb.GetCenterPosition();
						float mesh_subset_dist = Vec3Length( vMeshSubsetCenterPos - rCam.GetPosition() );

						if( mesh_subset_dist < ( cam_sphere.radius + mesh_subset_radius ) * dist_margin_factor )
						{
							// load the texture
							CD3DXMeshObjectBase::CMeshMaterial& mat = pMesh->Material(subset.vecMaterialIndex[j]);
							const size_t num_textures = mat.Texture.size();
							for( size_t k=0; k<num_textures; k++ )
							{
								if( mat.Texture[k].GetEntryState() == GraphicsResourceState::RELEASED )
									mat.LoadTextureAsync( k );
							}
						}
					}
				}
			}
		}

		if( !node.IsLeaf() )
		{
			for( int i=0; i<2; i++ )
			{
				const CAABNode& child_node = mesh_subset_tree.GetNode( node.child[i] );

				if( cam_sphere.IntersectsWith( child_node.aabb.CreateBoundingSphere() ) )
					m_vecNodesToCheckRU.push_back( node.child[i] );
			}
		}
	}
}

/*
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
m_PrevShaderTechinqueIndex(-1),
m_pTriangleMesh(NULL),
m_pTriangleMeshActor(NULL)
{}


CStaticGeometry::~CStaticGeometry()
{
	if( m_pTriangleMesh )
	{
		if( m_pTriangleMeshActor )
			m_pStage->GetPhysicsScene()->ReleaseActor( m_pTriangleMeshActor );

		PhysicsEngine().ReleaseTriangleMesh( m_pTriangleMesh );
	}
}


/*
void CStaticGeometry::SetGlobalParams()
{
}
*/

bool CStaticGeometry::Render( const CCamera& rCam, const unsigned int EffectFlag )
{
	this->UpdateResources( rCam );

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

	m_pStage->PauseTimer();
	double elapsed_time_in_stage_at_start = m_pStage->GetElapsedTime();

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
		bool shader_loaded = container.Load();
	}

	// load meshes
	bool load_all_meshes_at_startup_time = true;
	if( load_all_meshes_at_startup_time )
	{
		for( size_t i=0; i<m_Archive.m_vecMesh.size(); i++ )
		{
			m_Archive.m_vecMesh[i].Load();
		}
	}
	else
	{
		// modify mesh and textures desc to make them async resources

		// load the meshes synchronously and textures asynchronously
		for( size_t i=0; i<m_Archive.m_vecMesh.size(); i++ )
		{
//			m_Archive.m_vecMesh[i].m_Desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
			m_Archive.m_vecMesh[i].m_Desc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;
			m_Archive.m_vecMesh[i].Load();
		}
	}

	// register mesh collision to physics simulator
	// loaded from a separate archive in the db
	CreateCollisionGeometry( *(m_pStage->GetPhysicsScene()) );

	m_Archive.m_MeshSubsetTree.WriteToFile( "./debug/sg_aabtree-" + fnop::get_nopathfilename(db_filename) + ".txt" );

	m_pStage->ResumeTimer();
	double elapsed_time_in_stage_at_end = m_pStage->GetElapsedTime();

	return true;
}


int CStaticGeometry::ClipTrace( STrace& tr )
{
	physics::CRay ray;
	ray.Origin = *tr.pvStart;
	Vector3 vStoG = *tr.pvGoal- *tr.pvStart;
	Vec3Normalize( ray.Direction, vStoG );

/*	physics::CRaycastHit rayhit;
	rayhit;
	CShape *pShape m_pStage->GetPhysicsScene()->RaycastClosestShape( ray, rayhit, 0, FLT_MAX );
	if( pShape->GetType() == PhysShape::TriangleMesh );
*/
	return 0;
}


inline static void SetRGBAColor( const SFloatRGBAColor& src_color, CShaderParameter< vector<float> >& dest_param )
{
	dest_param.Parameter().resize( 4 ); // 4 float values for rgba
	dest_param.Parameter()[0] = src_color.fRed;
	dest_param.Parameter()[1] = src_color.fGreen;
	dest_param.Parameter()[2] = src_color.fBlue;
	dest_param.Parameter()[3] = src_color.fAlpha;
}


void CStaticGeometry::SetAmbientColor( const SFloatRGBAColor& ambient_color )
{
	// set ambient color to all the shaders
	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
		CShaderManager *pMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		if( pMgr )
		{
			CShaderParameter< vector<float> > param( "g_AmbientColor" );
			SetRGBAColor( ambient_color, param );

			pMgr->SetParam( param );
		}
	}
}


void CStaticGeometry::SetFogColor( const SFloatRGBAColor& color )
{
	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
		CShaderManager *pMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		if( pMgr )
		{
			CShaderParameter< vector<float> > param( "g_vTerrainFadeColor" );
			SetRGBAColor( color, param );

			pMgr->SetParam( param );
		}
	}
}


void CStaticGeometry::SetFogStartDist( float dist )
{
	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
		CShaderManager *pMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		if( pMgr )
		{
			CShaderParameter< vector<float> > param( "g_fFogStart" );
			param.Parameter().resize( 1 ); // 4 float values for rgba
			param.Parameter()[0] = dist;

			pMgr->SetParam( param );
		}
	}
}


void CStaticGeometry::SetFogEndDist( float dist )
{
	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
	{
		CShaderManager *pMgr = m_Archive.m_vecShaderContainer[i].m_ShaderHandle.GetShaderManager();
		if( pMgr )
		{
			CShaderParameter< vector<float> > param( "g_fFarClip" );
			param.Parameter().resize( 1 ); // 4 float values for rgba
			param.Parameter()[0] = dist;

			pMgr->SetParam( param );
		}
	}
}


physics::CActor *CStaticGeometry::CreateCollisionGeometry( physics::CScene& physics_scene )
{
	string db_filename = m_strFilename;

	CBinaryDatabase<string> db;
	bool db_open = db.Open( db_filename );

	if( !db_open )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + db_filename );
		return NULL;
	}

	physics::CStream collision_geometry_stream;
	bool retrieved = db.GetData( CStaticGeometryDBKey::CollisionGeometryStream, collision_geometry_stream );

	if( !retrieved )
	{
		LOG_PRINT_WARNING( " - Collision geometry stream was not found in the database: " + db_filename );
		return NULL;
	}


	physics::CTriangleMeshShapeDesc desc;

	desc.pTriangleMesh
		= m_pTriangleMesh
		= PhysicsEngine().CreateTriangleMesh( collision_geometry_stream );

	physics::CActorDesc actor_desc;
	
	// create as a static triangle mesh
	actor_desc.BodyDesc.Flags |= BodyFlag::Static;
	actor_desc.vecpShapeDesc.push_back( &desc );

	m_pTriangleMeshActor = physics_scene.CreateActor( actor_desc );

	return m_pTriangleMeshActor;
}
