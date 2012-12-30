#ifndef  __TEXTURETOOL_H__
#define  __TEXTURETOOL_H__

#include <d3d9.h>
#include <d3dx9.h>


namespace amorphous
{


class CTextureTool
{
public:
	CTextureTool() {}
	~CTextureTool() {}

	static bool CreateTexture( DWORD *padwTexelData, int iWidth, int iHeight, LPDIRECT3DTEXTURE9 *ppTexture );

};


} // namespace amorphous



#endif		/*  __TEXTURETOOL_H__  */
