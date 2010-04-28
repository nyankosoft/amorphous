#include "LinePrimitives.hpp"

using namespace std;


void CLinePrimitives::SetPoints( const std::vector<Vector3>& vecPoints, U32 color )
{
	m_vecPoint.resize( vecPoints.size() );

	const size_t num = vecPoints.size();
	for( size_t i=0; i<num; i++ )
	{
		m_vecPoint[i].vPosition = vecPoints[i];

		m_vecPoint[i].color = color;

		m_vecPoint[i].tex = TEXCOORD2(0,0);
	}
}


void CLineStrip::Draw()
{
	if( m_vecPoint.size() == 0 )
		return;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

//	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	// draw connected lines
	HRESULT hr ;

	hr = pd3dDev->SetFVF( TEXTUREVERTEX::FVF );
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, (UINT)(m_vecPoint.size() - 1), &(m_vecPoint[0]), sizeof(TEXTUREVERTEX) );

}


void CLineList::Draw()
{
	if( m_vecPoint.size() == 0 )
		return;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// turn off programmable shaders
	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetPixelShader( NULL );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

//	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	// draw connected lines
	HRESULT hr ;

	hr = pd3dDev->SetFVF( TEXTUREVERTEX::FVF );
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_LINELIST, (UINT)(m_vecPoint.size() / 2), &(m_vecPoint[0]), sizeof(TEXTUREVERTEX) );

}


void CLineList::SetLineSegmentColor( int line_segment_index, U32 color )
{
	if( (int)m_vecPoint.size() / 2 <= line_segment_index )
		return;

	m_vecPoint[line_segment_index * 2    ].color = color;
	m_vecPoint[line_segment_index * 2 + 1].color = color;
}


void CLineList::AddLineSegment( Vector3 start, Vector3 end, U32 color )
{
	Vector3 pos[2] = { start, end };
	for( int i=0; i<2; i++ )
	{
//		m_vecPoint.push_back( TEXTUREVERTEX ); // error. why?
		m_vecPoint.push_back( TEXTUREVERTEX() );
		m_vecPoint.back().vPosition = pos[i];
		m_vecPoint.back().color     = color;
		m_vecPoint.back().tex       = TEXCOORD2( 0.0f, 0.0f );
	}
}
