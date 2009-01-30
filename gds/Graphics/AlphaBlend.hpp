#ifndef __ALPHABLEND_H__
#define __ALPHABLEND_H__

class AlphaBlend
{
public:
	enum Mode
	{
		Zero,
		One,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,	
		NumModes
	};
};

class TextureAddress
{
public:
	enum Mode
	{
		Wrap,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,
		NumModes
	};
};

//---------------------------- D3D adapter ----------------------------

#include <d3dx9.h>

// conversion table: generic alpha blend mode enums > d3d blend enums
static const DWORD g_dwD3DBlendMode[] =
{
	D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA
};


static const DWORD g_dwD3DTextureAddressMode[] =
{
	D3DTADDRESS_WRAP,
	D3DTADDRESS_MIRROR,
	D3DTADDRESS_CLAMP,
	D3DTADDRESS_BORDER,
	D3DTADDRESS_MIRRORONCE
};


inline DWORD ToD3DTextureAddressMode( TextureAddress::Mode address_mode )
{
	return g_dwD3DTextureAddressMode[ (int)address_mode ];
}


//---------------------------- D3D adapter ----------------------------


#endif  /* __ALPHABLEND_H__ */
