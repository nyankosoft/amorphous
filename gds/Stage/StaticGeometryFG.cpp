#include "StaticGeometryFG.hpp"
#include "StaticGeometryArchiveFG.hpp"

#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"
#include "Support/lfs.hpp"
#include "Support/macro.h"

#include "bsptree.hpp"
#include "EntitySet.hpp"


#ifndef TEST_STATICGEOMETRY_FG

#include "Stage.hpp"
#include "ScreenEffectManager.hpp"

#include "Physics/Actor.hpp"
#include "Physics/RaycastHit.hpp"
//#include "JigLib/JL_PhysicsManager.hpp"
//#include "JigLib/JL_PhysicsActor.hpp"
//#include "JigLib/JL_ShapeDesc_TriangleMesh.hpp"
//#include "JigLib/TriangleMesh.hpp"

#endif /* TEST_STATICGEOMETRY_FG */


namespace amorphous
{

using namespace std;


CStaticGeometryFG::CStaticGeometryFG( CStage *pStage )
:
CStaticGeometryBase( pStage ),
m_pShaderManager(NULL),
m_pTriangleMesh(NULL),
m_FogStartDist(0),
m_FogEndDist(0),
m_FarClipDist(0)
{
}


CStaticGeometryFG::~CStaticGeometryFG()
{
	Release();
}


void CStaticGeometryFG::ReleaseGraphicsResources()
{
	Release();
}


void CStaticGeometryFG::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	LoadFromFile( m_strFilename, true );
}


void CStaticGeometryFG::Release()
{
	m_pSkyboxMesh.reset();
	SafeDelete( m_pShaderManager );

	// m_pTriangleMesh is not released here - it is released by physics simulator
}


void CStaticGeometryFG::SetDepthFogParams()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	pd3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);	// use vertex fog
//	pd3dDev->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_NONE);

	pd3dDev->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_LINEAR);	// use pixel fog
	pd3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE );

//	pd3dDev->SetRenderState(D3DRS_FOGCOLOR, 0xFF606050 );	//set color of the fog
	pd3dDev->SetRenderState(D3DRS_FOGCOLOR, m_FogColor.GetARGB32() );	//set color of the fog

///	float fMinDist = 6000.0f;
///	float fMaxDist = 9000.0f;
//	float fMinDist = 600.0f;
//	float fMaxDist = 900.0f;
	float fMinDist = m_FogStartDist;
	float fMaxDist = m_FogEndDist;//m_FarClipDist - 5000.0f;
	pd3dDev->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fMinDist));
	pd3dDev->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&fMaxDist));
}


void CStaticGeometryFG::RenderTerrainMesh( const Camera& rCamera )
{
	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	m_pShaderManager->SetWorldTransform( Matrix44Identity() );

	// alpha-blending settings 
/*	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'
*/
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
	if( !pEffect )
		return;

	m_pShaderManager->SetTexture( 1, m_FadeTex.GetTexture() );

	m_pShaderManager->SetTexture( 2, m_TileTexture.GetTexture() );

	m_pShaderManager->SetTexture( 3, m_NormalMapForWaterSurface.GetTexture() );

	hr = pEffect->SetValue( "g_vEyePos", &rCamera.GetPosition(), sizeof(float) * 3 );

//	hr = pEffect->SetTechnique( "QuickTest" );
///	hr = pEffect->SetTechnique( m_Technique.c_str() );

//	hr = pEffect->SetVector( "g_FogColor",		(D3DXVECTOR4 *)&m_FogColor );

	SFloatRGBAColor ambient_color = m_AmbientColor;
	if( m_pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PseudoNightVision )
	{
		ambient_color.red		+= 0.25f;
		ambient_color.green	+= 0.25f;
		ambient_color.blue		+= 0.25f;
	}

	hr = pEffect->SetVector( "g_AmbientColor",	(D3DXVECTOR4 *)&ambient_color );

	if( FAILED(hr) )
		return;
	size_t i, num_mesh_groups = m_vecMeshGroup.size();
	for( i=0; i<num_mesh_groups; i++ )
	{
		// bring near & far clip planes slightly forward
		// to avoid z-fighting with ground objects
		Matrix44 matProj;
		float z_shift = rCamera.GetNearClip() * m_vecMeshGroup[i].m_fDepthShiftFactor; // 0.1f;
		matProj = Matrix44PerspectiveFoV_LH( rCamera.GetFOV(), rCamera.GetAspectRatio(),
			rCamera.GetNearClip() - z_shift, rCamera.GetFarClip() - z_shift );

		m_pShaderManager->SetProjectionTransform( matProj );
	
		// set the shader technique for this mesh group
		pEffect->SetTechnique( m_vecMeshGroup[i].m_vecShaderTechnique[0].GetTechniqueName() );

		int mesh_index;//, num_meshes = m_vecpMesh.size();
		for( mesh_index = m_vecMeshGroup[i].m_StartIndex;
			 mesh_index <= m_vecMeshGroup[i].m_EndIndex;
			 mesh_index++ )
		{
//			m_vecpMesh[mesh_index]->UpdateVisibility( rCamera );
//			m_vecpMesh[mesh_index]->Render( pEffect, pEffect->GetParameterByName( 0, "Texture0" ) );

			BasicMesh *pMesh = m_vecMesh[mesh_index].GetMesh().get();
			pMesh->UpdateVisibility( rCamera );
//			pMesh->Render( pEffect, pEffect->GetParameterByName( 0, "Texture0" ) );
			pMesh->Render( *m_pShaderManager );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}


void CStaticGeometryFG::RenderSkybox( const Camera& rCamera )
{
	if( !m_pSkyboxMesh )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	Matrix44 matWorld = Matrix44Scaling( 10.0f, 10.0f, 10.0f );

	const Vector3 vPos = rCamera.GetPosition();
	matWorld(0,3) = vPos.x;
	matWorld(1,3) = vPos.y;
	matWorld(2,3) = vPos.z;
	matWorld(3,3) = 1.0f;

	m_pShaderManager->SetWorldTransform( matWorld );

	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
	if( !pEffect )
		return;

//	hr = pEffect->SetValue( "g_vEyePos", &(g_CameraController.GetPosition()), sizeof(float) * 3 );

//	hr = pEffect->SetTechnique( "NullShader" );
//	hr = pEffect->SetTechnique( "SkyBox" );
	hr = pEffect->SetTechnique( "SkySphereFG" );

	if( FAILED(hr) )
	{
		ONCE( g_Log.Print( WL_ERROR, "CStaticGeometryFG::RenderSkybox() - invalid shader technique" ) );
		return;
	}

	float cam_height = vPos.y;
	hr = pEffect->SetFloat( "g_CameraHeight", cam_height );

//	pd3dDevice->SetFVF( TEXTUREVERTEX::FVF );

	GraphicsDevice().Disable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	if( m_pSkyboxMesh )
	{
//		m_pSkyboxMesh->Render( pEffect, pEffect->GetParameterByName( 0, "Texture0" ) ); 
		m_pSkyboxMesh->Render( *m_pShaderManager ); 
	}

	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}


bool CStaticGeometryFG::Render( const Camera& rCamera, const unsigned int EffectFlag )
{
	Matrix44 view, proj;
	rCamera.GetCameraMatrix( view );
	rCamera.GetProjectionMatrix( proj );

	m_pShaderManager->SetWorldViewProjectionTransform( Matrix44Identity(), view, proj );


	FixedFunctionPipelineManager().SetProjectionTransform( proj );	// depth fog needs projection transform info

//	m_pShaderManager->SetTechnique( m_aShaderTechID[SHADER_TECH_TERRAIN] );
//	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();
//	pEffect->SetTechnique( "" );
/*
	if( m_Technique == "SGMiniatureFG" )
	{
		SetDepthFogParams();

		RenderTerrainMesh( rCamera );
	}*/

//	else
//	{
		// default render mode
		RenderSkybox( rCamera );

		SetDepthFogParams();

		RenderTerrainMesh( rCamera );
//	}
	
	m_pShaderManager->SetWorldViewProjectionTransform( Matrix44Identity(), view, proj );

	return true;
}


bool CStaticGeometryFG::LoadCollisionMesh( CStaticGeometryArchiveFG& archive )
{
#ifndef TEST_STATICGEOMETRY_FG
/*
	CJL_ShapeDesc_TriangleMesh mesh_desc;
	CJL_PhysicsActorDesc desc;

//	if( 0 < archive.m_CollisionMeshArchiveFilename.length() )
	if( true )
	{
		// load triangle mesh stored in the binary database file with the string key 'gs_CollisionMeshKey'
//		mesh_desc.BSPTreeTriangleMeshArchiveFilename = archive.m_CollisionMeshArchiveFilename;
		mesh_desc.BSPTreeTriangleMeshArchiveFilename = m_strFilename + "::" + CStaticGeometryArchiveFG::ms_CollisionMeshTreeKey;
	}
	else
	{
		mesh_desc.pvecvVertex			= &archive.m_CollisionMeshArchive.vecVertex;
		mesh_desc.pveciIndex			= &archive.m_CollisionMeshArchive.vecIndex;
		vector<short> vecsMaterialIndex;
		CopyVector( vecsMaterialIndex, archive.m_CollisionMeshArchive.vecMaterialIndex );
		mesh_desc.pvecsMaterialIndex	= &vecsMaterialIndex;
///		mesh_desc.MinimumCellVolume = 8000000.0f;	// 200[m] cube
///		mesh_desc.NumMaxTrianglesPerCell = 16;
//		mesh_desc.MinimumCellVolume = 1000000000.0f;	// 1000[m] cube
//		mesh_desc.NumMaxTrianglesPerCell = 256;
		mesh_desc.MinimumCellVolume = 1000000000.0f;	// 1[km] cube
		mesh_desc.NumMaxTrianglesPerCell = 800;
		mesh_desc.RecursionStopCond = "||";
	}

	// since triangles are much larger than other objects
	// edge optimization will not be effective and only take extra time for pre-processing
	mesh_desc.OptimizeEdgeCollision = false;

	desc.ActorFlag = JL_ACTOR_STATIC;
	desc.iCollisionGroup = ENTITY_COLL_GROUP_STATICGEOMETRY;
	desc.vecpShapeDesc.push_back( &mesh_desc );

	CJL_PhysicsActor *pActor = m_pStage->GetPhysicsManager()->CreateActor( desc );
	if( !pActor )
	{
		LOG_PRINT_WARNING( " - failed to create a collision mesh" );
		return false;
	}

	m_pTriangleMesh = (CTriangleMesh *)pActor->GetShape(0);
*/
#endif /* TEST_STATICGEOMETRY_FG */

	return true;
}


bool CStaticGeometryFG::LoadFromFile( const std::string& db_filename, bool bLoadGraphicsOnly )
{
	m_strFilename = db_filename;

	CBinaryDatabase<string> db;
	bool db_open = db.Open( db_filename );

	if( !db_open )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + db_filename );
		return false;
	}

	CStaticGeometryArchiveFG archive;

	// load the main archive
	// - most of the data is included in this archive
	db.GetData( "Main", archive );

	bool db_has_collision_mesh;
	if( db.KeyExists( CStaticGeometryArchiveFG::ms_CollisionMeshTreeKey ) )
		db_has_collision_mesh = true;
	else
		db_has_collision_mesh = false;

//	archive.LoadFromFile( filename );

	m_AABB = archive.GetAABB();

	// retrieve mesh groups from archive
	m_vecMeshGroup = archive.m_vecMeshGroup;

	m_AmbientColor = archive.m_AmbientColor;
	m_FogColor     = archive.m_FogColor;
	m_FogStartDist = archive.m_FogStartDist;
	m_FarClipDist  = archive.m_FarClipDist;
	m_FogEndDist   = archive.m_FarClipDist - 2000.0f;

	// close db
	// - can be done later but must be before LoadCollisionMesh()
	//   since phys mgr needs to open and get the data
	db.Close();

	// load meshes

	C3DMeshModelArchive mesh_archive;

	size_t i, num_meshes = archive.m_vecMeshArchiveKey.size();

	MeshResourceDesc mesh_desc;
	m_vecMesh.resize( num_meshes );
	for( i=0; i<num_meshes; i++ )
	{
		mesh_desc.ResourcePath = db_filename + "::" + archive.m_vecMeshArchiveKey[i];

		m_vecMesh[i].Load( mesh_desc );
	}

	// load skybox(skysphere)
	if( 0 < archive.m_SkyboxMeshArchive.GetVertexSet().GetNumVertices() 
	 && 0 < archive.m_SkyboxMeshArchive.GetMaterial().size() )
	{
		m_pSkyboxMesh.reset( new BasicMesh );
		bool loaded = m_pSkyboxMesh->LoadFromArchive( archive.m_SkyboxMeshArchive, db_filename, 0 );
		if( !loaded )
			m_pSkyboxMesh.reset();
	}

	// load texture for fade
	if( m_pSkyboxMesh )
	{
		vector<CMMA_Material>& skybox_mat = archive.m_SkyboxMeshArchive.GetMaterial();
		if( 0 < skybox_mat.size()
		 && 0 < skybox_mat[0].vecTexture.size() )
		{
			string fade_tex_filepath = skybox_mat[0].vecTexture[0].strFilename;
			lfs::insert_before_extension( fade_tex_filepath, "_fade" );
			m_FadeTex.Load( fade_tex_filepath );
		}
	}

	// register mesh collision to physics simulator
	// loaded from a separate archive in the db
	if( db_has_collision_mesh )
        LoadCollisionMesh( archive );

	if( !m_pShaderManager )
        LoadShaderFromFile( "./Shader/SGViewerFG.fx" );

	// load global overlay texture
	m_TileTexture.Load( archive.m_strTileTextureFilename );

	// load fake bump for water surface
//	if( m_Technique == "SGTerrainTexBlendWithWater" )
	if( 0 < m_vecMeshGroup.size()
	 && m_vecMeshGroup[0].m_vecShaderTechnique[0].GetTechniqueName() == "SGTerrainTexBlendWithWater" )
	{
		string tex_filepath = "Stage\\Texture\\watersurf_nmap.bmp";
		m_NormalMapForWaterSurface.Load( tex_filepath );
	}

	return true;
}


int CStaticGeometryFG::ClipTrace(STrace& tr)
{
#ifndef TEST_STATICGEOMETRY_FG

	if( !m_pTriangleMesh )
	{
//		PrintLogOnce( "CStaticGeometryFG::ClipTrace() - no collision mesh" );
		return 0;
	}

	// clip trace using triangle mesh (experiment)
	// - Not tested since converted from JibLib to Physics module.
	ONCE( LOG_PRINT_WARNING( " Calling CShape::Raycast(). Not tested since converted from JibLib to Physics module." ) );
	physics::CRay ray;
	ray.Origin = tr.vStart;
	ray.Direction = Vec3GetNormalized( tr.vGoal - tr.vStart );
	physics::CRaycastHit ray_hit;
	m_pTriangleMesh->GetShape(0)->Raycast( ray, 1000000.0f, 0, ray_hit, true );

#endif  /* TEST_STATICGEOMETRY_FG */

	return 0;
}


bool CStaticGeometryFG::LoadShaderFromFile( const char *pFilename )
{
	SafeDelete( m_pShaderManager );
	m_pShaderManager = new CShaderManager();

	bool result = m_pShaderManager->LoadShaderFromFile( pFilename );

	if( !result )
		SafeDelete( m_pShaderManager );

	return result;
}

/*
void CStaticGeometryFG::RenderTreeNode( node )
{
	//D3DXMATRIX matProj;
	//float z_shift = rCamera.GetNearClip() * m_vecMeshGroup[i].m_fDepthShiftFactor; // 0.1f;
	//D3DXMatrixPerspectiveFovLH( &matProj, rCamera.GetFOV(), rCamera.GetAspectRatio(),
	//	rCamera.GetNearClip() - z_shift, rCamera.GetFarClip() - z_shift );

	vector<D3DXMATRIX> m_vecProjectionMatrix;

	size_t i, num_subsets = 0;
	int prev_proj_matrix_index = -1;
	int prev_tech_index = -1;
	for( i=0; i<num_subsets; i++ )
	{
		CMeshSubset& subset;
		// bring near & far clip planes slightly forward
		// to avoid z-fighting with ground objects

		if( subset.ProjectionMatrixIndex != prev_proj_matrix_index )
			m_pShaderManager->SetProjectionTransform( m_vecProjectionMatrix[subset.ProjectionMatrixIndex] );

		if( subset.ShaderTechniqueIndex != prev_tech_index )
			m_pShaderManager->SetTeqhnique();

		// set the shader technique for this mesh group
		pEffect->SetTechnique( m_vecMeshGroup[i].m_vecShaderTechnique[0].GetTechniqueName() );

		CD3DXMeshObjectBase *pMesh = m_vecMesh[subset.MeshIndex].GetMesh().get();
//		pMesh->UpdateVisibility( rCamera );
		pMesh->RenderSubset( subset.MaterialIndex, *m_pShaderManager );
	}
}
*/


} // namespace amorphous
