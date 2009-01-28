#include "StaticGeometryFG.h"
#include "StaticGeometryArchiveFG.h"

#include "Graphics/Shader/ShaderManager.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/Camera.h"
#include "Graphics/D3DXMeshObject.h"

#include "Support/Serialization/BinaryDatabase.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/StringAux.h"
#include "Support/fnop.h"
#include "Support/macro.h"

#include "BSPTree.h"
#include "EntitySet.h"


#ifndef TEST_STATICGEOMETRY_FG

#include "Stage.h"
#include "ScreenEffectManager.h"

#include "JigLib/JL_PhysicsManager.h"
#include "JigLib/JL_PhysicsActor.h"
#include "JigLib/JL_ShapeDesc_TriangleMesh.h"
#include "JigLib/TriangleMesh.h"

#endif /* TEST_STATICGEOMETRY_FG */

using namespace std;


CStaticGeometryFG::CStaticGeometryFG( CStage *pStage )
:
CStaticGeometryBase( pStage ),
m_pSkyboxMesh(NULL),
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


void CStaticGeometryFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	LoadFromFile( m_strFilename, true );
}


void CStaticGeometryFG::Release()
{
	SafeDelete( m_pSkyboxMesh );
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


void CStaticGeometryFG::RenderTerrainMesh( const CCamera& rCamera )
{
	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pShaderManager->SetWorldTransform( matWorld );

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
		ambient_color.fRed		+= 0.25f;
		ambient_color.fGreen	+= 0.25f;
		ambient_color.fBlue		+= 0.25f;
	}

	hr = pEffect->SetVector( "g_AmbientColor",	(D3DXVECTOR4 *)&ambient_color );

	if( FAILED(hr) )
		return;
	size_t i, num_mesh_groups = m_vecMeshGroup.size();
	for( i=0; i<num_mesh_groups; i++ )
	{
		// bring near & far clip planes slightly forward
		// to avoid z-fighting with ground objects
		D3DXMATRIX matProj;
		float z_shift = rCamera.GetNearClip() * m_vecMeshGroup[i].m_fDepthShiftFactor; // 0.1f;
		D3DXMatrixPerspectiveFovLH( &matProj, rCamera.GetFOV(), rCamera.GetAspectRatio(),
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

			CD3DXMeshObjectBase *pMesh = m_vecMesh[mesh_index].GetMesh().get();
			pMesh->UpdateVisibility( rCamera );
//			pMesh->Render( pEffect, pEffect->GetParameterByName( 0, "Texture0" ) );
			pMesh->Render( *m_pShaderManager );
		}
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
}


void CStaticGeometryFG::RenderSkybox( const CCamera& rCamera )
{
	if( !m_pSkyboxMesh )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	const D3DXVECTOR3 vPos = rCamera.GetPosition();

	D3DXMatrixScaling( &matWorld, 10.0f, 10.0f, 10.0f );

	matWorld._41 = vPos.x;
	matWorld._42 = vPos.y;
	matWorld._43 = vPos.z;
	matWorld._44 = 1;

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

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

//	m_pSkyboxMesh->Render( pEffect, pEffect->GetParameterByName( 0, "Texture0" ) ); 
	m_pSkyboxMesh->Render( *m_pShaderManager ); 

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}


bool CStaticGeometryFG::Render( const CCamera& rCamera, const unsigned int EffectFlag )
{
	D3DXMATRIX matWorld, matCamera, matProj;
	D3DXMatrixIdentity( &matWorld );
	rCamera.GetCameraMatrix( matCamera );
	rCamera.GetProjectionMatrix( matProj );

	m_pShaderManager->SetWorldViewProjectionTransform( matWorld, matCamera, matProj );


	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );	// depth fog needs projection transform info

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
	
	m_pShaderManager->SetWorldViewProjectionTransform( matWorld, matCamera, matProj );

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

	HRESULT hr;

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

	CD3DXMeshObject *pMeshObject;
	C3DMeshModelArchive mesh_archive;

	size_t i, num_meshes = archive.m_vecMeshArchiveKey.size();

	CMeshResourceDesc mesh_desc;
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
		pMeshObject= new CD3DXMeshObject;
		hr = pMeshObject->LoadFromArchive( archive.m_SkyboxMeshArchive, db_filename, 0 );
		if( SUCCEEDED(hr) )
			m_pSkyboxMesh = pMeshObject;
		else
			SafeDelete( pMeshObject );
	}

	// load texture for fade
	if( m_pSkyboxMesh )
	{
		vector<CMMA_Material>& skybox_mat = archive.m_SkyboxMeshArchive.GetMaterial();
		if( 0 < skybox_mat.size()
		 && 0 < skybox_mat[0].vecTexture.size() )
		{
			string fade_tex_filepath = skybox_mat[0].vecTexture[0].strFilename;
			fnop::append_to_body( fade_tex_filepath, "_fade" );
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
	m_pTriangleMesh->ClipTrace( tr );

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
