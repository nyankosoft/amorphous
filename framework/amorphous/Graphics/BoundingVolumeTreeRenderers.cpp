#include "BoundingVolumeTreeRenderers.hpp"
#include "GraphicsDevice.hpp"
#include "PrimitiveShapeRenderer.hpp"
#include "../3DMath/OBBTree.hpp"


namespace amorphous
{


static void DrawOBB( const OBBDATA& OBBData )
{
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, false );
	Matrix34 pose;
	pose.vPosition = OBBData.Center;
	for( int i=0; i<3; i++ )
		pose.matOrient.SetColumn( i, OBBData.Axis[i] );
	const Vector3 edges = Vector3( OBBData.Length[0], OBBData.Length[1], OBBData.Length[2] );
	PrimitiveShapeRenderer shape_renderer;
	shape_renderer.RenderWireframeBox( edges, pose, SFloatRGBAColor::White() );
//	shape_renderer.RenderWireframeBox( Vector3(1,1,1), Matrix34Identity(), SFloatRGBAColor::Yellow() );
}


static void RenderOBBNode( const OBBNODE *pOBBNode, int draw_level, int level )
{
	if( level == draw_level ){
		DrawOBB( pOBBNode->OBBData );
		return;
	}
	++level;

	if( pOBBNode->pOBBNodeL ) RenderOBBNode( pOBBNode->pOBBNodeL, draw_level, level );
	if( pOBBNode->pOBBNodeR ) RenderOBBNode( pOBBNode->pOBBNodeR, draw_level, level );
}


void RenderOBBTree( const OBBTree& obb_tree, int draw_level )
{
	RenderOBBNode( obb_tree.GetTopNode(), draw_level, 0 );
}


} // namespace amorphous
