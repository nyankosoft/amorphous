#include "2DTriangle.h"
#include "3DMath/aabb2.h"


void C2DTriangle::SetPosition( const Vector2& vMin, const Vector2& vMax )
{
	AABB2 aabb;
	aabb.Nullify();
	const Vector2 vOrigPos[3] = { GetPosition2D(0), GetPosition2D(1), GetPosition2D(2) };
	for( int i=0; i<3; i++ )
		aabb.AddPoint( vOrigPos[i] );

	Vector2 vSpan = aabb.vMax - aabb.vMin;
	const float scale_x = (vMax.x - vMin.x) / vSpan.x;
	const float scale_y = (vMax.y - vMin.y) / vSpan.y;

	for( int i=0; i<3; i++ )
	{
		Vector2 vLocalPos = vOrigPos[i] - aabb.vMin;
		SetPosition( i, vMin + Vector2( vLocalPos.x * scale_x, vLocalPos.y * scale_y ) );
	}
}
