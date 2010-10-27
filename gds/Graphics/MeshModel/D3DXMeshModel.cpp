#include "D3DXMeshModel.hpp"
#include "Graphics/Direct3D/Conversions.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/FVF_BumpVertex.h"
#include "Graphics/FVF_BumpWeightVertex.h"
#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/FVF_NormalVertex.h"
#include "Graphics/FVF_WeightVertex.h"
#include "Graphics/FVF_ShadowVertex.h"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/lfs.hpp"
#include <assert.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace MeshModel;
//using namespace GameLib1::MeshModel;


//=========================================================================================
// CD3DXMeshModel
//=========================================================================================

CD3DXMeshModel::CD3DXMeshModel()
{
	m_pVB = NULL;
	m_pIB = NULL;

	m_iNumMaterials = 0;
	m_paMaterial = NULL;
	m_paTriangleSet = NULL;

	m_iNumBones = 0;
	m_paTransforms = NULL;

	m_pRootBone = NULL;

	m_dwFVF = 0;
	m_iNumVertices = 0;
	m_iVertexSize = 0;

	m_pVertexDecleration = NULL;
}


CD3DXMeshModel::~CD3DXMeshModel()
{
	Release();
}


void CD3DXMeshModel::LoadVertices( void*& pVBData, int& riSize, C3DMeshModelArchive& archive )
{
	CMMA_VertexSet& rVertexSet = archive.GetVertexSet();

	m_iNumVertices = rVertexSet.GetNumVertices();

	BUMPWEIGHTVERTEX *pBumpWeight;
    BUMPVERTEX *pBump;
	NORMALVERTEX *pVertex;
	SHADOWVERTEX *pShadowVert;
	WEIGHTVERTEX *pWeightVert;
	COLORVERTEX *pNVert;
	unsigned char Indices[4];

	int i, iNumVertices = rVertexSet.GetNumVertices();

	switch( rVertexSet.GetVertexFormat() )
	{
	case CMMA_VertexSet::VF_COLORVERTEX:	// unlit vertex with diffuse color

		m_dwFVF = COLORVERTEX::FVF;
		m_iVertexSize = sizeof(COLORVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new COLORVERTEX [iNumVertices] );
		pNVert = (COLORVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pNVert[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pNVert[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pNVert[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(COLORVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_BUMPVERTEX:	// unlit, textured vertex with bumpmap

		m_dwFVF = BUMPVERTEX::FVF;
		m_iVertexSize = sizeof(BUMPVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new BUMPVERTEX [iNumVertices] );
		pBump = (BUMPVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pBump[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pBump[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pBump[i].vBinormal = ToD3DXVECTOR3( rVertexSet.vecBinormal[i] );
			pBump[i].vTangent  = ToD3DXVECTOR3( rVertexSet.vecTangent[i] );
			pBump[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
			pBump[i].tex       = rVertexSet.vecTex[0][i];	// use the first texture coord
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(BUMPVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_TEXTUREVERTEX:	// unlit, textured vertex with no bumpmap

		m_dwFVF = NORMALVERTEX::FVF;
		m_iVertexSize = sizeof(NORMALVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new NORMALVERTEX [iNumVertices] );
		pVertex = (NORMALVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pVertex[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pVertex[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pVertex[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
			pVertex[i].tex       = rVertexSet.vecTex[0][i];	// use the first texture coord
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(NORMALVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_SHADOWVERTEX:

		m_dwFVF = SHADOWVERTEX::FVF;
		m_iVertexSize = sizeof(SHADOWVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new SHADOWVERTEX [iNumVertices] );
		pShadowVert = (SHADOWVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pShadowVert[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pShadowVert[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(SHADOWVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_WEIGHTVERTEX:
//		m_dwFVF = WEIGHTVERTEX::FVF;
		m_iVertexSize = sizeof(WEIGHTVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new WEIGHTVERTEX [iNumVertices] );
		pWeightVert = (WEIGHTVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pWeightVert[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pWeightVert[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pWeightVert[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
			pWeightVert[i].tex       = rVertexSet.vecTex[0][i];	// use the first texture coord

			rVertexSet.GetBlendMatrixIndices_4Bytes( i, Indices );
			pWeightVert[i].matrixIndices = D3DCOLOR_ARGB( Indices[0], Indices[1], Indices[2], Indices[3] );

			rVertexSet.GetBlendMatrixWeights_4Floats( i, (float *)&(pWeightVert[i].matrixWeights) );
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(WEIGHTVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_BUMPWEIGHTVERTEX:
		m_iVertexSize = sizeof(BUMPWEIGHTVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new BUMPWEIGHTVERTEX [iNumVertices] );
		pBumpWeight = (BUMPWEIGHTVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pBumpWeight[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pBumpWeight[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pBumpWeight[i].vBinormal = ToD3DXVECTOR3( rVertexSet.vecBinormal[i] );
			pBumpWeight[i].vTangent  = ToD3DXVECTOR3( rVertexSet.vecTangent[i] );
			pBumpWeight[i].color     = rVertexSet.vecDiffuseColor[i].GetARGB32();
			pBumpWeight[i].tex       = rVertexSet.vecTex[0][i];	// use the first texture coord

			rVertexSet.GetBlendMatrixIndices_4Bytes( i, Indices );
			pBumpWeight[i].matrixIndices = D3DCOLOR_ARGB( Indices[0], Indices[1], Indices[2], Indices[3] );

			rVertexSet.GetBlendMatrixWeights_4Floats( i, (float *)&(pBumpWeight[i].matrixWeights) );
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(BUMPWEIGHTVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	case CMMA_VertexSet::VF_SHADOWWEIGHTVERTEX:
		m_iVertexSize = sizeof(WEIGHTVERTEX);
		riSize = m_iVertexSize * iNumVertices;
		pVBData = (void *)( new WEIGHTVERTEX [iNumVertices] );
		pWeightVert = (WEIGHTVERTEX *)pVBData;
		for( i=0; i<iNumVertices; i++ )
		{
			pWeightVert[i].vPosition = ToD3DXVECTOR3( rVertexSet.vecPosition[i] );
			pWeightVert[i].vNormal   = ToD3DXVECTOR3( rVertexSet.vecNormal[i] );
			pWeightVert[i].color     = 0;
			pWeightVert[i].tex       = TEXCOORD2(0,0);

			rVertexSet.GetBlendMatrixIndices_4Bytes( i, Indices );
			pWeightVert[i].matrixIndices = D3DCOLOR_ARGB( Indices[0], Indices[1], Indices[2], Indices[3] );

			rVertexSet.GetBlendMatrixWeights_4Floats( i, (float *)&(pWeightVert[i].matrixWeights) );
		}
		DIRECT3D9.GetDevice()->CreateVertexDeclaration(WEIGHTVERTEX_DECLARATION, &m_pVertexDecleration);
		break;

	default:
		break;
	}
}


void CD3DXMeshModel::Release()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );

	SafeDeleteArray( m_paMaterial );
	SafeDeleteArray( m_paTriangleSet );
	m_iNumMaterials = 0;

	SafeDeleteArray( m_paTransforms );
	m_iNumBones = 0;
}


bool CD3DXMeshModel::LoadFromFile( const char *pFilename )
{
	m_strFilename = pFilename;

	C3DMeshModelArchive archive;
	bool b = archive.LoadFromFile(pFilename);

	if( !b )
		return false;

	return LoadFromArchive( archive );
}


bool CD3DXMeshModel::LoadFromArchive( C3DMeshModelArchive& rArchive, const string& strModelFilename )
{
	Release();

	int i, iSize = 0;
	
	if( 0 < strModelFilename.length() )
		m_strFilename = strModelFilename;

	// copy the contents from the archive

	LPDIRECT3DDEVICE9	pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	void *pDestVBData, *pSrcVBData = NULL;

	LoadVertices( pSrcVBData, iSize, rArchive );

	// create the Vertex Buffer
	hr = pd3dDev->CreateVertexBuffer( iSize, // archive.m_iVertexSize * archive.m_iNumVertices,
		                              0,
									  m_dwFVF,
									  D3DPOOL_MANAGED,
									  &m_pVB,
									  NULL);

	if( FAILED(hr) )
		return false;

	// put the vertex data to the Vertex Buffer
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pDestVBData, 0 ) ) )
		return false;

	memcpy( pDestVBData, pSrcVBData, iSize );

	m_pVB->Unlock();

	delete [] pSrcVBData;

	// create index buffer
	vector<unsigned int>& rvecVertexIndex = rArchive.GetVertexIndex();
	int iNumIndices = rArchive.GetNumVertexIndices();
	iSize = sizeof(short) * iNumIndices;

	// create the Index Buffer
	hr = pd3dDev->CreateIndexBuffer(iSize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL);

	if( FAILED(hr) )
			return false;

	unsigned short* pusIBData;

	// put the index data to the Index Buffer
    if( FAILED( m_pIB->Lock( 0, 0, (VOID**)&pusIBData, 0 ) ) )
		return false;

	for( i=0; i<iNumIndices; i++ )
		pusIBData[i] = (unsigned short)rvecVertexIndex[i];

	m_pIB->Unlock();


	vector<CMMA_Material>& rvecSrcMaterial = rArchive.GetMaterial();
	m_iNumMaterials = (int)rvecSrcMaterial.size();
	m_paMaterial = new CMM_Material [ m_iNumMaterials ];

	string strOrigFilename;
	strOrigFilename = m_strFilename;
	string strPath = lfs::get_parent_path( strOrigFilename );
	string strFilename;

	bool bLoaded;
	DWORD dwTexelColor[] = { 0xFFFFFFFF, D3DCOLOR_ARGB( 255, 128, 128, 255 ) };
	for( i=0; i<m_iNumMaterials; i++ )
	{
		size_t num_textures = rvecSrcMaterial[i].vecTexture.size();
		if( 2 < num_textures )
			num_textures = 2; // support surface color and normal map textures

		for( size_t tex=0; tex<num_textures; tex++ )
		{
			m_paMaterial[i].m_fSpecular = rvecSrcMaterial[i].fSpecular;

			// texture for surface
			if( 0 < strPath.length() )
				strFilename = strPath + "/" + rvecSrcMaterial[i].vecTexture[tex].strFilename;	// relative path
			else
				strFilename = rvecSrcMaterial[i].vecTexture[tex].strFilename;	// restore the full path

			switch( tex )
			{
			case 0:
//				m_paMaterial[i].m_SurfaceTexture.filename = strFilename;
				bLoaded = m_paMaterial[i].m_SurfaceTexture.Load( strFilename );
				break;
			case 1:
//				m_paMaterial[i].m_NormalMap.filename = strFilename;
				bLoaded = m_paMaterial[i].m_NormalMap.Load( strFilename );
				break;
			default:
				bLoaded = false;
				break;
			}

///			m_paMaterial[i].m_SurfaceTexture.Load( rvecSrcMaterial[i].strSurfaceTexture.c_str() );

//			if( !bLoaded )
//			{
//				// texture file was not found - create default white texture 
//				CTextureTool::CreateTexture( &dwTexelColor[tex], 1, 1, &(m_paMaterial[i].m_pSurfaceTexture) );
//			}
		}
	}


	// load triangle sets
	m_paTriangleSet = new CMMA_TriangleSet [ m_iNumMaterials ];
	for( i=0; i<m_iNumMaterials; i++ )
	{
		m_paTriangleSet[i] = rArchive.GetTriangleSet()[i];
	}

	// load bones
	m_iNumBones = rArchive.GetNumBones();
	if( 1 < m_iNumBones )
	{
		m_paTransforms = new Transform [m_iNumBones];
		for( i=0; i<m_iNumBones; i++ )
			m_paTransforms[i].SetIdentity();

		int iMatrixIndex = 0;
		m_pRootBone = new CMM_Bone();
		m_pRootBone->LoadBone_r( rArchive.GetSkeletonRootBone(), m_paTransforms, iMatrixIndex );
	}
	else
		m_iNumBones = 0;

	return true;
}


void CD3DXMeshModel::Render( const unsigned int RenderFlag, CShaderManager& shader_mgr )
{
	if( RenderFlag & RENDER_USE_FIXED_FUNCTION_SHADER )
	{
		RenderNonHLSL(RenderFlag);
		return;
	}

	int i, iNumMaterials = m_iNumMaterials;

	LPDIRECT3DDEVICE9	pd3dDev = DIRECT3D9.GetDevice();
//	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	LPD3DXEFFECT pEffect = shader_mgr.GetEffect();

	if( !pEffect )
		return;

	pd3dDev->SetFVF( m_dwFVF );

	pd3dDev->SetVertexDeclaration( m_pVertexDecleration );

	pd3dDev->SetStreamSource( 0, m_pVB, 0, m_iVertexSize );

	pd3dDev->SetIndices( m_pIB );

	HRESULT hr;

	for( i=0; i<iNumMaterials; i++ )
	{
		CMM_Material& rMaterial = m_paMaterial[i];
		CMMA_TriangleSet& rSubset = m_paTriangleSet[i];

//		pEffect->SetTexture( m_hParameter[PARAM_SURFACE_TEXTURE],   rMaterial.m_SurfaceTexture.GetTexture() );

		if( !rMaterial.m_pSurfaceTexture )
            hr = pEffect->SetTexture( "Texture0", rMaterial.m_SurfaceTexture.GetTexture() );
		else
			hr = pEffect->SetTexture( "Texture0", rMaterial.m_pSurfaceTexture );

		assert( !FAILED(hr) );

//		if( bump map )
//			pEffect->SetTexture( m_hParameter[PARAM_NORMALMAP_TEXTURE], rMaterial.m_NormalMap.GetTexture() );

		if( !rMaterial.m_pNormalMap )
			pEffect->SetTexture( "Texture1", rMaterial.m_NormalMap.GetTexture() );
		else
			pEffect->SetTexture( "Texture1", rMaterial.m_pNormalMap );

//		pEffect->SetValue( m_hParameter[PARAM_SPECULAR], rMaterial.m_fSpecular, sizeof(float) );

		// notify the current shader
		pEffect->CommitChanges();

		pd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			                           0,
									   rSubset.m_iMinIndex,
									   rSubset.m_iNumVertexBlocksToCover,
									   rSubset.m_iStartIndex,
									   rSubset.m_iNumTriangles );
	}
}


void CD3DXMeshModel::RenderNonHLSL( unsigned int RenderFlag )
{
	int i, iNumMaterials = m_iNumMaterials;

	LPDIRECT3DDEVICE9	pd3dDev = DIRECT3D9.GetDevice();

	assert( pd3dDev != NULL );

	// we use only the first texture stage (stage 0)
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// alpha arguments on texture stage 0
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	// alpha-blending settings 
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );


	pd3dDev->SetFVF( m_dwFVF );

	pd3dDev->SetVertexDeclaration( m_pVertexDecleration );

	pd3dDev->SetStreamSource( 0, m_pVB, 0, m_iVertexSize );

	pd3dDev->SetIndices( m_pIB );

	for( i=0; i<iNumMaterials; i++ )
	{
		CMM_Material& rMaterial = m_paMaterial[i];
		CMMA_TriangleSet& rSubset = m_paTriangleSet[i];

		pd3dDev->SetTexture( 0, rMaterial.m_SurfaceTexture.GetTexture() );

		pd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			                           0,
									   rSubset.m_iMinIndex,
									   rSubset.m_iNumVertexBlocksToCover,
									   rSubset.m_iStartIndex,
									   rSubset.m_iNumTriangles );

	}

	pd3dDev->DrawPrimitive(	D3DPT_POINTLIST, 0, m_iNumVertices );
}


bool CD3DXMeshModel::LockVertexBuffer( void*& pVertex )
{
	if( !m_pVB )
		return false;

	m_pVB->Lock( 0, 0, &pVertex, 0 );

	return true;
}


void CD3DXMeshModel::UnlockVertexBuffer()
{
	if( m_pVB )
		m_pVB->Unlock();
}
