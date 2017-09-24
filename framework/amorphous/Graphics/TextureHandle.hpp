#ifndef  __TextureHandle_H__
#define  __TextureHandle_H__


#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"
#include "GraphicsResourceHandle.hpp"


namespace amorphous
{


class TextureHandle : public GraphicsResourceHandle
{
protected:

	static const TextureHandle ms_NullHandle;

public:

	inline TextureHandle() {}

	~TextureHandle() { Release(); }

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	bool Load( const std::string& resource_path );

	bool LoadAsync( const std::string& resource_path );

	bool Load( const TextureResourceDesc& desc );

//	virtual bool LoadAsync( const TextureResourceDesc& desc );

	/// Direct access to D3D texture
	/// - Avoid using this whenever possible
	inline const LPDIRECT3DTEXTURE9 GetTexture() const
	{
		if( GetEntry()
		 && GetEntry()->GetTextureResource() )
		{
			return GetEntry()->GetTextureResource()->GetTexture();
		}
		else
			return NULL;
	}

	Result::Name AcceptTextureResourceVisitor( TextureResourceVisitor& visitor ) const
	{
		if( GetEntry()
		 && GetEntry()->GetTextureResource() )
		{
			return GetEntry()->GetTextureResource()->Accept( visitor );
		}
		else
		{
			return Result::UNKNOWN_ERROR;
		}
	}

	/// Creates an empty texture
	/// - Created as a shareable resource (Right now resource are always sharable)
	/// - GraphicsResourceHandle::filename is used as an id
	///   - The resources are shared if GraphicsResourceHandle::filenames are the same
	///     just like texture resources loaded from file
	/// \param mip_levels number of mip levels. set 0 to create a complete mipmap chain (0 by default).
	bool Create( std::shared_ptr<TextureFillingAlgorithm> pTextureLoader, const std::string& resource_name, int width, int height, TextureFormat::Format format, int mip_levels = 0 );

	bool SaveTextureToImageFile( const std::string& image_filepath );

	SRectangular GetSize2D( unsigned int level = 0 );

	Result::Name SetSamplingParameter( SamplingParameter::Name param, uint value );

	/// loads texture from memory in the form of image archive
//	bool Load( ImageArchive& img_archive );

	static const TextureHandle& Null() { return ms_NullHandle; }

//	inline const TextureHandle &operator=( const TextureHandle& handle );
};


//--------------------------------- inline implementations ---------------------------------

/*
inline const TextureHandle &TextureHandle::operator=( const TextureHandle& handle ){}
*/

} // namespace amorphous



#endif  /* __TextureHandle_H__ */
