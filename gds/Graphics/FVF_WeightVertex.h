#ifndef __WEIGHTVERTEX_H__
#define __WEIGHTVERTEX_H__

// WEIGHTVERTEX
//  for unlit & textured surface with vertex blending


#include <d3dx9.h>

#include "TextureCoord.hpp"


const D3DVERTEXELEMENT9 WEIGHTVERTEX_DECLARATION[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 }, // 12
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0 }, //  4
    { 0, 16, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 }, // 12
 	{ 0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0 }, //  8
	{ 0, 36, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 }, //  4
	{ 0, 40, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0 }, // 16

    D3DDECL_END()
};


struct WEIGHTVERTEX
{
    D3DXVECTOR3 vPosition;

	/// diffuse color
    DWORD       color;

	/// vertex normal
    D3DXVECTOR3 vNormal;

	/// texturecoord
	TEXCOORD2	tex;

	/// indices to blend matrices
	DWORD       matrixIndices;

	/// blend weights
    D3DXVECTOR4 matrixWeights;

/*	enum eFVF
	{
		FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_NORMAL|D3DFVF_TEX1| matrix indices | matrix blend weights)
	};*/
};


#endif  /*  __WEIGHTVERTEX_H__  */