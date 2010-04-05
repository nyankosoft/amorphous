#ifndef  __GLGraphicsResources_HPP__
#define  __GLGraphicsResources_HPP__


#include "fwd.hpp"
#include "Graphics/GraphicsResources.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include <gl/gl.h>

#include "Support/2DArray.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

//template<class T>class CBinaryDatabase<T>;


class CGLTextureResource : public CTextureResource
{
	GLuint m_TextureID;

	boost::shared_ptr<CLockedTexture> m_pLockedTexture;

	boost::shared_ptr< C2DArray<SFloatRGBAColor> > m_pLockedTextureImageBuffer;

	boost::shared_ptr< C2DArray<U32> > m_pLockedTextureRGBA32ImageBuffer;

protected:

	/// Release texture without changing the reference count
	/// called only from CGraphicsResourceManager
	/// and from CTextureResource if Refresh() is used
	virtual void Release();

	/// returns false if m_TextureDesc has valid width, height, and format 
//	virtual bool IsDiskResource() const;

	/// create an empty texture
	/// - texture settings are read from m_TextureDesc
	bool CreateFromDesc();

//	void UpdateDescForCachedResource( const CGraphicsResourceDesc& desc );

	bool CreateGLTexture( GLenum target, const GLenum& src_format, const GLenum& src_type, void *pImageData );

public:

	CGLTextureResource( const CTextureResourceDesc *pDesc );

	~CGLTextureResource();

	virtual bool LoadFromFile( const std::string& filepath );

	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );

//	inline LPDIRECT3DTEXTURE9 GetTexture();

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

	/// TODO: support mip level
	bool Lock( uint mip_level );

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
//	bool GetLockedTexture( boost::shared_ptr<CLockedTexture>& pLockedTexture );

	friend class CGraphicsResourceManager;
};



class CGLShaderResource : public CShaderResource
{
protected:

	/// Release the shader manager without changing the reference count
	/// - Called only from this class and CMeshObjectManager
//	virtual void Release();

	CShaderManager *CreateShaderManager();

public:

	CGLShaderResource( const CShaderResourceDesc *pDesc );

	~CGLShaderResource();
/*
	virtual bool LoadFromFile( const std::string& filepath );
	virtual bool LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname );
//	virtual bool CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc );

	friend class CGraphicsResourceManager;*/
};



//---------------------------- inline implementations ---------------------------- 



#endif  /* __GLGraphicsResources_HPP__ */
