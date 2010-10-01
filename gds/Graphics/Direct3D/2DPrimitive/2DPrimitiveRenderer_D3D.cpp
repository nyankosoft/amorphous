#include "2DPrimitiveRenderer_D3D.hpp"
#include "Graphics/Direct3D/Conversions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"


static LPDIRECT3DVERTEXDECLARATION9 sg_pTLVertDeclaration = NULL;


D3DPRIMITIVETYPE ToD3DPrimitiveType( PrimitiveType::Name pt )
{
	switch( pt )
	{
	case PrimitiveType::TRIANGLE_LIST:  return D3DPT_TRIANGLELIST;
	case PrimitiveType::TRIANGLE_FAN:   return D3DPT_TRIANGLEFAN;
	case PrimitiveType::TRIANGLE_STRIP: return D3DPT_TRIANGLESTRIP;
	default:
		return D3DPT_TRIANGLELIST;
	}
}



void C2DPrimitiveRenderer_D3D::Init()
{
	HRESULT hr;

	if( m_pVertexDecleration )
	{
		m_pVertexDecleration->Release();
		m_pVertexDecleration = NULL;
	}

	const D3DVERTEXELEMENT9 aVertElem[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,     0 },
		{ 0, 32, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
		{ 0, 40, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
		{ 0, 48, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2 },
		{ 0, 56, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3 },
		D3DDECL_END()
	};

	hr = DIRECT3D9.GetDevice()->CreateVertexDeclaration( aVertElem, &m_pVertexDecleration );


	if( m_pColoredVertexDecleration )
	{
		m_pColoredVertexDecleration->Release();
		m_pColoredVertexDecleration = NULL;
	}

	const D3DVERTEXELEMENT9 aColorVertElem[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,     0 },
		D3DDECL_END()
	};

	hr = DIRECT3D9.GetDevice()->CreateVertexDeclaration( aColorVertElem, &m_pColoredVertexDecleration );


	if( sg_pTLVertDeclaration )
	{
		sg_pTLVertDeclaration->Release();
		sg_pTLVertDeclaration = NULL;
	}

	const D3DVERTEXELEMENT9 aTLVertElem[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,     0 },
		D3DDECL_END()
	};

	hr = DIRECT3D9.GetDevice()->CreateVertexDeclaration( aTLVertElem, &sg_pTLVertDeclaration );
}


void C2DPrimitiveRenderer_D3D::Release()
{
	SAFE_RELEASE( m_pVertexDecleration );

	SAFE_RELEASE( m_pColoredVertexDecleration );
	SAFE_RELEASE( sg_pTLVertDeclaration );
}

int GetNumPrimitivesToDraw( PrimitiveType::Name pt, int num_vertices )
{
	switch(pt)
	{
	case PrimitiveType::TRIANGLE_LIST:  return num_vertices / 2;
	case PrimitiveType::TRIANGLE_FAN:   return num_vertices - 2;
	case PrimitiveType::TRIANGLE_STRIP: return num_vertices - 2;
	default:
		return 0;
	}
}


inline void C2DPrimitiveRenderer_D3D::CopyVertices( CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	if( (int)m_vecTLVertex.size() < num_vertices )
		m_vecTLVertex.resize( num_vertices );

	for( int i=0; i<num_vertices; i++ )
	{
		m_vecTLVertex[i].vPosition   = ToD3DXVECTOR3( paVertex[i].m_vPosition );
		m_vecTLVertex[i].color       = paVertex[i].m_DiffuseColor.GetARGB32();
		m_vecTLVertex[i].rhw         = paVertex[i].m_fRHW;
		m_vecTLVertex[i].tu          = paVertex[i].m_TextureCoord[0].u;
		m_vecTLVertex[i].tv          = paVertex[i].m_TextureCoord[0].v;
	}
}


void C2DPrimitiveRenderer_D3D::Render( CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	if( !paVertex )
		return;

	D3DPRIMITIVETYPE d3d_pt = ToD3DPrimitiveType( primitive_type );

	int num_primitives = GetNumPrimitivesToDraw( primitive_type, num_vertices );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr = S_OK;

	// use the general 2d vertex

	// For now, always draw primitives with TLVERTEX vertices
	// RGBA color can be represented by 4 floats represent when primitives are rendered with programmable shader.
	// Some graphics card accepts this even in the fixed function pipeline mode.
	// On some graphics card, however, the program crashes if DrawPrimitiveUP() is called to draw primitives
	// with a vertex declaration that uses float4 diffuse colors,
/*	const bool using_programmable_shader = false;
	if( using_programmable_shader )
	{

		hr = pd3dDev->SetVertexDeclaration( m_pVertexDecleration );

		hr = pd3dDev->DrawPrimitiveUP( d3d_pt, num_vertices - 2, paVertex, sizeof(CGeneral2DVertex) );

		return;
	}
*/

	// use the colored 2d vertex (lighter vertex with no texture coords)
	// Still crashes on some graphics card
/*
	CColored2DVertex verts[64];
	for( int i=0; i<num_vertices && i<numof(verts); i++ )
	{
		verts[i].m_vPosition    = paVertex[i].m_vPosition;
		verts[i].m_DiffuseColor = paVertex[i].m_DiffuseColor;
		verts[i].m_fRHW         = paVertex[i].m_fRHW;
	}

	hr = pd3dDev->SetVertexDeclaration( m_pColoredVertexDecleration );

	hr = pd3dDev->DrawPrimitiveUP( d3d_pt, num_vertices - 2, verts, sizeof(CColored2DVertex) );
*/

	CopyVertices( paVertex, num_vertices, primitive_type );

	// render with the fixed function pipeline
	// - safe practice in fixed function pipeline mode

	// clear programmable shaders
	hr = pd3dDev->SetVertexShader( NULL );
	hr = pd3dDev->SetPixelShader( NULL );

	hr = pd3dDev->SetFVF( TLVERTEX::FVF );
//	hr = pd3dDev->SetVertexDeclaration( sg_pTLVertDeclaration );

	hr = pd3dDev->DrawPrimitiveUP( d3d_pt, num_vertices - 2, &m_vecTLVertex[0], sizeof(TLVERTEX) );
}


void C2DPrimitiveRenderer_D3D::Render( CShaderManager& rShaderManager, CGeneral2DVertex *paVertex, int num_vertices, PrimitiveType::Name primitive_type )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = rShaderManager.GetEffect();

	if( !pEffect )
		return;

	D3DPRIMITIVETYPE d3d_pt = ToD3DPrimitiveType( primitive_type );

	int num_primitives = GetNumPrimitivesToDraw( primitive_type, num_vertices );

	CopyVertices( paVertex, num_vertices, primitive_type );

//	for( int tex=0; tex<num_textures; tex++ )
//		rShaderManager.SetTexture( tex, GetTexture( mat, tex ) );

	HRESULT hr = S_OK;

//	hr = pd3dDev->SetFVF( TLVERTEX::FVF );
	hr = pd3dDev->SetFVF( TLVERTEX2::FVF );

	hr = pEffect->CommitChanges();

	UINT cPasses = 0;
	hr = pEffect->Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		hr = pEffect->BeginPass( p );

		// Draw the mesh subset
		hr = pd3dDev->DrawPrimitiveUP( d3d_pt, num_vertices - 2, &m_vecTLVertex[0], sizeof(TLVERTEX) );

		hr = pEffect->EndPass();
	}

	hr = pEffect->End();
}


void C2DPrimitiveRenderer_D3D::RenderRect( CShaderManager& rShaderManager, const C2DRect& rect )
{
	CGeneral2DVertex verts[4];
	for( int i=0; i<4; i++ )
		verts[i] = rect.GetVertex( i );

	Render( rShaderManager, verts, 4, PrimitiveType::TRIANGLE_FAN );
}
