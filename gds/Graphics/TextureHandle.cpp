#include "TextureHandle.hpp"
#include "GraphicsResourceManager.hpp"

#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


//==================================================================================================
// CTextureHandle
//==================================================================================================

const CTextureHandle CTextureHandle::ms_NullHandle;


bool CTextureHandle::Load( const CTextureResourceDesc& desc )
{
	Release();

	if( desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		m_pResourceEntry = GraphicsResourceManager().LoadTexture( desc );
	}
	else
	{
		m_pResourceEntry = GraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;	// the texture has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool CTextureHandle::Load( const std::string& resource_path )
{
	CTextureResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


bool CTextureHandle::LoadAsync( const std::string& resource_path )
{
	CTextureResourceDesc desc;
	desc.ResourcePath = resource_path;
	desc.LoadingMode  = CResourceLoadingMode::ASYNCHRONOUS;
	return Load( desc );
}


/*
bool CTextureHandle::LoadAsync( const CTextureResourceDesc& desc )
{

	return true;
}
*/

/// \param weak pointer to an instance of CTextureFillingAlgorithm class that fill the texture content after the graphics device is released and recreated.
/// Owner of the texture handle is supposed to hold shared_ptr of texture loader and set it to the first argument
/// This can be set to null as boost::weak_ptr<CTextureFillingAlgorithm>() if you don't have to fill the content of the texture when it is re-created. e.g.) Texture for rendertarget
bool CTextureHandle::Create( boost::shared_ptr<CTextureFillingAlgorithm> pTextureLoader,
							 const std::string& resource_name,
							 int width, int height, TextureFormat::Format format, int mip_levels )
{
	Release();

	CTextureResourceDesc desc;
	desc.Width     = width;
	desc.Height    = height;
	desc.Format    = format;
	desc.MipLevels = mip_levels;
//	desc.Usage = ;
//	desc.Pool = ;

	desc.ResourcePath = resource_name;

	desc.pLoader = pTextureLoader;

	m_pResourceEntry = GraphicsResourceManager().CreateTexture( desc );

	if( m_pResourceEntry )
		return true;	// the texture has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool CTextureHandle::SaveTextureToImageFile( const std::string& image_filepath )
{
	if( GetEntry()
	 && GetEntry()->GetTextureResource() )
	{
		return GetEntry()->GetTextureResource()->SaveTextureToImageFile( image_filepath );
	}
	else
		return false;
}



/*
bool CTextureHandle::Load( CImageArchive& img_archive )
{
	return false;
}
*/


} // namespace amorphous
