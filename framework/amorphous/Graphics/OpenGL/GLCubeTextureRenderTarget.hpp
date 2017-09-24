#ifndef  __GLCubeTextureRenderTarget_HPP__
#define  __GLCubeTextureRenderTarget_HPP__


#include "../CubeTextureRenderTarget.hpp"


namespace amorphous
{


class GLCubeTextureRenderTarget : public CubeTextureRenderTarget
{
public:

	GLCubeTextureRenderTarget();

	GLCubeTextureRenderTarget( uint texture_size, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	GLCubeTextureRenderTarget( const TextureResourceDesc& texture_desc );

	~GLCubeTextureRenderTarget();

//	bool Init( const TextureResourceDesc& texture_desc );

	bool IsReady();

	void SaveOriginalRenderTarget();

	void Begin();

	bool CreateTextures( unsigned int texture_size, TextureFormat::Format texture_format );

	/// \param face_index [0,5]
	void SetRenderTarget( unsigned int face_index );

	void End();

	void LoadOriginalRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_pRenderTargetSurface, m_pRenderTargetCopySurface ); }

//	TextureHandle GetRenderTargetCopytexture() { return m_RenderTargetCopyTexture; }

	void ReleaseTextures();

//	void OutputImageFile( const std::string& image_file_path );

	void LoadGraphicsResources( const GraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	static std::shared_ptr<CubeTextureRenderTarget> Create() { std::shared_ptr<GLCubeTextureRenderTarget> p( new GLCubeTextureRenderTarget ); return p; }
};


} // namespace amorphous


#endif		/*  __GLCubeTextureRenderTarget_HPP__  */
