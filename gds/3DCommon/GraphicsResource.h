#ifndef  __GraphicsResource_H__
#define  __GraphicsResource_H__


#include <memory.h>
#include <string>
#include "3DCommon/TextureFormat.h"
#include "3DCommon/FloatRGBAColor.h"


class CLockedTexture;
class CTextureEntry;
class CGraphicsResourceHandle;
class CGraphicsResourceManager;


class GraphicsResourceType
{
public:
	enum Name
	{
		Texture,
		Mesh,
		Shader,
		Font,
		NumTypes
	};
};


class GraphicsResourceFlag
{
public:
	enum Flags
	{
		DontShare = ( 1 << 0 ),
		Flag1     = ( 1 << 1 ),
		Flag2     = ( 1 << 2 ),
	};
};


/// Used to fill the texture content when
/// - A texture resource is created.
/// - A texture resource is released and recreated after the graphics device is lost
class CTextureLoader
{
public:

	virtual ~CTextureLoader() {}

	/// called by the system after the texture resource is created
	virtual void FillTexture( CLockedTexture& texture ) = 0;
};


class CLockedTexture
{
	void *m_pBits;

	int m_Width;

	int m_Height;

public:

	CLockedTexture() : m_pBits(NULL) {}

//	int GetWidth();

	virtual void SetPixelARGB32( int x, int y, U32 argb_color ) { ((U32 *)m_pBits)[ y * m_Width + x ] = argb_color; }

	/// \param alpha [0,255]
	inline void SetAlpha( int x, int y, U8 alpha )
	{
		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	virtual void Clear( U32 argb_color )
	{
		const int num_bytes_per_pixel = sizeof(U32);
		if( argb_color == 0 )
			memset( m_pBits, 0, m_Width * m_Height * num_bytes_per_pixel );
		else if( argb_color == 0xFFFFFFFF )
			memset( m_pBits, 0xFF, m_Width * m_Height * num_bytes_per_pixel );
		else
		{
			int w = m_Width;
			int h = m_Height;
			for( int y=0; y<h; y++ )
			{
				for( int x=0; x<w; x++ )
				{
					SetPixelARGB32( x, y, argb_color );
				}
			}
		}
	}

	virtual void Clear( const SFloatRGBAColor& color ) { Clear( color.GetARGB32() ); }

	friend class CTextureEntry;
};


/**
 request to load a resource
*/
class ResourceLoadRequest
{
public:

	std::string name;
	GraphicsResourceType::Name type;
};


#endif /* __GraphicsResource_H__ */