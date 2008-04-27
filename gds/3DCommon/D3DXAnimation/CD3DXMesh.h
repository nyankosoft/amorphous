#ifndef __CD3DXMESH_H__
#define __CD3DXMESH_H__

#include <d3dx9.h>

class CD3DXMesh: public D3DXMESHCONTAINER
{
public:
	CD3DXMesh();
	~CD3DXMesh();
	void SetName( LPCTSTR Name );

public:
    LPDIRECT3DTEXTURE9*  ppTextures;       // array of textures, entries are NULL if no texture specified    
                                
    // SkinMesh info             
    LPD3DXMESH           pOrigMesh;
    LPD3DXATTRIBUTERANGE pAttributeTable;
    DWORD                NumAttributeGroups; 
    LPD3DXBUFFER         pBoneCombinationBuf;
    D3DXMATRIX**         ppBoneMatrixPtrs;
    D3DXMATRIX*          pBoneOffsetMatrices;
};

#endif // __CD3DXMESH_H__
