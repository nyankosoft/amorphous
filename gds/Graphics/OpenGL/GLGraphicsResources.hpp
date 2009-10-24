#ifndef  __GLGraphicsResources_HPP__
#define  __GLGraphicsResources_HPP__


#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"
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

	bool Lock();

	bool Unlock();

	/// Creates an empty texture from the current desc.
	/// Called by the render thread.
	bool Create();

	/// Returns true on success
	/// - Succeeds only between a pair of Lock() and Unlock() calls
	/// - Returns an object that provides access to the locked texture surface
//	bool GetLockedTexture( CLockedTexture& texture );

	friend class CGraphicsResourceManager;
};



//---------------------------- inline implementations ---------------------------- 



#endif  /* __GLGraphicsResources_HPP__ */
