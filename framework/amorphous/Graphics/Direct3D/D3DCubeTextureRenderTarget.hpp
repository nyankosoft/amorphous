#ifndef  __D3DCubeTextureRenderTarget_HPP__
#define  __D3DCubeTextureRenderTarget_HPP__


#include "../CubeTextureRenderTarget.hpp"
#include <d3d9.h>


namespace amorphous
{


class D3DCubeTextureRenderTarget : public CubeTextureRenderTarget
{
private:

	LPDIRECT3DCUBETEXTURE9       m_apCubeMapFp[2];  ///< Floating point format cube map
	LPDIRECT3DCUBETEXTURE9       m_pCubeMap32;      ///< 32-bit cube map (for fallback)
	LPDIRECT3DSURFACE9           m_pDepthCube;      ///< Depth-stencil buffer for rendering to cube texture

	LPDIRECT3DCUBETEXTURE9       m_pCurrentCubeMap; ///< Cube map(s) to use based on current cubemap format

	/// temporarily hold the original render target
	LPDIRECT3DSURFACE9 m_pOrigRenderTarget;
	LPDIRECT3DSURFACE9 m_pOrigDepthStencilSurface;

	int m_NumCubes;

public:

	D3DCubeTextureRenderTarget();

	D3DCubeTextureRenderTarget( uint texture_size, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	D3DCubeTextureRenderTarget( const TextureResourceDesc& texture_desc );

	~D3DCubeTextureRenderTarget();

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

	LPDIRECT3DCUBETEXTURE9 GetCubeTexture() { return m_pCurrentCubeMap; }

	void LoadGraphicsResources( const GraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	static boost::shared_ptr<CubeTextureRenderTarget> Create() { boost::shared_ptr<D3DCubeTextureRenderTarget> p( new D3DCubeTextureRenderTarget ); return p; }
};


} // namespace amorphous


#endif		/*  __D3DCubeTextureRenderTarget_HPP__  */
