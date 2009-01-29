#ifndef  __TEXTURETOOL_H__
#define  __TEXTURETOOL_H__

#include <d3d9.h>
#include <d3dx9.h>


class CTextureTool
{
public:
	CTextureTool() {}
	~CTextureTool() {}

	static bool CreateTexture( DWORD *padwTexelData, int iWidth, int iHeight, LPDIRECT3DTEXTURE9 *ppTexture );

};



#endif		/*  __TEXTURETOOL_H__  */
