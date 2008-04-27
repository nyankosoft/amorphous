#ifndef  __TextureHandle_H__
#define  __TextureHandle_H__


#include "GraphicsResourceHandle.h"
#include "GraphicsResourceManager.h"

#include <d3dx9tex.h>


class CImageArchive;


class CTextureHandle : public CGraphicsResourceHandle
{
protected:

	static const CTextureHandle ms_NullHandle;

public:

	inline CTextureHandle() {}

	~CTextureHandle() {}

	virtual bool Load();

	inline const LPDIRECT3DTEXTURE9 GetTexture() const { return GraphicsResourceManager.GetTexture(m_EntryID); }

	// load texture from memory in the form of image archive
	bool Load( CImageArchive& img_archive );

	static const CTextureHandle& Null() { return ms_NullHandle; }

//	inline const CTextureHandle &operator=( const CTextureHandle& handle );
};


/*
inline const CTextureHandle &CTextureHandle::operator=( const CTextureHandle& handle ){}
*/



#endif  /* __TextureHandle_H__ */
