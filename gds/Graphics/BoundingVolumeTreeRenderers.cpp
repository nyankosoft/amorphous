#include "BoundingVolumeTreeRenderers.hpp"
#include "GraphicsDevice.hpp"
#include "PrimitiveShapeRenderer.hpp"
#include "../3DMath/OBBTree.hpp"


namespace amorphous
{


#define D3DFVF_XYZ_COLOR		( D3DFVF_XYZ | D3DFVF_DIFFUSE )

struct VERTEX_XYZ_COLOR{
    FLOAT x, y, z;
	D3DCOLOR color;
};


static void DrawOBB( const OBBDATA& OBBData )
{
	Vector3 G = OBBData.Center
		- (	OBBData.Axis[0] * OBBData.Length[0]
		  + OBBData.Axis[1] * OBBData.Length[1]
		  + OBBData.Axis[2] * OBBData.Length[2] ) / 2;

	Vector3 Axis[3] = { OBBData.Axis[0] * OBBData.Length[0],
							OBBData.Axis[1] * OBBData.Length[1],
							OBBData.Axis[2] * OBBData.Length[2] };

	VERTEX_XYZ_COLOR Vertex[8] = {
		{ G.x, G.y, G.z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[1].x, G.y + Axis[1].y, G.z + Axis[1].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[1].x + Axis[2].x, G.y + Axis[1].y + Axis[2].y, G.z + Axis[1].z + Axis[2].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[2].x, G.y + Axis[2].y, G.z + Axis[2].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },

		{ G.x + Axis[0].x, G.y + Axis[0].y, G.z + Axis[0].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[0].x + Axis[1].x, G.y + Axis[0].y + Axis[1].y, G.z + Axis[0].z + Axis[1].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[0].x + Axis[1].x + Axis[2].x, G.y + Axis[0].y + Axis[1].y + Axis[2].y, G.z + Axis[0].z + Axis[1].z + Axis[2].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
		{ G.x + Axis[0].x + Axis[2].x, G.y + Axis[0].y + Axis[2].y, G.z + Axis[0].z + Axis[2].z, D3DCOLOR_ARGB( 255, 255, 255, 255 ) },
	};
	unsigned short Index[24] = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};
/*
	DIRECT3D9.GetDevice()->SetVertexShader( NULL );
	DIRECT3D9.GetDevice()->SetPixelShader( NULL );
	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_XYZ_COLOR );
	DIRECT3D9.GetDevice()->SetTexture( 0, NULL );
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, false );
	DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, Index, D3DFMT_INDEX16, Vertex, sizeof( VERTEX_XYZ_COLOR ) );
//	DIRECT3D9.GetDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
*/
	// >>> Render the wireframe box via CPrimitiveShapeRenderer
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, false );
	Matrix34 pose;
	pose.vPosition = OBBData.Center;
	for( int i=0; i<3; i++ )
		pose.matOrient.SetColumn( i, OBBData.Axis[i] );
	const Vector3 edges = Vector3( OBBData.Length[0], OBBData.Length[1], OBBData.Length[2] );
	CPrimitiveShapeRenderer shape_renderer;
	shape_renderer.RenderWireframeBox( edges, pose, SFloatRGBAColor::White() );
//	shape_renderer.RenderWireframeBox( Vector3(1,1,1), Matrix34Identity(), SFloatRGBAColor::Yellow() );
	// <<< Render the wireframe box via CPrimitiveShapeRenderer
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


void RenderOBBTree( const COBBTree& obb_tree, int draw_level )
{
	RenderOBBNode( obb_tree.GetTopNode(), draw_level, 0 );
}


} // namespace amorphous
