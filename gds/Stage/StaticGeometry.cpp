#include "StaticGeometry.hpp"
#include "bsptree.hpp"
#include "Stage.hpp" // used in CStaticGeometry::CreateCollisionGeometry() and dtor

#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Graphics/GraphicsResourceCacheManager.hpp"

#include "Physics/PhysicsEngine.hpp"
#include "Physics/Stream.hpp"
#include "Physics/TriangleMeshShapeDesc.hpp"
#include "Physics/Scene.hpp"
#include "Physics/Actor.hpp"
#include "Physics/RaycastHit.hpp"

#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"
#include "Support/fnop.hpp"
#include "Support/macro.h"

using namespace std;
using namespace boost;
using namespace physics;


const char *CStaticGeometryDBKey::Main                    = "Main";
const char *CStaticGeometryDBKey::CollisionGeometryStream = "CollisionGeometryStream";
const char *CStaticGeometryDBKey::GraphicsMeshArchive     = "GraphicsMeshArchive";
//const char *CStaticGeometryDBKey::Shaders               = "Shaders";
//const char *CStaticGeometryDBKey::MeshSubsetTree        = "MeshSubsetTree";



void CreateCachedResources()
{
	CTextureResourceDesc tex_desc;
	tex_desc.Format    = TextureFormat::A8R8G8B8;
	tex_desc.Width     = 1024;
	tex_desc.Height    = 1024;

	tex_desc.MipLevels = 1;//gs_TextureMipLevels;

	const int num_textures_to_preload = 8;
	for( int i=0; i<num_textures_to_preload; i++ )
	{
		GraphicsResourceCacheManager().AddCache( tex_desc );
	}
}


// temporary measure to avoid D3DXFilterTexture() calls in async loading at runtime
static void ForceTextureMipMapLevelsToOne( shared_ptr<CBasicMesh> pMesh )
{
	if( !pMesh )
		return;

	for( int i=0; i<pMesh->GetNumMaterials(); i++ )
	{
		CMeshMaterial& mat = pMesh->Material(i);

		for( size_t j=0; j<mat.TextureDesc.size(); j++ )
		{
			mat.TextureDesc[j].MipLevels = 1;
		}
	}
}



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

static float gs_fDistMarginFactor = 1.5f;

void CStaticGeometry::UpdateMeshSubsetResources( CMeshSubset& subset, const CCamera& rCam, const Sphere& cam_sphere )
{
	CStaticGeometryMeshHolder& mesh_holder = m_Archive.m_vecMesh[subset.MeshIndex];

	GraphicsResourceState::Name state = mesh_holder.m_Mesh.GetEntryState();
	if( state == GraphicsResourceState::RELEASED )
	{
		// load the mesh
//		mesh_holder.m_Mesh.LoadAsync( mesh_holder.m_Desc );
//		m_setResourcesToLoad.insert( mesh_holder.m_Mesh.GetEntry() );
	}
	else if( state == GraphicsResourceState::LOADED )
	{
		CBasicMesh *pMesh = mesh_holder.m_Mesh.GetMesh().get();
		if( !pMesh )
			return;

		for( size_t j=0; j<subset.vecMaterialIndex.size(); j++ )
		{
			// mesh is loaded
			// see if the textures on the mesh are loaded

			const AABB3& mesh_subset_aabb = pMesh->GetAABB( subset.vecMaterialIndex[j] );
			const float mesh_subset_radius = mesh_subset_aabb.CreateBoundingSphere().radius;
			Vector3 vMeshSubsetCenterPos = mesh_subset_aabb.GetCenterPosition();
			float mesh_subset_dist = Vec3Length( vMeshSubsetCenterPos - rCam.GetPosition() );
			float dbg_tex_load_height = 30000.0f;//3000.0f;

			if( mesh_subset_dist < ( cam_sphere.radius + mesh_subset_radius ) * gs_fDistMarginFactor// )
			 && rCam.GetPosition().y < dbg_tex_load_height )
			{
				// load the texture
				CMeshMaterial& mat = pMesh->Material(subset.vecMaterialIndex[j]);
				const size_t num_textures = mat.Texture.size();
				for( size_t k=0; k<num_textures; k++ )
				{
					if( mat.Texture[k].GetEntryState() == GraphicsResourceState::RELEASED )
					{
						mat.LoadTextureAsync( k );
//						m_setResourcesToLoad.insert( mat.Texture[k].GetEntry() );
					}
				}
			}
			else if( dbg_tex_load_height < rCam.GetPosition().y )
			{
				CMeshMaterial& mat = pMesh->Material(subset.vecMaterialIndex[j]);
				const size_t num_textures = mat.Texture.size();
				for( size_t k=0; k<num_textures; k++ )
				{
					if( mat.Texture[k].GetEntryState() == GraphicsResourceState::LOADED )
					{
						mat.Texture[k].Release();
//						m_setReleasedResources.insert( mat.Texture[k].GetEntry() );
					}
				}
			}
		}
	}
}


void CStaticGeometry::UpdateResources( const CCamera& rCam )
{
	CNonLeafyAABTree<CMeshSubset>& mesh_subset_tree = m_Archive.m_MeshSubsetTree;

	// compute a sphere that contains camera with some margin
	float r = rCam.GetFarClip() / cos( rCam.GetFOV() * 0.5f ) * gs_fDistMarginFactor;
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
		if( dist < ( cam_sphere.radius + node_radius ) * gs_fDistMarginFactor )
		{
			const size_t num_subsets = node.veciGeometryIndex.size();
			for( size_t i=0; i<num_subsets; i++ )
			{
				CMeshSubset& subset
					= mesh_subset_tree.GetGeometryBuffer()[node.veciGeometryIndex[i]];

				UpdateMeshSubsetResources( subset, rCam, cam_sphere );
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


void CStaticGeometry::UpdateResources_NonHierarchical( const CCamera& rCam )
{
	vector<CMeshSubset>& vecMeshSubset = m_Archive.m_MeshSubsetTree.GetGeometryBuffer();
	const int num_mesh_subsets = (int)vecMeshSubset.size();

	if( num_mesh_subsets == 0 )
		return; // no mesh subsets - happens when the static geometry was not properly load.

	// compute a sphere that contains camera with some margin
	float r = rCam.GetFarClip() / cos( rCam.GetFOV() * 0.5f ) * gs_fDistMarginFactor;
	Sphere cam_sphere = Sphere(rCam.GetPosition(),r);

	int i, index;
	int num_mesh_subsets_to_check_per_frame = 2;
	for( i=0; i<num_mesh_subsets_to_check_per_frame; i++ )
	{
		index = (m_MeshSubsetToCheckNext + i) % num_mesh_subsets;

		UpdateMeshSubsetResources( vecMeshSubset[index], rCam, cam_sphere );
	}

	m_MeshSubsetToCheckNext = index;
}


/*
void IsReadyToDisplay()
{
	/// Used during runtime
	/// - Holds indices of nodes to check for visibility
	std::vector<int> m_vecNodesToCheck;
	m_vecNodesToCheck.push_back(0);

	float gs_fDistMarginFactor = 1.5f;

	while( 0 < m_vecNodesToCheck.size() )
	{
		const CAABNode& node = mesh_subset_tree.GetNode( m_vecNodesToCheck.back() );
		m_vecNodesToCheck.pop_back();

		float dist = Vec3Length( vMeshCenterPos - rCam.GetPosition() );
		if( dist < ( rCam.GetFarClip() + mesh_radius ) * gs_fDistMarginFactor )
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

				if( dist < ( rCam.GetFarClip() + mesh_radius ) * gs_fDistMarginFactor )
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
m_pTriangleMeshActor(NULL),
m_MeshSubsetToCheckNext(0)
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
//	this->UpdateResources( rCam );

	// AABBs of nodes are not correct
	// - Use AABBs of mesh subsets
	this->UpdateResources_NonHierarchical( rCam );

//	SetGlobalParams();

	for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
		m_Archive.m_vecShaderContainer[i].SetParams();

	// reset world transform
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
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

	vector<int> vecVisibleMatIndex;

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

				CBasicMesh *pMesh = rvecMesh[subset.MeshIndex].m_Mesh.GetMesh().get();

				CShaderContainer& shader_container = rvecShaderContainer[subset.ShaderIndex];
				if( !shader_container.m_ShaderHandle.IsLoaded() )
					continue;

				CShaderManager &shader_mgr = *shader_container.m_ShaderHandle.GetShaderManager();// *(shader_container.m_pShaderManager.get());

				shader_mgr.SetTechnique( shader_container.m_vecTechniqueHandle[subset.ShaderTechniqueIndex] );

				// render all the materials in the subset
/*				pMesh->RenderSubsets( shader_mgr, subset.vecMaterialIndex );*/

				// collect visible materials(surfaces or triangle sets) of the mesh
				vecVisibleMatIndex.resize( 0 );
				for( size_t j=0; j<subset.vecMaterialIndex.size(); j++ )
				{
					if( rCam.ViewFrustumIntersectsWith( pMesh->GetAABB( subset.vecMaterialIndex[j] ) ) )
						vecVisibleMatIndex.push_back( subset.vecMaterialIndex[j] );
				}

				pMesh->RenderSubsets( shader_mgr, vecVisibleMatIndex );
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

//	double elapsed_time_in_stage_at_start = m_pStage->GetElapsedTime();

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


	bool load_all_meshes_at_startup_time = false;
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

//		create cached textures only once
		ONCE( CreateCachedResources() );

		// load the meshes synchronously and textures asynchronously
		for( size_t i=0; i<m_Archive.m_vecMesh.size(); i++ )
		{
//			m_Archive.m_vecMesh[i].m_Desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
			m_Archive.m_vecMesh[i].m_Desc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;
			m_Archive.m_vecMesh[i].Load();

			// temporary measure to avoid D3DXFilterTexture() calls at runtime
			shared_ptr<CBasicMesh> pMesh = m_Archive.m_vecMesh[i].m_Mesh.GetMesh();
			ForceTextureMipMapLevelsToOne( pMesh );
		}
	}

	// register mesh collision to physics simulator
	// loaded from a separate archive in the db
	// - Do NULL check for 'm_pStage' since the stage is not set in CStaticGeometryCompiler.
	if( m_pStage )
	{
		CreateCollisionGeometry( *(m_pStage->GetPhysicsScene()) );
	}

	m_Archive.m_MeshSubsetTree.WriteToFile( "./debug/sg_aabtree-" + fnop::get_nopathfilename(db_filename) + ".txt" );

//	double elapsed_time_in_stage_at_end = m_pStage->GetElapsedTime();

	return true;
}


int CStaticGeometry::ClipTrace( STrace& tr )
{
	if( !m_pTriangleMeshActor )
		return 0;

	physics::CRay ray;
	ray.Origin = *tr.pvStart;
	Vector3 vStoG = *tr.pvGoal- *tr.pvStart;
	Vec3Normalize( ray.Direction, vStoG );
	float ray_length = Vec3Dot( ray.Direction, vStoG );

	if( ray_length < 0.000001f )
		return 0;

	// For now, assume that the static geometry actor is always composed of a single triangle mesh shape
	if( 0 == m_pTriangleMeshActor->GetNumShapes() )
		return 0;
	
	physics::CShape *pShape = m_pTriangleMeshActor->GetShape( 0 );
	bool interested_in_only_the_first_hit = true;

	physics::CRaycastHit rayhit;
	bool hit = pShape->Raycast( ray, ray_length, 0, rayhit, interested_in_only_the_first_hit );

	if( !hit )
		return 0;

	const float fraction = Vec3Dot( ray.Direction, rayhit.WorldImpactPos - *tr.pvStart ) / ray_length;
	if( 1.0f <= fraction )
		return 0;

	// the ray hit the static geometry

	if( 1000000.0f < fabs(rayhit.WorldImpactPos.x)
	 || 1000000.0f < fabs(rayhit.WorldImpactPos.y)
	 || 1000000.0f < fabs(rayhit.WorldImpactPos.z) )
	{
		int sth_wrong_with_impact_pos_is = 1;
	}

	tr.vEnd         = rayhit.WorldImpactPos;
	tr.fFraction    = fraction;
	tr.plane.normal = rayhit.WorldNormal;
	tr.plane.dist   = Vec3Dot( rayhit.WorldNormal, rayhit.WorldImpactPos );

//	rayhit;
//	CShape *pShape m_pStage->GetPhysicsScene()->RaycastClosestShape( ray, rayhit, 0, FLT_MAX );
//	if( pShape->GetType() == PhysShape::TriangleMesh );

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
