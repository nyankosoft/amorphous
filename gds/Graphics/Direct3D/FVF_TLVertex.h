#ifndef  __FVFTLVERTEX_H__
#define  __FVFTLVERTEX_H__

#include <d3dx9.h>
#include "../TextureCoord.hpp"

struct TLVERTEX
{
	/// transformed position of the vertex
    D3DXVECTOR3 vPosition; 

	float rhw;

	/// vertex diffuse color
    DWORD color;

	/// texture coord
	float tu, tv;

	enum eFVF
	{
		FVF = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
	};
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_TLVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)


//size: 32 bytes
struct TLVERTEX2
{
	D3DXVECTOR3 vPosition;

	float rhw;

    DWORD color;

	TEXCOORD2 tex[2];		///< first & second texture coords

	enum eFVF
	{
		FVF = (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2)
	};
};

#define D3DFVF_TLVERTEX2 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2)


#endif  /*  __FVFTLVERTEX_H__  */