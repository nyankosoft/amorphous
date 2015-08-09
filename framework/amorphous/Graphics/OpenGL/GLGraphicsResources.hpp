#ifndef  __GLGraphicsResources_HPP__
#define  __GLGraphicsResources_HPP__


#include "amorphous/Graphics/GraphicsResources.hpp"
#include "amorphous/Graphics/TextureResourceVisitor.hpp"
#include "amorphous/Graphics/OpenGL/GLHeaders.h"
#include "amorphous/Support/fwd.hpp"


namespace amorphous
{
using namespace serialization;

//template<class T>class CBinaryDatabase<T>;


class GLTextureResourceBase : public TextureResource
{
protected:

	GLenum m_SourceFormat;
	GLenum m_SourceType;

	uint m_NumMipmaps;

public:

	GLTextureResourceBase( const TextureResourceDesc *pDesc );

	virtual ~GLTextureResourceBase() {}

	/// Sets the the image data to the texture
	/// \param [in] pImageData A source image data
	/// \param [out] texture_id A destination texture
	bool UpdateGLTextureImage( GLenum target, int level, int width, int height, const GLenum& src_format, const GLenum& src_type, void *pImageData, GLuint texture_id );

	/// \param [in] pImageData A source image data
	/// \param [out] texture_id A destination texture
	bool CreateGLTextureFromBitmapImage( GLenum target, BitmapImage& src_img, GLuint& texture_id );
};


class CGLTextureResource : public GLTextureResourceBase
{
	GLuint m_TextureID;

//	boost::shared_ptr<LockedTexture> m_pLockedTexture;

//	boost::shared_ptr< array2d<SFloatRGBAColor> > m_pLockedTextureImageBuffer;

//	boost::shared_ptr< array2d<U32> > m_pLockedTextureRGBA32ImageBuffer;

	boost::shared_ptr<BitmapImage> m_pLockedImage;

protected:

	/// Release texture without changing the reference count
	/// called only from GraphicsResourceManager
	/// and from TextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
//	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	bool CreateFromDesc();

//	void UpdateDescForCachedResource( const GraphicsResourceDesc& desc );

	bool CreateGLTexture( GLenum target, const GLenum& src_format, const GLenum& src_type, void *pImageData );

public:

	CGLTextureResource( const TextureResourceDesc *pDesc );

	~CGLTextureResource();

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	GLuint GetGLTextureID() const { return m_TextureID; }

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - Use this if the texture is loaded from file and filename is stored in GetDesc().ResourcePath
	///   - Wait! It will overwrite the original image file!
//	bool SaveTextureToImageFile(); commented out

	/// Save the texture to disk as an image file
	/// - For debugging
	/// - NOTE: image_filepath should be different from GetDesc().ResourcePath,
	///         or it will overwrite the original image file
	bool SaveTextureToImageFile( const std::string& image_filepath );

	SDim2 GetSize2D( unsigned int level );

	Result::Name SetSamplingParameter( SamplingParameter::Name param, uint value );

	/// TODO: support mip level
	bool Lock( uint mip_level );

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

	Result::Name Accept( TextureResourceVisitor& visitor ) { return visitor.Visit( *this ); }

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
//	bool GetLockedTexture( boost::shared_ptr<LockedTexture>& pLockedTexture );

	friend class GraphicsResourceManager;
};


class CGLCubeTextureResource : public GLTextureResourceBase
{
public:

	enum Params
	{
		NUM_CUBE_MAP_FACES = 6,
	};

private:

	GLuint m_TextureIDs[NUM_CUBE_MAP_FACES];

	bool CreateFromDesc() { return false; }

public:

//	CGLCubeTextureResource( const TextureResourceDesc *pDesc );
	CGLCubeTextureResource( const TextureResourceDesc *pDesc )
	:
	GLTextureResourceBase(pDesc)
	{
		for( int i=0; i<numof(m_TextureIDs); i++ )
			m_TextureIDs[i] = 0;
	}

	~CGLCubeTextureResource(){}

	bool LoadFromFile( const std::string& filepath );

	bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

	bool Create();

	GLuint GetCubeMapTexture( uint cube_map_face_index ) const { return m_TextureIDs[cube_map_face_index]; }

	Result::Name Accept( TextureResourceVisitor& visitor ) { return visitor.Visit( *this ); }
};


class CGLShaderResource : public ShaderResource
{
protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
//	virtual void Release();

	ShaderManager *CreateShaderManager();

	ShaderManager *CreateFixedFunctionPipelineManager();

public:

	CGLShaderResource( const ShaderResourceDesc *pDesc );

	~CGLShaderResource();
/*
	virtual bool LoadFromFile( const std::string& filepath );
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );
//	virtual bool CanBeSharedAsSameResource( const GraphicsResourceDesc& desc );

	friend class GraphicsResourceManager;*/
};



//---------------------------- inline implementations ---------------------------- 


} // namespace amorphous



#endif  /* __GLGraphicsResources_HPP__ */
