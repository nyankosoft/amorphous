#include "CD3DXMesh.h"
#include "../DXUtil.h"

CD3DXMesh::CD3DXMesh()
{
	// D3DXMESHCONTAINER —R—ˆ‚Ì•Ï”‚ğ‰Šú‰»
	Name = NULL;
	ZeroMemory( &MeshData, sizeof( D3DXMESHDATA ) );
	pMaterials = NULL;
	pEffects = NULL;
	NumMaterials = 0;
	pAdjacency = NULL;
	pSkinInfo = NULL;

	// ƒƒ“ƒo•Ï”‚Ì‰Šú‰»
	ppTextures = NULL;
	pOrigMesh = NULL;
	pAttributeTable = NULL;
	NumAttributeGroups = 0; 
	pBoneCombinationBuf = NULL;
	ppBoneMatrixPtrs = NULL;
	pBoneOffsetMatrices = NULL;
}

CD3DXMesh::~CD3DXMesh()
{
	SAFE_DELETE( Name );
	SAFE_RELEASE( MeshData.pMesh );
	SAFE_DELETE_ARRAY( pMaterials );
	SAFE_DELETE( pEffects ); // SAFE_DELETE_ARRAY‚©‚à???
	SAFE_DELETE_ARRAY( pAdjacency );
	SAFE_RELEASE( pSkinInfo );

	if( ppTextures ){
		for( UINT i = 0; i < NumMaterials; i++ ){
			SAFE_RELEASE( ppTextures[i] );
		}
	}
	SAFE_DELETE_ARRAY( ppTextures );
	SAFE_RELEASE( pOrigMesh );
	SAFE_DELETE_ARRAY( pAttributeTable );
	SAFE_RELEASE( pBoneCombinationBuf );
	SAFE_DELETE_ARRAY( ppBoneMatrixPtrs );
	SAFE_DELETE_ARRAY( pBoneOffsetMatrices );
}

void CD3DXMesh::SetName( LPCTSTR Name )
{
	if( Name ){
		UINT Length;
		Length = lstrlen( Name ) + 1;
		SAFE_DELETE( this->Name );
		this->Name = new TCHAR[Length];
		memcpy( this->Name, Name, Length * sizeof( TCHAR ) );
	}
}
