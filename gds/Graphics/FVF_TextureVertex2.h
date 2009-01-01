#ifndef __FVFTEXTUREVERTEX2_H__
#define __FVFTEXTUREVERTEX2_H__

#include <d3d9.h>
#include <d3dx9.h>

struct TEXTUREVERTEX2
{
	D3DXVECTOR3 vPosition;
	D3DCOLOR color;
	D3DXVECTOR2 vTexture1;
	D3DXVECTOR2 vTexture2;
};

#define D3DFVF_TEXTUREVERTEX2 (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2)

#endif	/* __FVFTEXTUREVERTEX2_H__ */