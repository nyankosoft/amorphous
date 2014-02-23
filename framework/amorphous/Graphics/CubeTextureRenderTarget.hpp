#ifndef  __CubeTextureRenderTarget_HPP__
#define  __CubeTextureRenderTarget_HPP__


#include "../base.hpp"
#include "GraphicsComponentCollector.hpp"
#include "SurfaceFormat.hpp"


namespace amorphous
{


class CubeTextureRenderTarget
{
protected:

	int m_CubeTextureSize;

	TextureFormat::Format m_TextureFormat;

public:

	CubeTextureRenderTarget() : m_CubeTextureSize(256), m_TextureFormat(TextureFormat::A8R8G8B8) {}
	virtual ~CubeTextureRenderTarget(){}

	inline int GetCubeTextureSize() const { return m_CubeTextureSize; }

	/// \param texture_size The length of the texture edges in texels, usually 256, 512, 1024, ... The texture is always square
	inline void SetCubeTextureSize( const int texture_size ) { m_CubeTextureSize = texture_size; }

	virtual bool CreateTextures( unsigned int texture_size, TextureFormat::Format texture_format ) { return false; }

	virtual bool IsReady() = 0;

	virtual void Begin() {}
	virtual void SetRenderTarget( int face_index ) {}
	virtual void End() {}

	static boost::shared_ptr<CubeTextureRenderTarget> (*ms_pCreateCubeTextureRenderTarget)(void);

	static void SetInstanceCreationFunction( boost::shared_ptr<CubeTextureRenderTarget> (*CreateTextureRenderTarget)(void) ) { ms_pCreateCubeTextureRenderTarget = CreateTextureRenderTarget; }

	static boost::shared_ptr<CubeTextureRenderTarget> Create();
};


} // namespace amorphous


#endif		/*  __CubeTextureRenderTarget_HPP__  */
