#include "D3DXMeshObject.hpp"

#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "Graphics/FVF_BumpVertex.h"
#include "Graphics/FVF_BumpWeightVertex.h"
#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/FVF_NormalVertex.h"
#include "Graphics/FVF_WeightVertex.h"
#include "Graphics/FVF_ShadowVertex.h"

#include "Graphics/MeshModel/MeshBone.hpp"
using namespace MeshModel;

#include "Support/Log/DefaultLog.hpp"


CD3DXMeshObject::CD3DXMeshObject()
: m_pMesh(NULL)
{
}


CD3DXMeshObject::CD3DXMeshObject( const std::string& filename )
: m_pMesh(NULL)
{
	bool loaded = LoadFromFile(filename);

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load a mesh file" + filename );
	}
}


CD3DXMeshObject::~CD3DXMeshObject()
{
	Release();
}


void CD3DXMeshObject::Release()
{
	// release materials
	CD3DXMeshObjectBase::Release();

    if( m_pMesh != NULL )
	{
        m_pMesh->Release();
		m_pMesh = NULL;
	}
}


HRESULT CD3DXMeshObject::LoadFromXFile( const std::string& filename )
{
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	HRESULT hr = LoadD3DXMeshAndMaterialsFromXFile( filename, m_pMesh, pAdjacencyBuffer );

	SAFE_RELEASE( pAdjacencyBuffer );

	return hr;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the object
//-----------------------------------------------------------------------------
void CD3DXMeshObject::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXMESH pMesh = m_pMesh;

	//We use only the first texture stage (stage 0)
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// alpha arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	// alpha-blending settings 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Meshes are divided into subsets by materials. Render each subset in a loop
//	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTex = NULL;
    for( int i=0; i<m_NumMaterials; i++ )
    {
        // Set the material and texture for this subset
		pd3dDevice->SetMaterial( &m_pMeshMaterials[i] );

//		if( FAILED(hr) ) MessageBox(NULL, "SetMaterial() Failed", "Error", MB_OK|MB_ICONWARNING);

		if( pTex = GetTexture(i,0).GetTexture() )
		{
			// blend color & alpha of vertex & texture
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		}
		else
		{
			// no texture for this material - use only the vertx color & alpha
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		}

		pd3dDevice->SetTexture( 0, pTex );

        // Draw the mesh subset
        m_pMesh->DrawSubset( i );
    }

}


bool CD3DXMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	// copy the contents from the archive

	m_strFilename = filename;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	LPD3DXMESH pMesh = LoadD3DXMeshFromArchive( archive );

	if( !pMesh )
		return false;

	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

	hr = SetAttributeTable( pMesh, archive.GetTriangleSet() );

	if( FAILED(hr) )
		return false;

	m_pMesh = pMesh;
	pMesh = NULL;

	PeekAttribTables( m_pMesh );

	return true;
}


bool CD3DXMeshObject::CreateMesh( int num_vertices, int num_indices, U32 option_flags,
								  std::vector<D3DVERTEXELEMENT9>& vecVertexElement )
{
	HRESULT hr;
	hr = D3DXCreateMesh(
			num_indices / 3,         // DWORD NumFaces,
			num_vertices,            // DWORD NumVertices,
			option_flags,            // DWORD Options,
			&(vecVertexElement[0]),  // CONST LPD3DVERTEXELEMENT9 * pDeclaration,
			DIRECT3D9.GetDevice(),   // LPDIRECT3DDEVICE9 pD3DDevice,
			&m_pMesh
		);

	if( FAILED(hr) )
		LOG_PRINT_ERROR( fmt_string( " D3DXCreateMesh() failed. (%d faces, %d vertices, flags: %d)", num_indices / 3, num_vertices, option_flags ) );

	return (m_pMesh != NULL);
}
