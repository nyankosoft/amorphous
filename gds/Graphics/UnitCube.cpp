#include "UnitCube.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Graphics/Direct3D/FVF_ColorVertex.h"


namespace amorphous
{


CUnitCube::CUnitCube()
{
	m_pVB = NULL;
	m_pIB = NULL;

	m_RenderMode = RS_POLYGON;
}


CUnitCube::~CUnitCube()
{
	Release();
}


void CUnitCube::Release()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );
}


//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------

HRESULT CUnitCube::Init()
{
    COLORVERTEX avVertex[UNITCUBE_NUM_VERTICES];
	memset( avVertex, 0, sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES );

	int iAxis, i;

	D3DXVECTOR3 vConstAxis, vAxis1, vAxis2;
	for( iAxis=0; iAxis<3; iAxis++ )
	{
		vConstAxis = vAxis1 = vAxis2 = D3DXVECTOR3(0,0,0);
		vConstAxis[iAxis] = 1;
		vAxis1[(iAxis + 1) % 3] = 1;
		vAxis2[(iAxis + 2) % 3] = 1;

		avVertex[iAxis*8 + 0].vPosition =  vConstAxis + ( 1) * vAxis1 + (-1) * vAxis2;
		avVertex[iAxis*8 + 1].vPosition =  vConstAxis + ( 1) * vAxis1 + ( 1) * vAxis2;
		avVertex[iAxis*8 + 2].vPosition =  vConstAxis + (-1) * vAxis1 + ( 1) * vAxis2;
		avVertex[iAxis*8 + 3].vPosition =  vConstAxis + (-1) * vAxis1 + (-1) * vAxis2;

		avVertex[iAxis*8 + 4].vPosition = -vConstAxis + ( 1) * vAxis1 + ( 1) * vAxis2;
		avVertex[iAxis*8 + 5].vPosition = -vConstAxis + ( 1) * vAxis1 + (-1) * vAxis2;
		avVertex[iAxis*8 + 6].vPosition = -vConstAxis + (-1) * vAxis1 + (-1) * vAxis2;
		avVertex[iAxis*8 + 7].vPosition = -vConstAxis + (-1) * vAxis1 + ( 1) * vAxis2;

		DWORD dwColor = D3DCOLOR_ARGB( 255, ((DWORD)vConstAxis[0] * 255), ((DWORD)vConstAxis[1] * 255), ((DWORD)vConstAxis[2] * 255) );
		for(i=0; i<4; i++)
		{
			avVertex[iAxis*8 + i    ].vNormal = vConstAxis * 10.0f;
			avVertex[iAxis*8 + i + 4].vNormal = vConstAxis * (-1);
			avVertex[iAxis*8 + i].color = dwColor;
			avVertex[iAxis*8 + i + 4].color = dwColor;
//			avVertex[iAxis*8 + i    ].color = 0xFF000000 | ( 0x000000FF << ( (2-iAxis) * 8 ) );
//			avVertex[iAxis*8 + i + 4].color = 0xFF000000 | ( 0x000000FF << ( (2-iAxis) * 8 ) );
		}
	}

	// change into unit size
	for(i=0; i<UNITCUBE_NUM_VERTICES; i++)
		avVertex[i].vPosition *= 0.5f;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr;

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( pd3dDevice->CreateVertexBuffer( sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES,
                                                0, COLORVERTEX::FVF,
                                                D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID* pVertex;
//    if( FAILED( m_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
    if( FAILED( m_pVB->Lock( 0, 0 /*sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES*/, (void**)&pVertex, 0 ) ) )
        return E_FAIL;

//    memcpy( pVertices, vertices, sizeof(vertices) );
    memcpy( pVertex, avVertex, sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES );

    m_pVB->Unlock();


	// create buffer to hold indices for wireframes
	hr = pd3dDevice->CreateIndexBuffer( sizeof(WORD) * 24,	// UINT Length
		                                0,					// DWORD Usage,
				                        D3DFMT_INDEX16,		// D3DFORMAT Format,
					                    D3DPOOL_DEFAULT,	// D3DPOOL Pool,
					                    &m_pIB,				// IDirect3DIndexBuffer9** ppIndexBuffer,
					                    NULL );				// HANDLE* pSharedHandle
	if( FAILED(hr) )
		return hr;

	VOID *pIndex;
	hr = m_pIB->Lock( 0, 0, (void **)&pIndex, 0 );

	WORD awIndex[24] = { 0,1, 1,2, 2,3, 3,0,  4,5, 5,6, 6,7, 7,4,  0,5, 1,4, 2,7, 3,6 };

	memcpy( pIndex, awIndex, sizeof(WORD) * 24 );

	m_pIB->Unlock();

    return S_OK;
}


void CUnitCube::Draw()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	pd3dDevice->SetTexture( 0, NULL );

	//We use only the first texture stage (stage 0)
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// alpha arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
//	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	// alpha-blending settings 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    // Draw the triangles in the vertex buffer. This is broken into a few
    // steps. We are passing the vertices down a "stream", so first we need
    // to specify the source of that stream, which is our vertex buffer. Then
    // we need to let D3D know what vertex shader to use. Full, custom vertex
    // shaders are an advanced topic, but in most cases the vertex shader is
    // just the FVF, so that D3D knows what type of vertices we are dealing
    // with. Finally, we call DrawPrimitive() which does the actual rendering
    // of our geometry (in this case, just one triangle).
    pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(COLORVERTEX) );
    pd3dDevice->SetFVF( COLORVERTEX::FVF );

//	pd3dDevice->SetVertexShader(NULL);

	if( m_RenderMode == CUnitCube::RS_POLYGON )
	{
		int iFace;
		for( iFace=0; iFace<6; iFace++ )
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, iFace*4, 2 );
	}
	else if( m_RenderMode == CUnitCube::RS_WIREFRAME )
	{
		pd3dDevice->SetIndices( m_pIB );
		pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, 8, 0, 12 );
	}

}


bool CUnitCube::SetDefault3Colors()
{
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0 /*sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES*/, (void**)&pVertices, 0 ) ) )
        return false;

	COLORVERTEX *pavVertex = (COLORVERTEX *)pVertices;

	int i, iAxis;
	DWORD adwColor[3];
	adwColor[0] = 0xFFFF0000;
	adwColor[1] = 0xFF00FF00;
	adwColor[2] = 0xFF0000FF;
	for(iAxis=0; iAxis<3; iAxis++)
	{
		for( i=0; i<4; i++ )
		{
			pavVertex[iAxis*8 + i].color = adwColor[iAxis];
			pavVertex[iAxis*8 + i + 4].color = adwColor[iAxis];
		}
	}

    m_pVB->Unlock();

	return true;
}


bool CUnitCube::SetUniformColor( float r, float g, float b, float a )
{
	DWORD dwColor = D3DCOLOR_ARGB( (int)(a * 255.0f), (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f) );

    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0 /*sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES*/, (void**)&pVertices, 0 ) ) )
        return false;

	COLORVERTEX *pavVertex = (COLORVERTEX *)pVertices;

	int i;
	for( i=0; i<UNITCUBE_NUM_VERTICES; i++ )
	{
		pavVertex[i].color = dwColor;
	}

    m_pVB->Unlock();

	return true;
}


/*
// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
*/

} // amorphous
