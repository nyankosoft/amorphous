#include "TextureTool.hpp"
#include "Graphics/Direct3D9.hpp"


bool CTextureTool::CreateTexture( DWORD *padwTexelData, int iWidth, int iHeight, LPDIRECT3DTEXTURE9 *ppTexture )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;
	hr = D3DXCreateTexture(pd3dDev, iWidth, iHeight, 0, 0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, ppTexture);
	if( FAILED(hr) )
		return false;

	D3DLOCKED_RECT locked_rect;
	hr = (*ppTexture)->LockRect( 0, &locked_rect, NULL, 0);	// Lock and get the pointer to the first texel of the texture

	if( FAILED(hr) )
		return false;

	D3DCOLOR* pARGBColor = (DWORD *)locked_rect.pBits;
	int i;
	for(i=0; i<iWidth * iHeight; i++)
	{
		pARGBColor[i] = (D3DCOLOR)padwTexelData[i];
	}
	(*ppTexture)->UnlockRect(0);

	D3DXFilterTexture((*ppTexture), NULL, 0, D3DX_FILTER_TRIANGLE);

	return true;
}
