#include "GraphicsResourceLoaders.h"


bool CDiskTextureLoader::LoadFromFile( const std::string& filepath )
{
	return m_Image.LoadFromFile( m_Desc.Filename );
}


bool CDiskTextureLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	return false;
	CImageArchive img_archive;
	bool retrieved = db.GetData( keyname, img_archive );
	if( retrieved )
	{
		bool loaded = m_Image.CreateFromImageArchive( img_archive );
		return loaded;
	}
	else
		return false;
}


bool CDiskTextureLoader::CopyTo( CGraphicsResourceEntry *pDestEntry )
{
	CTextureEntry *pTextureEntry = dynamic_cast<CTextureEntry *>(pDestEntry);
	if( !pTextureEntry )
		return false;

	CLockedTexture locked_texture;
	bool retrieved = pTextureEntry->GetLockedTexture( locked_texture );
	if( retrieved )
	{
		FillTexture( locked_texture );
		return true;
	}
	else
		return false;
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
