
#include "3DCommon/Direct3D9.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/TextureTool.h"
#include "3DCommon/TextureRenderTarget.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/D3DXSMeshObject.h"
#include "3DCommon/D3DXMeshObject.h"
#include "LightWave/LightWaveObject.h"
#include "LightWave/3DMeshModelBuilder_LW.h"

#include "Support/fnop.h"
#include "Support/BMPImageExporter.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"


#include "BumpmapTextureMaker2_LWO2.h"

using namespace boost;


static int s_RenderVolumeTexture = 0;//1;


bool CBumpmapTextureMaker2_LWO2::LoadShader( const string& shader_filename )
{
	// load shader for rendering normal map, flat texture surface and bumpmap surface
	SafeDelete( m_pShaderManager );
	m_pShaderManager = new CShaderManager;

	if( !m_pShaderManager->LoadShaderFromFile( shader_filename ) )
	{
		SafeDelete( m_pShaderManager );
		return false;
	}

	CShader::Get()->SetShaderManager( m_pShaderManager );

	m_pShaderManager->RegisterTechnique( BTM_RENDERMODE_NORMALMAP,				"NormalMap" );
	m_pShaderManager->RegisterTechnique( BTM_RENDERMODE_FLAT_TEXTURED_SURFACE,	"FlatTexSurface" );
	m_pShaderManager->RegisterTechnique( BTM_RENDERMODE_BUMPY_TEXTURED_SURFACE,	"BumpyTexturedSurface" );

	if( s_RenderVolumeTexture  == 1 )
	{
		// overwrite the second technique
		m_pShaderManager->RegisterTechnique( BTM_RENDERMODE_FLAT_TEXTURED_SURFACE,	"AlphaSurface" );
	}


	// set the default camera matrix which faces along z-axis
	D3DXMATRIX matCamera;
	D3DXMatrixLookAtLH( &matCamera, &D3DXVECTOR3(0, 0, 5), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0) );

	m_pShaderManager->SetViewTransform( matCamera );

	return true;
}


CBumpmapTextureMaker2_LWO2::CBumpmapTextureMaker2_LWO2()
:
m_p3DModel(NULL),
m_pShaderManager(NULL)
{

//	m_fViewVolumeWidth = 1.0f;

	m_TechniqueID = 0;


	memset( (void *)&m_DirLight, 0, sizeof(CDirectionalLight) );

	this->SetViewWidth( 1.0f );

	// create 1x1 white texture as a default texture
	m_pDefaultTexture = NULL;
	DWORD dwTexel = 0xFFFFFFFF;
	CTextureTool::CreateTexture( &dwTexel, 1, 1, &m_pDefaultTexture );

}


CBumpmapTextureMaker2_LWO2::~CBumpmapTextureMaker2_LWO2()
{
	Release();
}


void CBumpmapTextureMaker2_LWO2::Release()
{
	SafeDelete( m_p3DModel );
	SafeDelete( m_pShaderManager );
}


bool CBumpmapTextureMaker2_LWO2::LoadModel( const char *pFilename )
{
	shared_ptr<CLWO2_Object> pObj;

	pObj = shared_ptr<CLWO2_Object>( new CLWO2_Object() );
	if( !pObj->LoadLWO2Object(pFilename) )
	{
		return false;
	}

	// find layer with the name "LYR_BumpSource"
	list<CLWO2_Layer>::iterator itrLayer;
	list<CLWO2_Layer>& rlstLayer = pObj->GetLayer();

	SLayerSet layer_set;

	int pos;
	string mesh_tag = "LYR_BumpSource";
	string strMeshFilename;

	for(itrLayer = rlstLayer.begin();
		itrLayer != rlstLayer.end();
		itrLayer++)
	{
		pos = itrLayer->GetName().find( mesh_tag );

		if( pos != string::npos )
		{
			// the layer name is "LYR_BumpSource" - register this as a mesh layer
			strMeshFilename = pFilename;
			fnop::change_ext( strMeshFilename, "msh" );

			layer_set.strOutputFilename = strMeshFilename;
//			layer_set.vecitrMeshLayer.push_back( itrLayer );
			layer_set.vecpMeshLayer.push_back( &(*itrLayer) );
			break;
		}
	}

	if( itrLayer == rlstLayer.end() )
	{
		LOG_PRINT_ERROR( " - No layer with the name '" + mesh_tag + "'" );
		return false;
	}

	// create mesh model
	shared_ptr<C3DMeshModelBuilder_LW> pMeshLoader
		= shared_ptr<C3DMeshModelBuilder_LW>( new C3DMeshModelBuilder_LW(pObj) );

	pMeshLoader->SetTexturePathnameOption( TexturePathnameOption::ORIGINAL_FILENAME );
	if( !pMeshLoader->BuildMeshModel( layer_set ) )
	{
		LOG_PRINT_ERROR( "Failed to build a mesh object" );
		return false;
	}

	C3DMeshModelBuilder general_mesh_to_mesh_archive_converter;
	general_mesh_to_mesh_archive_converter.BuildMeshModel( pMeshLoader );

	general_mesh_to_mesh_archive_converter.GetArchive().SaveToFile( layer_set.strOutputFilename );

	// the mesh has been created.
	// destroy the source LW object and the mesh builder
	pMeshLoader.reset();

	// load a mesh model
//	m_p3DModel = new CD3DXSMeshObject;
	m_p3DModel = new CD3DXMeshObject;

	if( !m_p3DModel->LoadFromFile(strMeshFilename) )
	{
		LOG_PRINT_ERROR( "Cannot load a mesh object: " + strMeshFilename );
		SafeDelete( m_p3DModel );
		return false;
	}

	m_strBaseFilename = pFilename;

	return true;
}


/// load a mesh object file
bool CBumpmapTextureMaker2_LWO2::LoadMeshObject( const char *pFilename )
{
	return false;
}


void CBumpmapTextureMaker2_LWO2::SetViewWidth( float fViewVolumeWidth )
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
					   100.0f		// zf - Maximum z-value of the view volume which is referred to as z-far
					   );

	if( m_pShaderManager )
		m_pShaderManager->SetProjectionTransform( matProj );

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );
}


void CBumpmapTextureMaker2_LWO2::UpdateDirectionalLight()
{
	Matrix33 matRotX, matRotY;

	matRotX.SetRotationX( m_DirLight.m_AngleX );
	matRotY.SetRotationY( m_DirLight.m_AngleY );

	m_DirLight.m_vDir = matRotY * matRotX * Vector3(0,0,1);

	LPD3DXEFFECT pEffect = m_pShaderManager->GetEffect();

	pEffect->SetValue( "g_vLightDir", (void *)&m_DirLight.m_vDir, sizeof(float) * 3 );
}


void CBumpmapTextureMaker2_LWO2::RenderBackground()
{
	C2DRect background_rect;

	float w = 512;

	background_rect.SetPosition( D3DXVECTOR2(0,0), D3DXVECTOR2(w,w) );

	DWORD dwColor;
	if( m_TechniqueID == BTM_RENDERMODE_NORMALMAP )
		dwColor = 0xFF808080;
	else
		dwColor = 0xFF000000;

	background_rect.SetColor( dwColor );

	background_rect.Draw(); 
}


void CBumpmapTextureMaker2_LWO2::Render()
{
	if( m_TechniqueID == BTM_RENDERMODE_PREVIEW )
		RenderPreview();
	else
		RenderTexture();
}


void CBumpmapTextureMaker2_LWO2::RenderPreview()
{
	const int WindowSize = 512;

	C2DRect rect( Vector2(0,0), Vector2(WindowSize,WindowSize), 0xFFFFFFFF );

	rect.SetTextureUV( TEXCOORD2( 0.0f, 0.0f ), TEXCOORD2( 3.0f, 3.0f ) );

	rect.Draw( m_PreviewTexture );
}


void CBumpmapTextureMaker2_LWO2::RenderTexture()
{
	if( !m_p3DModel )
		return;

	HRESULT hr;

	LPD3DXEFFECT pEffect;
	if( !m_pShaderManager || !( pEffect = m_pShaderManager->GetEffect() ) )
		return;

	// udpate the directional light for the shader
	UpdateDirectionalLight();

	CShaderManager *pShaderManager = m_pShaderManager;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pShaderManager->SetWorldTransform( matWorld );

	pEffect->CommitChanges();

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	hr = pShaderManager->SetTechnique( m_TechniqueID );

//	if( FAILED(hr) )
//		return;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	if( s_RenderVolumeTexture && m_TechniqueID != BTM_RENDERMODE_NORMALMAP )
	{
		// set device for layered transparency
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	}
	else
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}

//	RenderBackground();

//	hr = pEffect->SetValue( "g_vEyePos", &(g_CameraController.GetPosition()), sizeof(float) * 3 );

//	if( FAILED(hr) )
//		return;

	UINT cPasses;
	pEffect->Begin( &cPasses, 0 );

	CD3DXMeshObjectBase *pMeshObject = m_p3DModel;

	if( !pMeshObject )
		return;


	pMeshObject->Render( *pShaderManager );


/*
//	LPD3DXPMESH pPMesh = pMeshObject->GetPMesh();
	LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
	if( !pMesh )
		return;

//	pd3dDevice->SetVertexDeclaration( pMeshObject->GetVertexDeclaration() );

	LPDIRECT3DTEXTURE9 pTex;





	// Meshes are divided into subsets by materials. Render each subset in a loop
	DWORD dwNumMaterials = pMeshObject->GetNumMaterials();
	for( DWORD i=0; i<dwNumMaterials; i++ )
	{
		// Set the material and texture for this subset
//		pd3dDevice->SetMaterial( &pMeshObject->GetMaterial(i) );

		pTex = pMeshObject->GetTexture(i);
		if( pTex )
            pShaderManager->SetTexture( 0, pTex );
		else
            pShaderManager->SetTexture( 0, m_pDefaultTexture );

		LPDIRECT3DTEXTURE9 pNMTex = NULL;
		if( pNMTex = pMeshObject->GetNormalMapTexture(i) )
			pShaderManager->SetTexture( 1, pNMTex );

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
*/
}



void CBumpmapTextureMaker2_LWO2::SetRenderMode( unsigned int render_mode )
{
	int prev_mode = m_TechniqueID;

	m_TechniqueID = render_mode;

	if( prev_mode != BTM_RENDERMODE_PREVIEW
	 && render_mode == BTM_RENDERMODE_PREVIEW )
	{
		m_PreviewTexture.Load();
	}
}


void CBumpmapTextureMaker2_LWO2::SaveImages( int width, int height )
{
	CTextureRenderTarget tex_render_target;

	tex_render_target.Init( width, height );

	tex_render_target.SetBackgroundColor( 0xFF000000 );

	int i;
	string strTexFilename[3];
	strTexFilename[0] = m_strBaseFilename;
	fnop::change_ext( strTexFilename[0], "bmp" );
	strTexFilename[1] = strTexFilename[0];
	fnop::append_to_body( strTexFilename[1], "_NM" );
	strTexFilename[2] = strTexFilename[0];
	fnop::append_to_body( strTexFilename[2], "_FB" );

	// save current technique, or the rendering mode
	int orig_tech_id = m_TechniqueID;

	int technique[3] =
	{ BTM_RENDERMODE_FLAT_TEXTURED_SURFACE,
	  BTM_RENDERMODE_NORMALMAP,
	  BTM_RENDERMODE_BUMPY_TEXTURED_SURFACE };

	CBMPImageExporter bmp_exporter;
	DWORD *pdwTexelData;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	for( i=0; i<3; i++ )
	{
		// release and load textures for the render target
		tex_render_target.ReleaseTextures();
		tex_render_target.LoadTextures();

		tex_render_target.SetRenderTarget();

		m_TechniqueID = technique[i];

//	    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
		pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128,128,128), 1.0f, 0 );

		// Begin the scene
		pd3dDevice->BeginScene();

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		Render();

		// End the scene
		pd3dDevice->EndScene();
		// Present the backbuffer contents to the display
		pd3dDevice->Present( NULL, NULL, NULL, NULL );

		tex_render_target.ResetRenderTarget();

		tex_render_target.CopyRenderTarget();

		LPDIRECT3DTEXTURE9 pTex = tex_render_target.GetRenderTargetCopyTexture();

		// gain the access to the texel data of the texture surface
		D3DLOCKED_RECT locked_rect;
		pTex->LockRect( 0, &locked_rect, NULL, 0 );
		pdwTexelData = (DWORD *)locked_rect.pBits;

		bmp_exporter.OutputImage_24Bit( strTexFilename[i].c_str(), width, height, pdwTexelData );
	}

	m_TechniqueID = orig_tech_id;

	// set the fake-bump texture filename for preview
	m_PreviewTexture.filename = strTexFilename[2];
}
