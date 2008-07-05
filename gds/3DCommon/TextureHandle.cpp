#include "TextureHandle.h"
#include "GraphicsResourceEntries.h"

#include "Support/Log/DefaultLog.h"

using namespace std;


//==================================================================================================
// CTextureHandle
//==================================================================================================

const CTextureHandle CTextureHandle::ms_NullHandle;


void CTextureHandle::IncResourceRefCount()
{
	if( 0 <= m_EntryID )
		GraphicsResourceManager().GetTextureEntry(m_EntryID).IncRefCount();
}


void CTextureHandle::DecResourceRefCount()
{
	if( 0 <= m_EntryID )
		GraphicsResourceManager().GetTextureEntry(m_EntryID).DecRefCount();
}


bool CTextureHandle::Load()
{
	Release();

	m_EntryID = GraphicsResourceManager().LoadTexture( filename );

	if( m_EntryID == -1 )
		return false;	// the loading failed - this is mostly because the texture file was not found
	else
		return true;	// the texture has been successfully loaded
}

/// \param weak pointer to an instance of CTextureLoader class that fill the texture content after the graphics device is released and recreated.
/// Owner of the texture handle is supposed to hold shared_ptr of texture loader and set it to the first argument
/// This can be set to null as boost::weak_ptr<CTextureLoader>() if you don't have to fill the content of the texture when it is re-created. e.g.) Texture for rendertarget
bool CTextureHandle::Create( boost::weak_ptr<CTextureLoader> pTextureLoader, int width, int height, TextureFormat::Format format, int mip_levels )
{
	Release();

	CTextureResourceDesc desc;
	desc.Width     = width;
	desc.Height    = height;
	desc.Format    = format;
	desc.MipLevels = mip_levels;
//	desc.Usage = ;
//	desc.Pool = ;

	desc.Filename = filename;

	desc.pLoader = pTextureLoader;

	m_EntryID = GraphicsResourceManager().CreateTexture( desc );

	if( m_EntryID == -1 )
		return false;	// the loading failed - this is mostly because the texture file was not found
	else
		return true;	// the texture has been successfully loaded
}

/*
bool CTextureHandle::Load( CImageArchive& img_archive )
{
	return false;
}
*/
