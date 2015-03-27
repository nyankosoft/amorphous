#ifndef  __D3DTextureRenderTarget_HPP__
#define  __D3DTextureRenderTarget_HPP__


#include "../TextureRenderTarget.hpp"
#include <d3d9.h>


namespace amorphous
{


class CD3DTextureRenderTarget : public TextureRenderTarget
{
private:

//	LPDIRECT3DTEXTURE9 m_pRenderTargetTexture;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface;
	LPDIRECT3DSURFACE9 m_pRenderTargetDepthSurface;

	/// buffer to hold the content of render target surface
	/// need to access the scene image because the texture created
	/// as render target is not lockable
//	LPDIRECT3DTEXTURE9 m_pRenderTargetCopyTexture;
	LPDIRECT3DSURFACE9 m_pRenderTargetCopySurface;

	/// used to temporarily hold original surfaces
	LPDIRECT3DSURFACE9 m_pOriginalSurface;
	LPDIRECT3DSURFACE9 m_pOriginalDepthSurface;

	D3DVIEWPORT9 m_OriginalViewport;

	TextureHandle m_RenderTargetCopyTexture;

//	TextureResourceDesc m_TextureDesc;
	
public:

	CD3DTextureRenderTarget();

	CD3DTextureRenderTarget( const TextureResourceDesc& texture_desc );

	~CD3DTextureRenderTarget();

//	bool Init( const TextureResourceDesc& texture_desc );

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_pRenderTargetSurface, m_pRenderTargetCopySurface ); }

//	inline LPDIRECT3DTEXTURE9 GetD3DRenderTargetTexture() { return m_pRenderTargetTexture; }

	/// returns the lockable texture of the scene
//	inline LPDIRECT3DTEXTURE9 GetD3DRenderTargetCopyTexture() { return m_pRenderTargetCopyTexture; }

	TextureHandle GetRenderTargetCopytexture() { return m_RenderTargetCopyTexture; }

	bool LoadTextures();

	void ReleaseTextures();

	static boost::shared_ptr<TextureRenderTarget> Create() { boost::shared_ptr<CD3DTextureRenderTarget> p( new CD3DTextureRenderTarget ); return p; }
};


} // namespace amorphous



#endif		/*  __D3DTextureRenderTarget_HPP__  */
