#ifndef  __LINESEGMENTRENDERER_HPP__
#define  __LINESEGMENTRENDERER_HPP__


#include "3DMath/ray.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/FVF_TLVertex.h"

class CLineSegmentRenderer
{
public:
	CLineSegmentRenderer() {}
	~CLineSegmentRenderer() {}

	inline static void Draw( const Vector3& rvStart, const Vector3& rvEnd, DWORD dwColor = 0xFFFFFFFF );
	inline static void Draw( const CLineSegment& line_segment, DWORD dwColor = 0xFFFFFFFF );
};


class C2DLineSegment
{
public:
	C2DLineSegment() {}
	inline static void Draw( const D3DXVECTOR2& rvStart, const D3DXVECTOR2& rvEnd, DWORD dwColor = 0xFFFFFFFF );
};


//CLineSegmentRenderer::CLineSegmentRenderer(){}
//CLineSegmentRenderer::~CLineSegmentRenderer(){}


inline void CLineSegmentRenderer::Draw( const Vector3& rvStart, const Vector3& rvEnd, DWORD dwColor )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	pd3dDevice->SetTexture( 0, NULL );

	//We use only the first texture stage (stage 0)
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	// alpha arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	// alpha-blending settings
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	COLORVERTEX avLineSegment[2];

	avLineSegment[0].vPosition = rvStart;
	avLineSegment[0].color = dwColor;
	avLineSegment[1].vPosition = rvEnd;
	avLineSegment[1].color = dwColor;

    pd3dDevice->SetFVF( COLORVERTEX::FVF );
//	pd3dDevice->SetVertexShader(NULL);

	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, avLineSegment, sizeof(COLORVERTEX) );
}


inline void CLineSegmentRenderer::Draw( const CLineSegment& line_segment, DWORD dwColor )
{
	CLineSegmentRenderer::Draw( line_segment.vStart, line_segment.vGoal, dwColor );
}



inline void C2DLineSegment::Draw( const D3DXVECTOR2& rvStart, const D3DXVECTOR2& rvEnd, DWORD dwColor )
{

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	pd3dDevice->SetTexture( 0, NULL );

	//We use only the first texture stage (stage 0)
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	// alpha arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	// alpha-blending settings
//	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	TLVERTEX avVertex[2];

	avVertex[0].vPosition = D3DXVECTOR3(rvStart.x, rvStart.y, 0);
	avVertex[1].vPosition = D3DXVECTOR3(rvEnd.x,   rvEnd.y, 0);

	avVertex[0].rhw = 1.0f;
	avVertex[1].rhw = 1.0f;
	avVertex[0].color = dwColor;
	avVertex[1].color = dwColor;


	pd3dDevice->SetFVF( D3DFVF_TLVERTEX );
//	pd3dDevice->SetVertexShader(NULL);

	pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, avVertex, sizeof(TLVERTEX) );


}

#endif		/*  __LINESEGMENTRENDERER_HPP__  */
