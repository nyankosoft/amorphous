
#include "GlobalRadarTextureMaker.h"

#include "Stage/StaticGeometryArchiveFG.h"
#include "3DCommon/D3DXMeshObject.h"
#include "Support/memory_helpers.h"
#include "Support/fnop.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace fnop;

using namespace std;


CGlobalRadarTextureMaker::CGlobalRadarTextureMaker()
:
m_IsReady(false)
{
}	


CGlobalRadarTextureMaker::~CGlobalRadarTextureMaker()
{
	SafeDeleteVector( m_vecpMeshObject );
}


bool CGlobalRadarTextureMaker::LoadGeometry( const string& filename )
{
	if( filename.rfind( ".sga" ) != string::npos )
	{
		CBinaryDatabase<string> db;
		bool open = db.Open( filename );
		if( !open )
			return false;

		CStaticGeometryArchiveFG archive;
//		bool loaded = archive.LoadFromFile( filename );

		bool loaded = db.GetData( "Main", archive );

//		vector<C3DMeshModelArchive>& vecMesh = archive.m_vecMeshArchive;
		vector<string>& vecMeshKey = archive.m_vecMeshArchiveKey;

		if( !loaded )
			m_IsReady = false;

		C3DMeshModelArchive mesh_archive;

		size_t i, num = vecMeshKey.size();
		m_vecpMeshObject.resize( num );
		for( i=0; i<num; i++ )
		{
			m_vecpMeshObject[i] = new CD3DXMeshObject();
//			m_vecpMeshObject[i]->LoadMeshFromArchive( vecMesh[i], "" );

			db.GetData( archive.m_vecMeshArchiveKey[i], mesh_archive );
			m_vecpMeshObject[i]->LoadFromArchive( mesh_archive, filename + ":" + vecMeshKey[i] );
		}

		m_IsReady = true;
		return true;
	}
	else if( filename.rfind( ".msh" ) != string::npos )
	{
		m_vecpMeshObject.push_back( new CD3DXMeshObject( filename ) );
	}

	return false;
}


bool CGlobalRadarTextureMaker::InitShader()
{
	string current_work_dir = get_cwd();

//	bool loaded = m_ShaderManager.LoadShaderFromFile( ".\\Shader\\GlobalRadarMap.fx" );
	bool loaded = m_ShaderManager.LoadShaderFromFile( "../../Shader/GlobalRadarMap.fx" );
	if( loaded )
	{
        CShader::Get()->SetShaderManager( &m_ShaderManager );
//		m_ShaderManager.RegisterTechnique( TECH_HEIGHTMAP,				"HeightMap" );
	}
	else
		return false;

	// set the default camera matrix for look-down view along y-axis
	D3DXMATRIX matCamera;
//	D3DXMatrixLookAtLH( &matCamera, &D3DXVECTOR3(0, 100, 0), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 1) );
	D3DXMatrixLookAtLH( &matCamera, &D3DXVECTOR3(0,6000, 0), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, 1) );
	m_ShaderManager.SetViewTransform( matCamera );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_VIEW, &matCamera );

//	this->SetViewWidth( 1.0f );

    return true;
}


void CGlobalRadarTextureMaker::SetViewWidth( float fViewVolumeWidth )
{
	if( fViewVolumeWidth < 0 )
		return;

	m_fViewVolumeWidth = fViewVolumeWidth;

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH( &matProj,	// D3DXMATRIX *pOut
		               m_fViewVolumeWidth,                // w  - Width of the view volume
//					   m_fViewVolumeWidth * 3.0f / 4.0f,  // h  - Height of the view volume
					   m_fViewVolumeWidth,  // h  - Height of the view volume
					   0.1f,		// zn - Minimum z-value of the view volume which is referred to as z-near
					   10000.0f		// zf - Maximum z-value of the view volume which is referred to as z-far
					   );
/*
	D3DXMatrixPerspectiveFovLH( &matProj, 3.14f * 0.5f, 1.0f, 0.1f, 1000.0f );
*/
	m_ShaderManager.SetProjectionTransform( matProj );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
}


void CGlobalRadarTextureMaker::RenderMesh( int mesh_index )
{
	LPD3DXEFFECT pEffect = m_ShaderManager.GetEffect();
	if( !pEffect )
		return;

	// Meshes are divided into subsets by materials. Render each subset in a loop
	CD3DXMeshObject *pMeshObject = m_vecpMeshObject[mesh_index];

	CD3DXMeshObjectBase *pBaseMeshObject = pMeshObject;
//	pBaseMeshObject->Render( pEffect, pEffect->GetParameterByName(NULL,"Texture0") );
	pBaseMeshObject->Render( m_ShaderManager );
	return;

	LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
	if( !pMesh )
		return;

	UINT cPasses;
	pEffect->Begin( &cPasses, 0 );

	DWORD i, dwNumMaterials = pMeshObject->GetNumMaterials();
	for( i=0; i<dwNumMaterials; i++ )
	{
		// Set the material and texture for this subset
//		pd3dDevice->SetMaterial( &pMeshObject->GetMaterial(i) );

		LPDIRECT3DTEXTURE9 pTex = pMeshObject->GetTexture(i,0).GetTexture();
		if( pTex )
			m_ShaderManager.SetTexture( 0, pTex );
//		else
//			m_ShaderManager.SetTexture( 0, NULL/*m_pDefaultTexture*/ );

//		LPDIRECT3DTEXTURE9 pNMTex = NULL;
//		if( pNMTex = pMeshObject->GetNormalMapTexture(i) )
//			m_ShaderManager.SetTexture( 1, pNMTex );

		pEffect->CommitChanges();
		for( UINT p = 0; p < cPasses; ++p )
		{
			pEffect->BeginPass( p );

			// Draw the mesh subset
			pMesh->DrawSubset( i );

			pEffect->EndPass();
		}
	}

	pEffect->End();
}


void CGlobalRadarTextureMaker::Render()
{
	HRESULT hr;

	LPD3DXEFFECT pEffect = m_ShaderManager.GetEffect();
	if( !pEffect )
		return;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_ShaderManager.SetWorldTransform( matWorld );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	hr = pEffect->SetTechnique( "HeightMap" );
//	hr = pEffect->SetTechnique( "NoShader" );

//	if( FAILED(hr) )
//		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

//	CD3DXMeshObjectBase *pMeshObject = m_p3DModel;
//	if( !pMeshObject )
//		return;

//	pd3dDevice->SetVertexDeclaration( pMeshObject->GetVertexDeclaration() );

	pEffect->CommitChanges();

	int i, num_meshes = (int)m_vecpMeshObject.size();
	for( i=0; i<num_meshes; i++ )
	{
		RenderMesh( i );
//		m_vecpMeshObject[i]->Render();
	}

}
