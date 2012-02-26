#include "D3DCustomMeshRenderer.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"


using namespace std;


CD3DCustomMeshRenderer CD3DCustomMeshRenderer::ms_Instance;


static HRESULT SetD3DFVF( const CCustomMesh& mesh )
{
	const U32 vert_flags = mesh.GetVertexFormatFlags();
	DWORD fvf = 0;

	if( vert_flags & VFF::POSITION )      fvf |= D3DFVF_XYZ;
	if( vert_flags & VFF::NORMAL )        fvf |= D3DFVF_NORMAL;
	if( vert_flags & VFF::DIFFUSE_COLOR ) fvf |= D3DFVF_DIFFUSE;
	if( vert_flags &  VFF::TEXCOORD2_0 )  fvf |= D3DFVF_TEX1;
	if( vert_flags & (VFF::TEXCOORD2_0 & VFF::TEXCOORD2_1) )   fvf |= D3DFVF_TEX2;
	if( vert_flags & (VFF::TEXCOORD2_0 & VFF::TEXCOORD2_1 & VFF::TEXCOORD2_2) )   fvf |= D3DFVF_TEX3;

	if( fvf == 0 )
		return E_FAIL;

	return DIRECT3D9.GetDevice()->SetFVF( fvf );
}


void CD3DCustomMeshRenderer::DrawPrimitives( const CCustomMesh& mesh )
{

	const uint num_verts   = mesh.GetNumVertices();
	const uint num_indices = mesh.GetNumIndices();

	const void *pV = (void *)mesh.GetVertexBufferPtr();
	const void *pI = (void *)mesh.GetIndexBufferPtr();
	if( !pV || !pI )
		return;

	HRESULT hr = SetD3DFVF( mesh );

	hr = DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		num_verts,
		num_indices / 3,
		pI,
		D3DFMT_INDEX16,
		pV,
		mesh.GetVertexSize()
		);
}


void CD3DCustomMeshRenderer::RenderMesh( CCustomMesh& mesh )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;

	hr = pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	hr = pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	const int num_mats = mesh.GetNumMaterials();
	if( 0 < num_mats && 0 < mesh.GetMaterial(0).Texture.size() )
	{
		// color arguments on texture stage 0
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		// alpha arguments on texture stage 0
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	}
	else
	{
		// color arguments on texture stage 0
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

		// alpha arguments on texture stage 0
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
//		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	}

	// default alpha-blending settings
	hr = pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	hr = pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
	hr = pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// alpha-blending settings 
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
/**/
//	static LPDIRECT3DTEXTURE9 s_pTex = NULL;
//	if( !s_pTex )
//		D3DXCreateTextureFromFile( pd3dDevice, "textures/SmashedGrayMarble.jpg", &s_pTex );
	for( int i=0; i<num_mats; i++ )
	{
		const int num_textures = mesh.GetNumTextures(i);
		for( int j=0; j<num_textures; j++ )
		{
			LPDIRECT3DTEXTURE9 pTex = mesh.GetTexture(i,j).GetTexture();
			hr = pd3dDevice->SetTexture( j, pTex );
		}
	}

	hr = pd3dDevice->SetVertexShader( NULL );
	hr = pd3dDevice->SetPixelShader( NULL );

	DrawPrimitives( mesh );
}


void CD3DCustomMeshRenderer::RenderMesh( CCustomMesh& mesh, CShaderManager& shader_mgr )
{
	if( &shader_mgr == &FixedFunctionPipelineManager() )
	{
		// Render the mesh via the fixed function pipeline.
		RenderMesh( mesh );
		return;
	}

	const int num_mats = mesh.GetNumMaterials();
	for( int i=0; i<num_mats; i++ )
	{
		const int num_textures = mesh.GetNumTextures(i);
		for( int j=0; j<num_textures; j++ )
		{
			shader_mgr.SetTexture( j, mesh.GetTexture(i,j) );
		}
	}

	SetD3DFVF( mesh );

	LPD3DXEFFECT pEffect = shader_mgr.GetEffect();
	if( !pEffect )
		return;

	pEffect->CommitChanges();

	UINT cPasses = 0;
	pEffect->Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pEffect->BeginPass( p );

		// Draw the mesh subset
		DrawPrimitives( mesh );

		pEffect->EndPass();
	}
	pEffect->End();
}
