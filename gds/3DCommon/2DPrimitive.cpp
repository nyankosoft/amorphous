#include "2DPrimitive.h"


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
