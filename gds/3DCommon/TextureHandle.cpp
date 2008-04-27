#include "TextureHandle.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


//==================================================================================================
// CTextureHandle
//==================================================================================================

const CTextureHandle CTextureHandle::ms_NullHandle;

bool CTextureHandle::Load()
{
	Release();

	m_EntryID = GraphicsResourceManager.LoadTexture( filename );

	if( m_EntryID == -1 )
		return false;	// the loading failed - this is mostly because the texture file was not found
	else
		return true;	// the texture has been successfully loaded
}


bool CTextureHandle::Load( CImageArchive& img_archive )
{
	return false;

/*	Release();

	m_EntryID = GraphicsResourceManager.LoadTexture( img_archive );

	if( m_EntryID == -1 )
		return false;	// the loading failed
	else
		return true;	// the texture has been successfully loaded
*/
}
