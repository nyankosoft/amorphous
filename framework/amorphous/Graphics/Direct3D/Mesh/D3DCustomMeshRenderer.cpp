#include "D3DCustomMeshRenderer.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/Direct3D/D3DTextureResourceVisitor.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{


using namespace std;


CD3DCustomMeshRenderer CD3DCustomMeshRenderer::ms_Instance;


static HRESULT SetD3DFVF( const CustomMesh& mesh )
{
	const U32 vert_flags = mesh.GetVertexFormatFlags();
	DWORD fvf = 0;

	if( vert_flags & VFF::POSITION )      fvf |= D3DFVF_XYZ;
	if( vert_flags & VFF::NORMAL )        fvf |= D3DFVF_NORMAL;
	if( vert_flags & VFF::DIFFUSE_COLOR ) fvf |= D3DFVF_DIFFUSE;
	if( vert_flags &  VFF::TEXCOORD2_0 )  fvf |= D3DFVF_TEX1;
	if( vert_flags & (VFF::TEXCOORD2_0 & VFF::TEXCOORD2_1) )   fvf |= D3DFVF_TEX2;
	if( vert_flags & (VFF::TEXCOORD2_0 & VFF::TEXCOORD2_1 & VFF::TEXCOORD2_2) )   fvf |= D3DFVF_TEX3;
	if( vert_flags & (VFF::BLEND_WEIGHTS|VFF::BLEND_INDICES) )      fvf |= (D3DFVF_XYZB5|D3DFVF_LASTBETA_UBYTE4);
//	if( vert_flags & (VFF::BLEND_WEIGHTS|VFF::BLEND_INDICES) )      fvf |= (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4);

	if( fvf == 0 )
		return E_FAIL;

	return DIRECT3D9.GetDevice()->SetFVF( fvf );
}

/*
///  Render the only subset of the mesh
///  - Replaced by the implementation below this function.
void CD3DCustomMeshRenderer::DrawPrimitivesOfSingleSubsetMesh( const CustomMesh& mesh, bool use_zsorted_indices )
{
	const uint num_verts   = mesh.GetNumVertices();
	const uint num_indices = mesh.GetNumIndices();

	HRESULT hr = DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		num_verts,
		num_indices / 3,
//		pI,
		mesh.GetIndexBufferPtr(),
		D3DFMT_INDEX16,
//		pV,
		mesh.GetVertexBufferPtr(),
		mesh.GetVertexSize()
		);
}
*/


const std::vector<int>& CD3DCustomMeshRenderer::GetAllSubsetIndices( const CustomMesh& mesh )
{
	if( (int)m_SubsetIndices.size() < mesh.GetNumMaterials() )
	{
		int start_index = (int)m_SubsetIndices.size();
		m_SubsetIndices.insert( m_SubsetIndices.end(), mesh.GetNumMaterials() - start_index, 0 );
		for( int i=0; i<mesh.GetNumMaterials() - start_index; i++ )
			m_SubsetIndices[start_index+i] = start_index+i;
	}

	return m_SubsetIndices;
}


void CD3DCustomMeshRenderer::DrawPrimitives( const CustomMesh& mesh, int subset_index, bool use_zsorted_indices )
{
	if( use_zsorted_indices && 1 < mesh.GetTriangleSets().size() )
		return; // z-sorting is not supported for mesh with multiple subsets 

	if( (int)mesh.GetTriangleSets().size() <= subset_index )
		return;

	const CMMA_TriangleSet& ts = mesh.GetTriangleSets()[subset_index];

	const void *pV = (void *)mesh.GetVertexBufferPtr();
//	const void *pI = (void *)mesh.GetIndexBufferPtr();
	const void *pI
		= use_zsorted_indices
		? (void *)mesh.GetZSortedIndexBufferPtr()
		: (void *)( mesh.GetIndexBufferPtr() + (ts.m_iStartIndex * mesh.GetIndexSize()) );

	if( !pV || !pI )
		return;

	HRESULT hr = SetD3DFVF( mesh );

	D3DFORMAT index_format = D3DFMT_INDEX16;
	switch( mesh.GetIndexSize() )
	{
	case 2: index_format = D3DFMT_INDEX16; break;
	case 4: index_format = D3DFMT_INDEX32; break;
	default:
		LOG_PRINT_ERROR( "An unsupported index size: " + to_string(mesh.GetIndexSize()) );
		break;
	}

	hr = DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		ts.m_iMinIndex,
		ts.m_iNumVertexBlocksToCover,
		ts.m_iNumTriangles,
		pI,
		index_format,
		pV,
		mesh.GetVertexSize()
		);
}


void CD3DCustomMeshRenderer::RenderMesh( CustomMesh& mesh, const int *subsets_to_render, int num_subsets_to_render, bool use_zsorted_indices )
{
	if( !subsets_to_render || num_subsets_to_render == 0 )
		return;

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

	hr = pd3dDevice->SetVertexShader( NULL );
	hr = pd3dDevice->SetPixelShader( NULL );

	Result::Name res = Result::SUCCESS;
	for( int i=0; i<num_mats; i++ )
	{
		const int num_textures = mesh.GetNumTextures(i);
		for( int j=0; j<num_textures; j++ )
		{
			res = SetTextureD3D_FFP( (uint)j, mesh.GetTexture(i,j) );
		}

		DrawPrimitives( mesh, i, use_zsorted_indices );
	}

//	DrawPrimitivesOfSingleSubsetMesh( mesh, use_zsorted_indices );
}


void CD3DCustomMeshRenderer::RenderMesh(
	CustomMesh& mesh, ShaderManager& shader_mgr,
	const int *subsets_to_render,
	int num_subsets_to_render,
	bool use_zsorted_indices )
{
	if( !subsets_to_render || num_subsets_to_render == 0 )
		return;

	if( &shader_mgr == &FixedFunctionPipelineManager() )
	{
		// Render the mesh via the fixed function pipeline.
		RenderMesh( mesh );
		return;
	}

	SetD3DFVF( mesh );

	LPD3DXEFFECT pEffect = shader_mgr.GetEffect();
	if( !pEffect )
		return;

	UINT cPasses = 0;
	pEffect->Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pEffect->BeginPass( p );

//		const int num_mats = mesh.GetNumMaterials();
//		for( int i=0; i<num_mats; i++ )
		for( int i=0; i<num_subsets_to_render; i++ )
		{
			int subset_index = subsets_to_render[i];

			const int num_textures = mesh.GetNumTextures(i);
			for( int j=0; j<num_textures; j++ )
			{
				shader_mgr.SetTexture( j, mesh.GetTexture(subset_index,j) );
			}

			pEffect->CommitChanges();

			// Draw the i-th subset of the mesh
			DrawPrimitives( mesh, subset_index, use_zsorted_indices );

			// To see if the triangle set is correct, commet out the line above
			// and uncomment the following two lines. Note that this works only
			// if the mesh has just one subset.
//			DrawPrimitivesOfSingleSubsetMesh( mesh, use_zsorted_indices );
//			break;
		}

		pEffect->EndPass();
	}
	pEffect->End();
}


void CD3DCustomMeshRenderer::RenderSubset( CustomMesh& mesh, int subset_index )
{
	LOG_PRINT_ERROR( " Not implemented." );
}


void CD3DCustomMeshRenderer::RenderSubset( CustomMesh& mesh, ShaderManager& shader_mgr, int subset_index )
{
	vector<int> single_subset;
	single_subset.resize( 1 );
	single_subset[0] = subset_index;
	RenderMesh( mesh, shader_mgr, single_subset, false );
}


} // namespace amorphous
