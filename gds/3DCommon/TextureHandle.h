#ifndef  __TextureHandle_H__
#define  __TextureHandle_H__


#include "fwd.h"
#include "GraphicsResource.h"
#include "GraphicsResourceDescs.h"
#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"
#include <boost/weak_ptr.hpp>
#include <d3dx9tex.h>


class CTextureHandle : public CGraphicsResourceHandle
{
protected:

	static const CTextureHandle ms_NullHandle;

	virtual void IncResourceRefCount();
	virtual void DecResourceRefCount();

public:

	inline CTextureHandle() {}

	~CTextureHandle() { Release(); }

	inline void Release();

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	bool Load( const std::string& resource_path );

	bool Load( const CTextureResourceDesc& desc );

//	virtual bool LoadAsync( const CTextureResourceDesc& desc );

	/// Direct access to D3D texture
	/// - Avoid using this whenever possible
	inline const LPDIRECT3DTEXTURE9 GetTexture() const { return GraphicsResourceManager().GetTexture(m_EntryID); }

	/// Creates an empty texture
	/// - Created as a shareable resource (Right now resource are always sharable)
	/// - CGraphicsResourceHandle::filename is used as an id
	///   - The resources are shared if CGraphicsResourceHandle::filenames are the same
	///     just like texture resources loaded from file
	/// \param mip_levels number of mip levels. set 0 to create a complete mipmap chain (0 by default).
	bool Create( boost::weak_ptr<CTextureLoader> pTextureLoader, const std::string& resource_name, int width, int height, TextureFormat::Format format, int mip_levels = 0 );

	bool SaveTextureToImageFile( const std::string& image_filepath );

	/// loads texture from memory in the form of image archive
//	bool Load( CImageArchive& img_archive );

	static const CTextureHandle& Null() { return ms_NullHandle; }

//	inline const CTextureHandle &operator=( const CTextureHandle& handle );
};


//--------------------------------- inline implementations ---------------------------------

/*
inline const CTextureHandle &CTextureHandle::operator=( const CTextureHandle& handle ){}
*/


inline void CTextureHandle::Release()
{
	if( 0 <= m_EntryID )
	{
		DecResourceRefCount();
		m_EntryID = -1;
	}
}



#endif  /* __TextureHandle_H__ */
