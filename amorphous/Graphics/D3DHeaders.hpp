#ifndef __D3DHeaders_HPP__
#define __D3DHeaders_HPP__


// Always build with Direct3D on VC++
#ifdef _MSC_VER
#define BUILD_WITH_DIRECT3D
#endif _MSC_VER


#ifdef BUILD_WITH_DIRECT3D

#include <d3d9.h>
#include <d3dx9.h>


namespace amorphous
{

#else /* BUILD_WITH_DIRECT3D */


typedef void* LPDIRECT3DDEVICE9;
typedef void* LPDIRECT3DTEXTURE9;
typedef void* LPD3DXBASEMESH;
typedef void* LPD3DXMESH;
typedef void* LPD3DXEFFECT;


#endif /* BUILD_WITH_DIRECT3D */


} // amorphous



#endif /* __D3DHeaders_HPP__ */
