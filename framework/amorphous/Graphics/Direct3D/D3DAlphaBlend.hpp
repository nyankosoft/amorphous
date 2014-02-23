#ifndef __D3DAlphaBlend_HPP__
#define __D3DAlphaBlend_HPP__


#include <d3dx9.h>
#include "../AlphaBlend.hpp"


namespace amorphous
{


// conversion table: generic alpha blend mode enums > d3d blend enums
static const DWORD g_dwD3DBlendMode[] =
{
	D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA,
	D3DBLEND_DESTCOLOR,
	D3DBLEND_INVDESTCOLOR,
	D3DBLEND_SRCALPHASAT
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


} // amorphous



#endif /* __D3DAlphaBlend_HPP__ */
