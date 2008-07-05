#ifndef  __TextureHandle_H__
#define  __TextureHandle_H__


#include "GraphicsResource.h"
#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"
#include <boost/weak_ptr.hpp>
#include <d3dx9tex.h>


class CTextureLoader;


class CTextureHandle : public CGraphicsResourceHandle
{
protected:

	static const CTextureHandle ms_NullHandle;

	virtual void IncResourceRefCount();
	virtual void DecResourceRefCount();

public:

//	boost::weak_ptr<CTextureLoader> m_pTextureLoader;

	inline CTextureHandle() {}

	~CTextureHandle() { Release(); }

	inline void Release();

	GraphicsResourceType::Name GetResourceType() const { return GraphicsResourceType::Texture; }

	virtual bool Load();

	inline const LPDIRECT3DTEXTURE9 GetTexture() const { return GraphicsResourceManager().GetTexture(m_EntryID); }

	/// creates an empty texture
	/// - Created as a shareable resource (Right now resource are always sharable)
	/// - CGraphicsResourceHandle::filename is used as an id
	///   - The resource is shared if CGraphicsResourceHandle::filenames are the same
	///     just like texture resources loaded from file
	/// \param mip_levels number of mip levels. set 0 to create a complete mipmap chain (0 by default).
	bool Create( boost::weak_ptr<CTextureLoader> pTextureLoader, int width, int height, TextureFormat::Format format, int mip_levels = 0 );

	/// loads texture from memory in the form of image archive
//	bool Load( CImageArchive& img_archive );

	static const CTextureHandle& Null() { return ms_NullHandle; }

//	inline const CTextureHandle &operator=( const CTextureHandle& handle );
};


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
