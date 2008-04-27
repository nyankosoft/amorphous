#include "CD3DXFrame.h"
#include "../DXUtil.h"

CD3DXFrame::CD3DXFrame()
{
	// D3DXFRAME —R—ˆ‚Ì•Ï”‚ð‰Šú‰»
	Name = NULL;
	D3DXMatrixIdentity( &TransformationMatrix );
	pMeshContainer = NULL;
	pFrameSibling = NULL;
	pFrameFirstChild = NULL;

	// ƒƒ“ƒo•Ï”‰Šú‰»
	D3DXMatrixIdentity( &CombinedTransformationMatrix );
}

CD3DXFrame::~CD3DXFrame()
{
	SAFE_DELETE( Name );
}

void CD3DXFrame::SetName( LPCTSTR Name )
{
	if( Name ){
		UINT Length;
		Length = lstrlen( Name ) + 1;
		SAFE_DELETE( this->Name );
		this->Name = new TCHAR[Length];
		memcpy( this->Name, Name, Length * sizeof( TCHAR ) );
	}
}
