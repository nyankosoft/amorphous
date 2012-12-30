#ifndef __FVF_MAPVERTEX_H__
#define __FVF_MAPVERTEX_H__

// MAPVERTEX
//  mainly used for static geometry of game levels


#include <d3dx9.h>
#include "../TextureCoord.hpp"


namespace amorphous
{

struct MAPVERTEX
{
	D3DXVECTOR3 vPosition;

	D3DXVECTOR3 vNormal;

	D3DXVECTOR3 vTangent;

	/// diffuse color
	D3DCOLOR color;

	/// surface texture
	TEXCOORD2 vTex0;

	/// static lightmap
	TEXCOORD2 vTex1;

	/// dynamic lightmap
	TEXCOORD2 vTex2;

	inline MAPVERTEX() { memset(this, 0, sizeof(MAPVERTEX)); color = 0xFFFFFFFF; }

	inline MAPVERTEX(const MAPVERTEX& v);

	inline MAPVERTEX operator=( MAPVERTEX v );

	inline MAPVERTEX operator+( MAPVERTEX& v );
	inline MAPVERTEX operator-( MAPVERTEX& v );

	inline MAPVERTEX operator*( float f );
	inline MAPVERTEX operator/( float f );
	
//	static const D3DVERTEXELEMENT9 DECLARATION[];

};


// FVF format
// #define D3DFVF_MAPVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3)

/*
// vertex decleration
const D3DVERTEXELEMENT9 MAPVERTEX::DECLARATION[8] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
    { 0, 36, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
    { 0, 40, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 48, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    { 0, 56, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
    D3DDECL_END()
};*/


// vertex decleration
const D3DVERTEXELEMENT9 MAPVERTEX_DECLARATION[8] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0 },
    { 0, 36, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
    { 0, 40, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 48, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    { 0, 56, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
    D3DDECL_END()
};



// =================================== inline implementations =================================== 

inline MAPVERTEX::MAPVERTEX(const MAPVERTEX& v)
{
	vPosition = v.vPosition;
	vNormal = v.vNormal;
	vTangent = v.vTangent;
	color = v.color;
	vTex0 = v.vTex0;
	vTex1 = v.vTex1;
	vTex2 = v.vTex2;
}


inline MAPVERTEX MAPVERTEX::operator=( MAPVERTEX v )
{
	vPosition = v.vPosition;
	vNormal = v.vNormal;
	vTangent = v.vTangent;
	color = v.color;
	vTex0 = v.vTex0;
	vTex1 = v.vTex1;
	vTex2 = v.vTex2;
	return *this;
}


inline MAPVERTEX MAPVERTEX::operator+( MAPVERTEX& v )
{
	MAPVERTEX out;
	out.vPosition = vPosition + v.vPosition;
	D3DXVec3Normalize( &out.vNormal, &(vNormal + v.vNormal) );
	D3DXVec3Normalize( &out.vTangent, &(vTangent + v.vTangent) );

	out.vTex0 = vTex0 + v.vTex0;
	out.vTex1 = vTex1 + v.vTex1;
	out.vTex2 = vTex2 + v.vTex2;

	out.color = 
		  ( ((color & 0x000000FF) + (v.color & 0x000000FF)) & 0x000000FF )
		| ( ((color & 0x0000FF00) + (v.color & 0x0000FF00)) & 0x0000FF00 )
		| ( ((color & 0x00FF0000) + (v.color & 0x00FF0000)) & 0x00FF0000 )
		| ( ((color & 0xFF000000) + (v.color & 0x00FF0000)) & 0xFF000000 );
			 

/*		out.color = D3DCOLOR_ARGB
		((v.color & 0x000000FF) + (color & 0x000000FF))
		(((v.color >>  8) & 0x000000FF) + ((color >>  8) & 0x000000FF))
		(((v.color >> 16) & 0x000000FF) + ((color >> 16) & 0x000000FF))
		(((v.color >> 24) & 0x000000FF) + ((color >> 24) & 0x000000FF))*/

	return out;
}

inline MAPVERTEX MAPVERTEX::operator-( MAPVERTEX& v )
{
	MAPVERTEX out;
	out.vPosition = vPosition - v.vPosition;
	D3DXVec3Normalize( &out.vNormal, &(vNormal - v.vNormal) );
	D3DXVec3Normalize( &out.vTangent, &(vTangent - v.vTangent) );

	out.vTex0 = vTex0 - v.vTex0;
	out.vTex1 = vTex1 - v.vTex1;

	out.vTex2 = vTex2 - v.vTex2;

	out.color = 
		  ( ((color & 0x000000FF) - (v.color & 0x000000FF)) & 0x000000FF )
		| ( ((color & 0x0000FF00) - (v.color & 0x0000FF00)) & 0x0000FF00 )
		| ( ((color & 0x00FF0000) - (v.color & 0x00FF0000)) & 0x00FF0000 )
		| ( ((color & 0xFF000000) - (v.color & 0x00FF0000)) & 0xFF000000 );

	return out;
}


inline MAPVERTEX MAPVERTEX::operator*( float f )
{
	MAPVERTEX v;
	v.vPosition = vPosition * f;

	v.vTex0 = vTex0 * f;
	v.vTex1 = vTex1 * f;

	v.vTex2 = vTex2 * f;

	v.color = D3DCOLOR_ARGB( 
		  (int)( (float)((color >> 24) & 0x000000FF) * f ),
		  (int)( (float)((color >> 16) & 0x000000FF) * f ),
		  (int)( (float)((color >>  8) & 0x000000FF) * f ),
		  (int)( (float)((color      ) & 0x000000FF) * f ) );

	return v;
}


inline MAPVERTEX MAPVERTEX::operator/( float f )
{
	MAPVERTEX v;
	v.vPosition = vPosition / f;

	v.vTex0 = vTex0 / f;
	v.vTex1 = vTex1 / f;

	v.vTex2 = vTex2 / f;
		
	v.color = D3DCOLOR_ARGB( 
		  (int)( (float)((color >> 24) & 0x000000FF) * f ),
		  (int)( (float)((color >> 16) & 0x000000FF) * f ),
		  (int)( (float)((color >>  8) & 0x000000FF) * f ),
		  (int)( (float)((color      ) & 0x000000FF) * f ) );

	return v;
}

} // amorphous


#endif  /*  __FVF_MAPVERTEX_H__  */
