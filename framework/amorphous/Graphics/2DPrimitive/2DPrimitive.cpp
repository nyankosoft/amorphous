#include "2DPrimitive.hpp"


/*
2D primitive classes

Class                    # vertices    # triangles     Primitive types      Parent class
-----------------------------------------------------------------------------------------
C2DTriangle               3            1               TRIANGLE_LIST        C2DPrimitive
C2DRect                   4            2               TRIANGLE_FAN         C2DPrimitive
C2DFrameRect             10            8               TRIANGLE_STRIP       C2DPrimitive
C2DRoundRect             Variable      Variable        TRIANGLE_FAN         C2DPrimitive
C2DRoundFrameRect        Variable      Variable        TRIANGLE_STRIP       C2DRoundRect
C2DRegularPolygon        Variable      Variable        TRIANGLE_STRIP       C2DRoundRect




*/


namespace amorphous
{


void C2DPrimitive::SetTextureCoords( int stretch_x, int stretch_y,
	                                 const TEXCOORD2& left_top_offset,
	                                 TextureAddress::Mode mode )
{
	Vector2 vExtent = GetPosition2D( 2 ) - GetPosition2D( 0 );
	SetTextureUV(
		left_top_offset,
		left_top_offset + TEXCOORD2( vExtent.x / (float)stretch_x, vExtent.y / (float)stretch_y )
		);
}


} // namespace amorphous
