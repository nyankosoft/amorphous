#include "TextureHandle.hpp"
#include "GraphicsResourceManager.hpp"

#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


//==================================================================================================
// TextureHandle
//==================================================================================================

const TextureHandle TextureHandle::ms_NullHandle;


bool TextureHandle::Load( const TextureResourceDesc& desc )
{
	Release();

	if( desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadTexture( desc );
	}
	else
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;	// the texture has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool TextureHandle::Load( const std::string& resource_path )
{
	TextureResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


bool TextureHandle::LoadAsync( const std::string& resource_path )
{
	TextureResourceDesc desc;
	desc.ResourcePath = resource_path;
	desc.LoadingMode  = CResourceLoadingMode::ASYNCHRONOUS;
	return Load( desc );
}


/*
bool TextureHandle::LoadAsync( const TextureResourceDesc& desc )
{

	return true;
}
*/

/// \param weak pointer to an instance of TextureFillingAlgorithm class that fill the texture content after the graphics device is released and recreated.
/// Owner of the texture handle is supposed to hold shared_ptr of texture loader and set it to the first argument
/// This can be set to null as std::weak_ptr<TextureFillingAlgorithm>() if you don't have to fill the content of the texture when it is re-created. e.g.) Texture for rendertarget
bool TextureHandle::Create( std::shared_ptr<TextureFillingAlgorithm> pTextureLoader,
							 const std::string& resource_name,
							 int width, int height, TextureFormat::Format format, int mip_levels )
{
	Release();

	TextureResourceDesc desc;
	desc.Width     = width;
	desc.Height    = height;
	desc.Format    = format;
	desc.MipLevels = mip_levels;
//	desc.Usage = ;
//	desc.Pool = ;

	desc.ResourcePath = resource_name;

	desc.pLoader = pTextureLoader;

	m_pResourceEntry = GetGraphicsResourceManager().CreateTexture( desc );

	if( m_pResourceEntry )
		return true;	// the texture has been successfully loaded
	else
		return false;	// the loading failed - this is mostly because the texture file was not found
}


bool TextureHandle::SaveTextureToImageFile( const std::string& image_filepath )
{
	if( GetEntry()
	 && GetEntry()->GetTextureResource() )
	{
		return GetEntry()->GetTextureResource()->SaveTextureToImageFile( image_filepath );
	}
	else
		return false;
}


SRectangular TextureHandle::GetSize2D( unsigned int level )
{
	if( GetEntry()
	 && GetEntry()->GetTextureResource() )
	{
		return GetEntry()->GetTextureResource()->GetSize2D( level );
	}
	else
		return SRectangular(0,0);
}


Result::Name TextureHandle::SetSamplingParameter( SamplingParameter::Name param, uint value )
{
	if( GetEntry()
	 && GetEntry()->GetTextureResource() )
	{
		return GetEntry()->GetTextureResource()->SetSamplingParameter( param, value );
	}
	else
		return Result::UNKNOWN_ERROR;
}


/*
bool TextureHandle::Load( ImageArchive& img_archive )
{
	return false;
}
*/


} // namespace amorphous
