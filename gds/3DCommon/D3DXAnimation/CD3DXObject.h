#ifndef __CD3DXOBJECT_H__
#define __CD3DXOBJECT_H__

#include <d3dx9.h>
class CD3DXMesh;
class CD3DXFrame;

class CD3DXObject
{
public:
	CD3DXObject();
	~CD3DXObject();
	void Release();
	bool Load( char *pFileName );
	HRESULT GenerateSkinnedMesh( CD3DXMesh *pCD3DXMesh );
	void Render();
	void Update( float Time );
	void UpdateMatricesInPreorder( D3DXMATRIX** papSrcMatrixArray, int &riCount );
	void UpdateGlobalTransformationMatricesInPreorder(
		D3DXMATRIX** papSrcMatrixArray,	int &riCount );
	D3DXMATRIX& GetMatrix();
	void SetMatrix( D3DXMATRIX& rmatWorld ) { matWorld = rmatWorld; }
	bool SetTrackEnable( DWORD Track, BOOL Enable );
	bool SetTrackSpeed( DWORD Track, FLOAT Speed );
	bool SetTrackWeight( DWORD Track, FLOAT Weight );
	void SetupBoneLocalTransformationMatrices( D3DXMATRIX* paSrcMatrixArray, int& riCount );

private:
	bool LoadAnimation( LPD3DXANIMATIONCONTROLLER pAnimControllerTmp );
	void DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pD3DXFrame );
	void DrawFrame( LPD3DXFRAME pD3DXFrame );
	bool SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pD3DXMeshContainer );
	bool SetupBoneMatrixPointers( LPD3DXFRAME pD3DXFrame );
	void SetupBoneLocalTransformMatrices_r( D3DXMATRIX* paSrcMatrixArray,	int& riCount, 
		LPD3DXFRAME pD3DXFrame );
//	void SetGlobalTransformationMatrixPointers_r( vector<D3DXMATRIX *>* pvecpGlobalTransformationMatrixPointer );

	
	void UpdateFrameMatrices( LPD3DXFRAME pD3DXFrame, LPD3DXMATRIX pParentMatrix );
	void UpdateLocalTransformMatricesInPreorder_r( LPD3DXFRAME pD3DXMesh,
													D3DXMATRIX** papSrcMatrixArray,
													int &riCount );
	void UpdateGlobalTransformMatricesInPreorder_r( LPD3DXFRAME pD3DXMesh,
													   D3DXMATRIX** papSrcMatrixArray,
													   int &riCount );
	CD3DXFrame* FindRootBoneFrame_r( LPD3DXFRAME pD3DXMesh );

private:
    UINT						NumBoneMatricesMax;
    D3DXMATRIXA16*				pBoneMatrices;
    LPD3DXFRAME					pFrameRoot;
    LPD3DXANIMATIONCONTROLLER	pAnimController;
	D3DXMATRIX					matWorld;

	CD3DXFrame *m_pRootBoneFrame;
};

#endif // __CD3DXOBJECT_H__
