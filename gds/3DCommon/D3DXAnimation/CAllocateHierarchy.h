#ifndef __CALLOCATEHIERARCHY_H__
#define __CALLOCATEHIERARCHY_H__

#include <d3dx9.h>
class CD3DXObject;

class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
public:
	STDMETHOD( CreateFrame )( THIS_ LPCTSTR Name, LPD3DXFRAME *ppNewFrame );
	STDMETHOD( CreateMeshContainer )( THIS_ LPCTSTR Name, LPD3DXMESHDATA pMeshData,
					LPD3DXMATERIAL pMaterials, LPD3DXEFFECTINSTANCE pEffectInstances, DWORD NumMaterials, 
					DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, 
					LPD3DXMESHCONTAINER *ppNewMeshContainer );
	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerBase );
	CAllocateHierarchy( CD3DXObject *pCD3DXObject );

private:
	CD3DXObject* pCD3DXObject;
};

#endif // __CALLOCATEHIERARCHY_H__
