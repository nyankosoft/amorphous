
#include "D3DXDMeshObject.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/FVF_BumpVertex.h"
#include "3DCommon/FVF_BumpWeightVertex.h"
#include "3DCommon/FVF_ColorVertex.h"
#include "3DCommon/FVF_TextureVertex.h"
#include "3DCommon/FVF_NormalVertex.h"
#include "3DCommon/FVF_WeightVertex.h"
#include "3DCommon/FVF_ShadowVertex.h"

#include "Support/fnop.h"
#include "Support/memory_helpers.h"
#include "Support/ContentStreaming/DynamicResourceManager.h"
using namespace fnop;


CD3DXDMeshObject::CD3DXDMeshObject()
:
m_pMesh(NULL),
m_ppDRTextures(NULL),
m_ppDRNormalMapTextures(NULL)
{
}


CD3DXDMeshObject::~CD3DXDMeshObject()
{
	Release();
}


void CD3DXDMeshObject::Release()
{
	SAFE_RELEASE( m_pMesh );

	SafeDeleteArray( m_ppDRTextures );
	SafeDeleteArray( m_ppDRNormalMapTextures );

	CD3DXMeshObjectBase::Release();
}


HRESULT CD3DXDMeshObject::LoadFromFile( const std::string& filename,
									    CPackedFile& packed_file )
{
	m_pPackFile = &packed_file;

	if( 0 < filename.length() )
		m_strFilename = filename;
	else
		return E_FAIL;

	C3DMeshModelArchive archive;
	bool b = archive.LoadFromFile( filename );

	if( !b )
		return E_FAIL;

	Release();

	DWORD i, iSize = 0;


	// copy the contents from the archive

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;


	void *pDestVBData, *pSrcVBData = NULL;

	// load vertex data from the archive
	LoadVertices( pSrcVBData, archive );

	const D3DVERTEXELEMENT9 *pVertexElements = GetVertexElemenets( archive.GetVertexSet() );

	DWORD num_vertices = (DWORD)archive.GetVertexSet().GetNumVertices();


	LPD3DXMESH pMesh = NULL;

	hr = D3DXCreateMesh( (DWORD)archive.GetVertexIndex().size() / 3,
		                 num_vertices,
						 0,
						 pVertexElements,
						 pd3dDev,
						 &pMesh );

	if( FAILED(hr) )
		return hr;


	// copy vertices to vertex buffer
	hr = pMesh->LockVertexBuffer( 0, &pDestVBData );

	memcpy( pDestVBData, pSrcVBData, m_iVertexSize * num_vertices );

	pMesh->UnlockVertexBuffer();

	SafeDeleteArray( pSrcVBData );

	// copy vertex indices to index buffer
	FillIndexBuffer( pMesh, archive );

	// load materials
	vector<CMMA_Material>& rvecSrcMaterial = archive.GetMaterial();

	m_dwNumMaterials = (DWORD)rvecSrcMaterial.size();

	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];


	// load AABBs that represent bounding boxes for each triangle set
	m_vecAABB.resize(m_dwNumMaterials);
	for( DWORD mat=0; mat < m_dwNumMaterials; mat++ )
		m_vecAABB[mat] = archive.GetTriangleSet()[mat].m_AABB;

	// all triangle sets are set visible by default
	m_IsVisible.resize( m_dwNumMaterials + 1, 1 );


	string tex_filename;

	m_ppDRTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
	for( i = 0; i < m_dwNumMaterials; i++ ) m_ppDRTextures[i] = NULL;

	m_ppDRNormalMapTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
	for( i = 0; i < m_dwNumMaterials; i++ ) m_ppDRNormalMapTextures[i] = NULL;

	m_DRTexFilename.resize( m_dwNumMaterials );
	m_DRNormalTexFilename.resize( m_dwNumMaterials );

	m_TexState.resize( m_dwNumMaterials, TEX_NOT_LOADED );

	// change to the directory of the mesh file
	dir_stack DirStack;
	DirStack.setdir( get_path(m_strFilename) );

	bool bLoaded = false;
	for( i=0; i<m_dwNumMaterials; i++ )
	{
		memset( &m_pMeshMaterials[i], 0, sizeof(D3DMATERIAL9) );

		float specular = rvecSrcMaterial[i].fSpecular;
		m_pMeshMaterials[i].Specular.r = specular;
		m_pMeshMaterials[i].Specular.g = specular;
		m_pMeshMaterials[i].Specular.b = specular;
		m_pMeshMaterials[i].Specular.a = specular;

		m_pMeshMaterials[i].Diffuse.r = 1.0f;
		m_pMeshMaterials[i].Diffuse.g = 1.0f;
		m_pMeshMaterials[i].Diffuse.b = 1.0f;
		m_pMeshMaterials[i].Diffuse.a = 1.0f;

		m_pMeshMaterials[i].Ambient.r = 0.25f;
		m_pMeshMaterials[i].Ambient.g = 0.25f;
		m_pMeshMaterials[i].Ambient.b = 0.25f;
		m_pMeshMaterials[i].Ambient.a = 1.00f;


		tex_filename = rvecSrcMaterial[i].SurfaceTexture.strFilename;
		if( 0 < tex_filename.length() )
		{
			m_DRTexFilename[i] = tex_filename;

			// do not load texture here
//			if( FAILED( D3DXCreateTextureFromFile( pd3dDev, tex_filename.c_str(), &m_ppMeshTextures[i] ) ) )
//				m_ppMeshTextures[i] = NULL;
		}

/*		if( !bLoaded )
		{	// texture file was not found - create default white texture 
			DWORD dwColor = 0xFFFFFFFF;
			CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_paMaterial[i].m_pSurfaceTexture) );
		}*/

		tex_filename = rvecSrcMaterial[i].NormalMapTexture.strFilename;
		if( 0 < tex_filename.length() )
		{
			m_DRNormalTexFilename[i] = tex_filename;

			// do not load texture here
//			if( FAILED( D3DXCreateTextureFromFile( pd3dDev, tex_filename.c_str(), &m_ppNormalMapTextures[i] ) ) )
//				m_ppNormalMapTextures[i] = NULL;
		}

//		m_paMaterial[i].m_NormalMap.Load( rvecSrcMaterial[i].strNormalMapTexture.c_str() );

/*		if( !bLoaded )
		{	// normal map texture was not found - create default normap map (x,y,z) = (0,0,1)
			DWORD dwColor = D3DCOLOR_ARGB( 255, 128, 128, 255 );
			CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_paMaterial[i].m_pNormalMap) );
		}*/
	}

	DirStack.prevdir();


	SetAttributeTable( pMesh, archive.GetTriangleSet() );


	LPD3DXBUFFER pAdjacencyBuffer = NULL;

	// get adjacency info
	DWORD buffer_size_factor = 300;	// 3 at least
	DWORD num_faces = pMesh->GetNumFaces();
	D3DXCreateBuffer( sizeof(DWORD) * pMesh->GetNumFaces() * buffer_size_factor, &pAdjacencyBuffer );
	DWORD *pBufferPtr = (DWORD *)pAdjacencyBuffer->GetBufferPointer();

	memset( pBufferPtr, 0, sizeof(DWORD) * pMesh->GetNumFaces() * buffer_size_factor );

	hr = pMesh->GenerateAdjacency( 0.01f, pBufferPtr );

	if( FAILED(hr) )
	{
		Release();
		return hr;
	}

	m_pMesh = pMesh;

//	m_pMeshForTest = pMesh;
//	return hr;


/*	hr = D3DXValidMesh( pMesh, pBufferPtr, NULL );
	if( hr == D3DXERR_INVALIDMESH )
	{
		assert( !"invalid mesh" );
		return hr;
	}*/


	// check the attribute tables (for debugging)
/*	D3DXATTRIBUTERANGE attrib_table[128];
	DWORD attrib_table_size;
	if( 0 < m_vecpPMesh.size() && m_vecpPMesh[0] ) {
		m_vecpPMesh[0]->GetAttributeTable( attrib_table, &attrib_table_size );
	}*/

	return hr;
}


LPDIRECT3DTEXTURE9 CD3DXDMeshObject::GetTexture( int i )
{
	if( i < 0 || m_dwNumMaterials <= (DWORD)i )
		return NULL;

	return m_ppDRTextures[i];
}


void CD3DXDMeshObject::UpdateResources()
{
	int i, num_materials = (int)m_dwNumMaterials;
	for( i=0; i<num_materials; i++ )
	{
		if( !IsMeshVisible(i) )
			continue;

		// i-th material(triangle set) is visible
		// load texture if necessary

		if( m_TexState[i] == TEX_NOT_LOADED )
		{
			DynamicResourceManager.CreateTextureFromFile_Async(
				m_DRTexFilename[i],
				*m_pPackFile,
				&m_ppDRTextures[i],
				NULL );

			// avoid double-loading
			m_TexState[i] = TEX_LOADING;
		}
	}

/*	for()
	{
		if( visible in the near future )
		{
			tex_entity.Load();
//			CreateTextureFromFile9_Async( DIRECT3D9.GetDevice(), filename,  );

		}
		else
		{
			// not visible for now - release if it is loaded
			tex_entity.Release();
		}
	}*/
}
