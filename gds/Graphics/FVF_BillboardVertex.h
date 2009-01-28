#ifndef __FVFBILLBOARDVERTEX_H__
#define __FVFBILLBOARDVERTEX_H__

#include <d3d9.h>
#include <d3dx9.h>
#include "TextureCoord.h"


struct BILLBOARDVERTEX
{
	D3DXVECTOR3 vPosition;

	D3DCOLOR color;

	TEXCOORD2 tex;

	/// local offset of one of the four rectangle vertices
	D3DXVECTOR2 local_offset;

	/// multiplied with local_offset to determine the final local offset
	float factor;

	enum eFVF { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE3(1) };
};


const D3DVERTEXELEMENT9 BILLBOARDVERTEX_DECLARATION[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
	{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	D3DDECL_END()
};



struct STATICBILLBOARDVERTEX
{
	D3DXVECTOR3 vPosition;

	D3DCOLOR color;

	TEXCOORD2 tex;

	/// local offset of one of the four rectangle vertices
	D3DXVECTOR2 local_offset;

	enum eFVF { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2 };
};


const D3DVERTEXELEMENT9 STATICBILLBOARDVERTEX_DECLARATION[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
	{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
	D3DDECL_END()
};


#endif	/* __FVFBILLBOARDVERTEX_H__ */