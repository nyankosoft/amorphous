#include "D3DCustomMeshRenderer.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/Direct3D9.hpp"


using namespace std;


CD3DCustomMeshRenderer CD3DCustomMeshRenderer::ms_Instance;


void CD3DCustomMeshRenderer::RenderMesh( CCustomMesh& mesh )
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
		return;

	void *pV = (void *)mesh.GetVertexBufferPtr();
	void *pI = (void *)mesh.GetIndexBufferPtr();
	if( !pV || !pI )
		return;

	const uint num_verts = mesh.GetNumVertices();
	const uint num_indices = mesh.GetNumIndices();

	vector<Vector3> positions;
	mesh.GetPositions( positions );

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

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

	// alpha-blending settings 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	for( int i=0; i<num_mats; i++ )
	{
		const int num_textures = mesh.GetNumTextures(i);
		for( int j=0; j<num_textures; j++ )
		{
			pd3dDevice->SetTexture( j, mesh.GetTexture(i,j).GetTexture() );
		}
	}

	HRESULT hr = S_OK;
	hr = DIRECT3D9.GetDevice()->SetVertexShader( NULL );
	hr = DIRECT3D9.GetDevice()->SetPixelShader( NULL );
	hr = DIRECT3D9.GetDevice()->SetFVF( fvf );
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
