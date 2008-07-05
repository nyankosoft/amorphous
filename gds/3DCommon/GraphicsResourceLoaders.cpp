#include "GraphicsResourceLoaders.h"



bool CDiskTextureLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	CTextureEntry *pTextureEntry = dynamic_cast<CTextureEntry *>(pDestEntry);
	if( !pTextureEntry )
		return false;

	CLockedTexture locked_texture;
//	pTextureEntry->GetLockedTexture(  );
	FillTexture( locked_texture );

	return true;
}


void CDiskTextureLoader::FillTexture( CLockedTexture& texture )
{
//	if( !m_Image.IsLoaded() )
//		return;

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = m_Image.GetWidth();
	const int h = m_Image.GetHeight();
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			texture.SetPixelARGB32( x, y, m_Image.GetPixelARGB32(x,y) );
		}
	}
}


bool CMeshLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	CMeshObjectEntry *pMeshEntry = dynamic_cast<CMeshObjectEntry *>(pDestEntry);
	if( !pMeshEntry )
		return false;

	CD3DXMeshObjectBase *pMesh = pMeshEntry->GetMeshObject();
	if( !pMesh )
		return false;

	// copy vertices to VB / IB?

//	pMesh->


	return true;
}
