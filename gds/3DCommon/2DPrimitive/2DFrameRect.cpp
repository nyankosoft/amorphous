#include "2DFrameRect.h"


/*
void C2DFrameRect::SetTextureUV( const D3DXVECTOR2& rvMin, const D3DXVECTOR2& rvMax)
{
	TLVERTEX* pav2DFrameRect = m_avRectVertex;

	pav2DFrameRect[0].tu = rvMin.x;		// top-left corner of the rectangle
	pav2DFrameRect[0].tv = rvMin.y;

	pav2DFrameRect[1].tu = rvMax.x;
	pav2DFrameRect[1].tv = rvMin.y;

	pav2DFrameRect[2].tu = rvMax.x;		// bottom-right corner of the rectangle
	pav2DFrameRect[2].tv = rvMax.y;

	pav2DFrameRect[3].tu = rvMin.x;
	pav2DFrameRect[3].tv = rvMax.y;
}*/


void C2DFrameRect::SetZDepth(float fZValue)
{
	for(int i=0; i<10; i++)
		m_avRectVertex[i].vPosition.z = fZValue;
}


void C2DFrameRect::ScalePosition( float fScale )
{
	for(int i=0; i<10; i++)
		m_avRectVertex[i].vPosition *= fScale;
}