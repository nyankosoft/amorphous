#include "UnitSphere.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/FVF_ColorVertex.h"


CUnitSphere::CUnitSphere()
{
	m_pVB = NULL;
	m_pIB = NULL;

//	m_RenderMode = RS_POLYGON;
}


CUnitSphere::~CUnitSphere()
{
	Release();
}


void CUnitSphere::Release()
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

HRESULT CUnitSphere::Init()
{
    COLORVERTEX avVertex[NUM_VERTICES];
	memset( avVertex, 0, sizeof(COLORVERTEX) * NUM_VERTICES );
	
	SetVertices( avVertex );

	// set default color
	for( int i=0; i<NUM_VERTICES; i++ )
	{
		avVertex[i].color = 0xFFCCCCCC;
	}

//	for(i=0; i<NUM_VERTICES; i++){}

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr;

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( pd3dDevice->CreateVertexBuffer( sizeof(COLORVERTEX) * NUM_VERTICES,
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
    memcpy( pVertex, avVertex, sizeof(COLORVERTEX) * NUM_VERTICES );

    m_pVB->Unlock();

	// create buffer to hold indices for wireframes
	hr = pd3dDevice->CreateIndexBuffer( sizeof(WORD) * NUM_INDICES,	// UINT Length
		                                0,					// DWORD Usage,
				                        D3DFMT_INDEX16,		// D3DFORMAT Format,
					                    D3DPOOL_DEFAULT,	// D3DPOOL Pool,
					                    &m_pIB,				// IDirect3DIndexBuffer9** ppIndexBuffer,
					                    NULL );				// HANDLE* pSharedHandle
	if( FAILED(hr) )
		return hr;

	VOID *pIndex;
	hr = m_pIB->Lock( 0, 0, (void **)&pIndex, 0 );

	WORD awIndex[NUM_INDICES] =
	{
		 1,  0,  2,   3,  1,  4,   5,  4,  2,   4,  1,  2,   6,  3,  7,   7,  4,  8,   9,  8,  5,   7,  3,  4,   8,  4,  5,
		10,  0,  1,  11, 10, 12,   3, 12,  1,  12, 10,  1,  13, 11, 14,  14, 12, 15,   6, 15,  3,  14, 11, 12,  15, 12,  3,
		16,  0, 10,  17, 16, 18,  11, 18, 10,  18, 16, 10,  19, 17, 20,  20, 18, 21,  13, 21, 11,  20, 17, 18,  21, 18, 11,
		16, 22,  0,  23, 22, 24,  17, 24, 16,  24, 22, 16,  25, 23, 26,  27, 26, 24,  19, 27, 17,  26, 23, 24,  27, 24, 17,
		22,  2,  0,   5,  2, 28,  23, 28, 22,  28,  2, 22,   9,  5, 29,  30, 29, 28,  25, 30, 23,  29,  5, 28,  30, 28, 23,
		31, 33, 32,  32, 35, 34,  33, 36, 35,  33, 35, 32,  34, 38, 37,  35, 39, 38,  36, 40, 39,  35, 38, 34,  35, 36, 39,
		31, 41, 33,  33, 42, 36,  41, 43, 42,  41, 42, 33,  36, 44, 40,  42, 45, 44,  43, 46, 45,  42, 44, 36,  42, 43, 45,
		31, 47, 41,  41, 48, 43,  47, 49, 48,  41, 47, 48,  43, 50, 46,  48, 51, 50,  49, 52, 51,  48, 50, 43,  48, 49, 51,
		31, 53, 47,  47, 54, 49,  53, 55, 54,  47, 53, 54,  49, 56, 52,  54, 57, 56,  55, 58, 57,  54, 56, 49,  54, 55, 57,
		31, 32, 53,  53, 59, 55,  32, 34, 59,  32, 59, 53,  55, 60, 58,  59, 61, 60,  34, 37, 61,  59, 60, 55,  59, 34, 61,
		62,  8,  9,  63,  7,  8,  64, 63, 62,  62, 63,  8,  65,  6,  7,  66, 65, 63,  37, 66, 64,  65,  7, 63,  66, 63, 64,
		67, 15,  6,  68, 14, 15,  69, 68, 67,  67, 68, 15,  70, 13, 14,  71, 70, 68,  40, 71, 69,  70, 14, 68,  71, 68, 69,
		72, 21, 13,  73, 20, 21,  74, 73, 72,  72, 73, 21,  75, 19, 20,  76, 75, 73,  46, 76, 74,  75, 20, 73,  74, 76, 73,
		77, 27, 19,  78, 26, 27,  79, 78, 77,  77, 78, 27,  80, 25, 26,  81, 80, 78,  52, 81, 79,  80, 26, 78,  79, 81, 78,
		82, 30, 25,  83, 29, 30,  84, 83, 82,  82, 83, 30,  85,  9, 29,  86, 85, 83,  58, 86, 84,  85, 29, 83,  84, 86, 83,
		39, 40, 69,  39, 87, 38,  87, 69, 67,  39, 69, 87,  38, 66, 37,  87, 65, 66,  67,  6, 65,  38, 87, 66,  87, 67, 65,
		45, 46, 74,  45, 88, 44,  88, 74, 72,  45, 74, 88,  44, 71, 40,  88, 70, 71,  72, 13, 70,  44, 88, 71,  88, 72, 70,
		51, 52, 79,  50, 51, 89,  89, 79, 77,  51, 79, 89,  50, 76, 46,  89, 75, 76,  75, 77, 19,  50, 89, 76,  89, 77, 75,
		57, 58, 84,  56, 57, 90,  90, 84, 82,  57, 84, 90,  56, 81, 52,  90, 80, 81,  80, 82, 25,  56, 90, 81,  90, 82, 80,
		61, 37, 64,  60, 61, 91,  91, 64, 62,  61, 64, 91,  60, 86, 58,  91, 85, 86,  85, 62,  9,  60, 91, 86,  91, 62, 85
	};

	memcpy( pIndex, awIndex, sizeof(WORD) * NUM_INDICES );

	m_pIB->Unlock();

    return S_OK;
}


void CUnitSphere::Draw()
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
	pd3dDevice->SetIndices( m_pIB );
    pd3dDevice->SetFVF( COLORVERTEX::FVF );
//	pd3dDevice->SetVertexShader(NULL);

//	if( m_RenderMode == CUnitSphere::RS_POLYGON )
//	{
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, NUM_VERTICES, 0, NUM_TRIANGLES );
//	}
/*	else if( m_RenderMode == CUnitSphere::RS_WIREFRAME )
	{
		pd3dDevice->SetIndices( m_pIB );
		pd3dDevice->DrawIndexedPrimitive( D3DPT_LINELIST, 0, 0, 8, 0, 12 );
	}
*/
}


bool CUnitSphere::SetColor( float r, float g, float b, float a )
{
	DWORD dwColor = D3DCOLOR_ARGB( (int)(a * 255.0f), (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f) );

    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, 0 /*sizeof(COLORVERTEX) * UNITCUBE_NUM_VERTICES*/, (void**)&pVertices, 0 ) ) )
        return false;

	COLORVERTEX *pavVertex = (COLORVERTEX *)pVertices;

	int i;
	for( i=0; i<NUM_VERTICES; i++ )
	{
		pavVertex[i].color = dwColor;
	}

    m_pVB->Unlock();

	return true;
}


void CUnitSphere::SetVertices( COLORVERTEX *pv )
{
	pv[0].vPosition = D3DXVECTOR3(0.000000f,1.000000f,0.000000f); pv[0].vNormal = D3DXVECTOR3(0.000000f,1.000000f,0.000000f);
	pv[1].vPosition = D3DXVECTOR3(0.343074f,0.932670f,0.111472f); pv[1].vNormal = D3DXVECTOR3(0.334582f,0.936075f,0.108712f);
	pv[2].vPosition = D3DXVECTOR3(0.000000f,0.932670f,0.360730f); pv[2].vNormal = D3DXVECTOR3(0.000000f,0.936075f,0.351800f);
	pv[3].vPosition = D3DXVECTOR3(0.639949f,0.739749f,0.207932f); pv[3].vNormal = D3DXVECTOR3(0.646643f,0.733285f,0.210107f);
	pv[4].vPosition = D3DXVECTOR3(0.356822f,0.794654f,0.491123f); pv[4].vNormal = D3DXVECTOR3(0.356822f,0.794654f,0.491123f);
	pv[5].vPosition = D3DXVECTOR3(0.000000f,0.739749f,0.672883f); pv[5].vNormal = D3DXVECTOR3(-0.000000f,0.733285f,0.679921f);
	pv[6].vPosition = D3DXVECTOR3(0.850651f,0.447214f,0.276393f); pv[6].vNormal = D3DXVECTOR3(0.850651f,0.447214f,0.276393f);
	pv[7].vPosition = D3DXVECTOR3(0.639949f,0.516806f,0.568662f); pv[7].vNormal = D3DXVECTOR3(0.646643f,0.515861f,0.561908f);
	pv[8].vPosition = D3DXVECTOR3(0.343074f,0.516806f,0.784354f); pv[8].vNormal = D3DXVECTOR3(0.334582f,0.515861f,0.788633f);
	pv[9].vPosition = D3DXVECTOR3(0.000000f,0.447214f,0.894427f); pv[9].vNormal = D3DXVECTOR3(0.000000f,0.447214f,0.894427f);
	pv[10].vPosition = D3DXVECTOR3(0.212032f,0.932670f,-0.291836f); pv[10].vNormal = D3DXVECTOR3(0.206783f,0.936075f,-0.284612f);
	pv[11].vPosition = D3DXVECTOR3(0.395510f,0.739749f,-0.544373f); pv[11].vNormal = D3DXVECTOR3(0.399648f,0.733285f,-0.550068f);
	pv[12].vPosition = D3DXVECTOR3(0.577350f,0.794654f,-0.187592f); pv[12].vNormal = D3DXVECTOR3(0.577350f,0.794654f,-0.187592f);
	pv[13].vPosition = D3DXVECTOR3(0.525731f,0.447214f,-0.723607f); pv[13].vNormal = D3DXVECTOR3(0.525731f,0.447214f,-0.723607f);
	pv[14].vPosition = D3DXVECTOR3(0.738585f,0.516806f,-0.432902f); pv[14].vNormal = D3DXVECTOR3(0.734230f,0.515861f,-0.441355f);
	pv[15].vPosition = D3DXVECTOR3(0.851981f,0.516806f,-0.083904f); pv[15].vNormal = D3DXVECTOR3(0.853426f,0.515861f,-0.074505f);
	pv[16].vPosition = D3DXVECTOR3(-0.212032f,0.932670f,-0.291836f); pv[16].vNormal = D3DXVECTOR3(-0.206783f,0.936075f,-0.284612f);
	pv[17].vPosition = D3DXVECTOR3(-0.395510f,0.739749f,-0.544373f); pv[17].vNormal = D3DXVECTOR3(-0.399648f,0.733285f,-0.550068f);
	pv[18].vPosition = D3DXVECTOR3(0.000000f,0.794654f,-0.607062f); pv[18].vNormal = D3DXVECTOR3(0.000000f,0.794654f,-0.607062f);
	pv[19].vPosition = D3DXVECTOR3(-0.525731f,0.447214f,-0.723607f); pv[19].vNormal = D3DXVECTOR3(-0.525731f,0.447214f,-0.723607f);
	pv[20].vPosition = D3DXVECTOR3(-0.183479f,0.516806f,-0.836210f); pv[20].vNormal = D3DXVECTOR3(-0.192865f,0.515861f,-0.834680f);
	pv[21].vPosition = D3DXVECTOR3(0.183479f,0.516806f,-0.836210f); pv[21].vNormal = D3DXVECTOR3(0.192865f,0.515861f,-0.834680f);
	pv[22].vPosition = D3DXVECTOR3(-0.343074f,0.932670f,0.111472f); pv[22].vNormal = D3DXVECTOR3(-0.334582f,0.936075f,0.108712f);
	pv[23].vPosition = D3DXVECTOR3(-0.639949f,0.739749f,0.207932f); pv[23].vNormal = D3DXVECTOR3(-0.646643f,0.733285f,0.210107f);
	pv[24].vPosition = D3DXVECTOR3(-0.577350f,0.794654f,-0.187592f); pv[24].vNormal = D3DXVECTOR3(-0.577350f,0.794654f,-0.187592f);
	pv[25].vPosition = D3DXVECTOR3(-0.850651f,0.447214f,0.276393f); pv[25].vNormal = D3DXVECTOR3(-0.850651f,0.447214f,0.276393f);
	pv[26].vPosition = D3DXVECTOR3(-0.851981f,0.516806f,-0.083904f); pv[26].vNormal = D3DXVECTOR3(-0.853426f,0.515861f,-0.074505f);
	pv[27].vPosition = D3DXVECTOR3(-0.738585f,0.516806f,-0.432902f); pv[27].vNormal = D3DXVECTOR3(-0.734230f,0.515861f,-0.441355f);
	pv[28].vPosition = D3DXVECTOR3(-0.356822f,0.794654f,0.491123f); pv[28].vNormal = D3DXVECTOR3(-0.356822f,0.794654f,0.491123f);
	pv[29].vPosition = D3DXVECTOR3(-0.343074f,0.516806f,0.784354f); pv[29].vNormal = D3DXVECTOR3(-0.334582f,0.515861f,0.788633f);
	pv[30].vPosition = D3DXVECTOR3(-0.639949f,0.516806f,0.568662f); pv[30].vNormal = D3DXVECTOR3(-0.646643f,0.515861f,0.561908f);
	pv[31].vPosition = D3DXVECTOR3(0.000000f,-1.000000f,0.000000f); pv[31].vNormal = D3DXVECTOR3(0.000000f,-1.000000f,0.000000f);
	pv[32].vPosition = D3DXVECTOR3(0.212032f,-0.932670f,0.291836f); pv[32].vNormal = D3DXVECTOR3(0.206783f,-0.936075f,0.284612f);
	pv[33].vPosition = D3DXVECTOR3(0.343074f,-0.932670f,-0.111472f); pv[33].vNormal = D3DXVECTOR3(0.334582f,-0.936075f,-0.108712f);
	pv[34].vPosition = D3DXVECTOR3(0.395510f,-0.739749f,0.544373f); pv[34].vNormal = D3DXVECTOR3(0.399648f,-0.733285f,0.550068f);
	pv[35].vPosition = D3DXVECTOR3(0.577350f,-0.794654f,0.187592f); pv[35].vNormal = D3DXVECTOR3(0.577350f,-0.794654f,0.187592f);
	pv[36].vPosition = D3DXVECTOR3(0.639949f,-0.739749f,-0.207932f); pv[36].vNormal = D3DXVECTOR3(0.646643f,-0.733285f,-0.210107f);
	pv[37].vPosition = D3DXVECTOR3(0.525731f,-0.447214f,0.723607f); pv[37].vNormal = D3DXVECTOR3(0.525731f,-0.447214f,0.723607f);
	pv[38].vPosition = D3DXVECTOR3(0.738585f,-0.516806f,0.432902f); pv[38].vNormal = D3DXVECTOR3(0.734230f,-0.515861f,0.441355f);
	pv[39].vPosition = D3DXVECTOR3(0.851981f,-0.516806f,0.083904f); pv[39].vNormal = D3DXVECTOR3(0.853426f,-0.515861f,0.074505f);
	pv[40].vPosition = D3DXVECTOR3(0.850651f,-0.447214f,-0.276393f); pv[40].vNormal = D3DXVECTOR3(0.850651f,-0.447214f,-0.276393f);
	pv[41].vPosition = D3DXVECTOR3(0.000000f,-0.932670f,-0.360730f); pv[41].vNormal = D3DXVECTOR3(0.000000f,-0.936075f,-0.351800f);
	pv[42].vPosition = D3DXVECTOR3(0.356822f,-0.794654f,-0.491123f); pv[42].vNormal = D3DXVECTOR3(0.356822f,-0.794654f,-0.491123f);
	pv[43].vPosition = D3DXVECTOR3(0.000000f,-0.739749f,-0.672883f); pv[43].vNormal = D3DXVECTOR3(0.000000f,-0.733285f,-0.679921f);
	pv[44].vPosition = D3DXVECTOR3(0.639949f,-0.516806f,-0.568662f); pv[44].vNormal = D3DXVECTOR3(0.646643f,-0.515861f,-0.561908f);
	pv[45].vPosition = D3DXVECTOR3(0.343074f,-0.516806f,-0.784354f); pv[45].vNormal = D3DXVECTOR3(0.334582f,-0.515861f,-0.788633f);
	pv[46].vPosition = D3DXVECTOR3(0.000000f,-0.447214f,-0.894427f); pv[46].vNormal = D3DXVECTOR3(-0.000000f,-0.447214f,-0.894427f);
	pv[47].vPosition = D3DXVECTOR3(-0.343074f,-0.932670f,-0.111472f); pv[47].vNormal = D3DXVECTOR3(-0.334582f,-0.936075f,-0.108712f);
	pv[48].vPosition = D3DXVECTOR3(-0.356822f,-0.794654f,-0.491123f); pv[48].vNormal = D3DXVECTOR3(-0.356822f,-0.794654f,-0.491123f);
	pv[49].vPosition = D3DXVECTOR3(-0.639949f,-0.739749f,-0.207932f); pv[49].vNormal = D3DXVECTOR3(-0.646643f,-0.733285f,-0.210107f);
	pv[50].vPosition = D3DXVECTOR3(-0.343074f,-0.516806f,-0.784354f); pv[50].vNormal = D3DXVECTOR3(-0.334582f,-0.515861f,-0.788633f);
	pv[51].vPosition = D3DXVECTOR3(-0.639949f,-0.516806f,-0.568662f); pv[51].vNormal = D3DXVECTOR3(-0.646643f,-0.515861f,-0.561908f);
	pv[52].vPosition = D3DXVECTOR3(-0.850651f,-0.447214f,-0.276393f); pv[52].vNormal = D3DXVECTOR3(-0.850651f,-0.447214f,-0.276393f);
	pv[53].vPosition = D3DXVECTOR3(-0.212032f,-0.932670f,0.291836f); pv[53].vNormal = D3DXVECTOR3(-0.206783f,-0.936075f,0.284612f);
	pv[54].vPosition = D3DXVECTOR3(-0.577350f,-0.794654f,0.187592f); pv[54].vNormal = D3DXVECTOR3(-0.577350f,-0.794654f,0.187592f);
	pv[55].vPosition = D3DXVECTOR3(-0.395510f,-0.739749f,0.544373f); pv[55].vNormal = D3DXVECTOR3(-0.399648f,-0.733285f,0.550068f);
	pv[56].vPosition = D3DXVECTOR3(-0.851981f,-0.516806f,0.083904f); pv[56].vNormal = D3DXVECTOR3(-0.853427f,-0.515861f,0.074505f);
	pv[57].vPosition = D3DXVECTOR3(-0.738585f,-0.516806f,0.432902f); pv[57].vNormal = D3DXVECTOR3(-0.734230f,-0.515861f,0.441355f);
	pv[58].vPosition = D3DXVECTOR3(-0.525731f,-0.447214f,0.723607f); pv[58].vNormal = D3DXVECTOR3(-0.525731f,-0.447214f,0.723607f);
	pv[59].vPosition = D3DXVECTOR3(0.000000f,-0.794654f,0.607062f); pv[59].vNormal = D3DXVECTOR3(-0.000000f,-0.794654f,0.607062f);
	pv[60].vPosition = D3DXVECTOR3(-0.183479f,-0.516806f,0.836210f); pv[60].vNormal = D3DXVECTOR3(-0.192865f,-0.515861f,0.834680f);
	pv[61].vPosition = D3DXVECTOR3(0.183479f,-0.516806f,0.836210f); pv[61].vNormal = D3DXVECTOR3(0.192865f,-0.515861f,0.834680f);
	pv[62].vPosition = D3DXVECTOR3(0.212032f,0.156076f,0.964719f); pv[62].vNormal = D3DXVECTOR3(0.206783f,0.164060f,0.964534f);
	pv[63].vPosition = D3DXVECTOR3(0.577350f,0.187592f,0.794654f); pv[63].vNormal = D3DXVECTOR3(0.577350f,0.187592f,0.794654f);
	pv[64].vPosition = D3DXVECTOR3(0.395510f,-0.156076f,0.905103f); pv[64].vNormal = D3DXVECTOR3(0.399648f,-0.164060f,0.901868f);
	pv[65].vPosition = D3DXVECTOR3(0.851981f,0.156076f,0.499769f); pv[65].vNormal = D3DXVECTOR3(0.853426f,0.164060f,0.494720f);
	pv[66].vPosition = D3DXVECTOR3(0.738585f,-0.156076f,0.655845f); pv[66].vNormal = D3DXVECTOR3(0.734230f,-0.164060f,0.658780f);
	pv[67].vPosition = D3DXVECTOR3(0.983024f,0.156076f,0.096461f); pv[67].vNormal = D3DXVECTOR3(0.981225f,0.164060f,0.101395f);
	pv[68].vPosition = D3DXVECTOR3(0.934172f,0.187592f,-0.303531f); pv[68].vNormal = D3DXVECTOR3(0.934172f,0.187592f,-0.303531f);
	pv[69].vPosition = D3DXVECTOR3(0.983024f,-0.156076f,-0.096461f); pv[69].vNormal = D3DXVECTOR3(0.981225f,-0.164060f,-0.101395f);
	pv[70].vPosition = D3DXVECTOR3(0.738585f,0.156076f,-0.655845f); pv[70].vNormal = D3DXVECTOR3(0.734230f,0.164060f,-0.658780f);
	pv[71].vPosition = D3DXVECTOR3(0.851981f,-0.156076f,-0.499769f); pv[71].vNormal = D3DXVECTOR3(0.853427f,-0.164060f,-0.494720f);
	pv[72].vPosition = D3DXVECTOR3(0.395510f,0.156076f,-0.905103f); pv[72].vNormal = D3DXVECTOR3(0.399648f,0.164060f,-0.901868f);
	pv[73].vPosition = D3DXVECTOR3(0.000000f,0.187592f,-0.982247f); pv[73].vNormal = D3DXVECTOR3(-0.000000f,0.187592f,-0.982247f);
	pv[74].vPosition = D3DXVECTOR3(0.212032f,-0.156076f,-0.964719f); pv[74].vNormal = D3DXVECTOR3(0.206783f,-0.164060f,-0.964534f);
	pv[75].vPosition = D3DXVECTOR3(-0.395510f,0.156076f,-0.905103f); pv[75].vNormal = D3DXVECTOR3(-0.399648f,0.164060f,-0.901868f);
	pv[76].vPosition = D3DXVECTOR3(-0.212032f,-0.156076f,-0.964719f); pv[76].vNormal = D3DXVECTOR3(-0.206783f,-0.164060f,-0.964534f);
	pv[77].vPosition = D3DXVECTOR3(-0.738585f,0.156076f,-0.655845f); pv[77].vNormal = D3DXVECTOR3(-0.734230f,0.164060f,-0.658780f);
	pv[78].vPosition = D3DXVECTOR3(-0.934172f,0.187592f,-0.303531f); pv[78].vNormal = D3DXVECTOR3(-0.934172f,0.187592f,-0.303531f);
	pv[79].vPosition = D3DXVECTOR3(-0.851981f,-0.156076f,-0.499769f); pv[79].vNormal = D3DXVECTOR3(-0.853427f,-0.164060f,-0.494720f);
	pv[80].vPosition = D3DXVECTOR3(-0.983024f,0.156076f,0.096461f); pv[80].vNormal = D3DXVECTOR3(-0.981225f,0.164060f,0.101395f);
	pv[81].vPosition = D3DXVECTOR3(-0.983024f,-0.156076f,-0.096461f); pv[81].vNormal = D3DXVECTOR3(-0.981225f,-0.164060f,-0.101395f);
	pv[82].vPosition = D3DXVECTOR3(-0.851981f,0.156076f,0.499769f); pv[82].vNormal = D3DXVECTOR3(-0.853426f,0.164060f,0.494720f);
	pv[83].vPosition = D3DXVECTOR3(-0.577350f,0.187592f,0.794654f); pv[83].vNormal = D3DXVECTOR3(-0.577350f,0.187592f,0.794654f);
	pv[84].vPosition = D3DXVECTOR3(-0.738585f,-0.156076f,0.655845f); pv[84].vNormal = D3DXVECTOR3(-0.734230f,-0.164060f,0.658780f);
	pv[85].vPosition = D3DXVECTOR3(-0.212032f,0.156076f,0.964719f); pv[85].vNormal = D3DXVECTOR3(-0.206783f,0.164060f,0.964534f);
	pv[86].vPosition = D3DXVECTOR3(-0.395510f,-0.156076f,0.905103f); pv[86].vNormal = D3DXVECTOR3(-0.399648f,-0.164060f,0.901868f);
	pv[87].vPosition = D3DXVECTOR3(0.934172f,-0.187592f,0.303531f); pv[87].vNormal = D3DXVECTOR3(0.934172f,-0.187592f,0.303531f);
	pv[88].vPosition = D3DXVECTOR3(0.577350f,-0.187592f,-0.794654f); pv[88].vNormal = D3DXVECTOR3(0.577350f,-0.187592f,-0.794654f);
	pv[89].vPosition = D3DXVECTOR3(-0.577350f,-0.187592f,-0.794654f); pv[89].vNormal = D3DXVECTOR3(-0.577350f,-0.187592f,-0.794654f);
	pv[90].vPosition = D3DXVECTOR3(-0.934172f,-0.187592f,0.303531f); pv[90].vNormal = D3DXVECTOR3(-0.934172f,-0.187592f,0.303531f);
	pv[91].vPosition = D3DXVECTOR3(0.000000f,-0.187592f,0.982247f); pv[91].vNormal = D3DXVECTOR3(0.000000f,-0.187592f,0.982247f);
}