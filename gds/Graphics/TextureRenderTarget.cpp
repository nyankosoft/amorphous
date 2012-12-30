#include "TextureRenderTarget.hpp"
#include "Support/Profile.hpp"


namespace amorphous
{
//#include "Support/BMPImageExporter.hpp"


boost::shared_ptr<CTextureRenderTarget> (*CTextureRenderTarget::ms_pCreateTextureRenderTarget)(void);


CTextureRenderTarget::CTextureRenderTarget()
:
m_BackgroundColor( SFloatRGBAColor( 0.449f, 0.398f, 0.738f, 1.0f ) ),
m_bScreenSizeRenderTarget( false )
{
	m_TextureDesc.UsageFlags = UsageFlag::RENDER_TARGET;
	m_TextureDesc.Sharable   = false;
	m_TextureDesc.Width      = 1;
	m_TextureDesc.Height     = 1;
	m_TextureDesc.Format     = TextureFormat::A8R8G8B8;
	m_TextureDesc.MipLevels  = 1;

//	m_BackgroundColor = 0x00000000;
}


CTextureRenderTarget::CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
:
m_BackgroundColor( SFloatRGBAColor( 0.449f, 0.398f, 0.738f, 1.0f ) ),
m_bScreenSizeRenderTarget( false )
{
	m_TextureDesc.UsageFlags = UsageFlag::RENDER_TARGET;
	m_TextureDesc.Sharable   = false;
	m_TextureDesc.Width      = texture_width;
	m_TextureDesc.Height     = texture_height;
	m_TextureDesc.Format     = texture_format;
	m_TextureDesc.MipLevels  = 1;

	LoadTextures();
}


CTextureRenderTarget::CTextureRenderTarget( const CTextureResourceDesc& texture_desc )
:
m_TextureDesc(texture_desc),
m_BackgroundColor( SFloatRGBAColor( 0.449f, 0.398f, 0.738f, 1.0f ) ),
m_bScreenSizeRenderTarget( false )
{
	LoadTextures();
}


CTextureRenderTarget::~CTextureRenderTarget()
{
	ReleaseGraphicsResources();
}


bool CTextureRenderTarget::Init( int texture_width, int texture_height, TextureFormat::Format texture_format, uint option_flags )
{
	m_TextureDesc.Width  = texture_width;
	m_TextureDesc.Height = texture_height;
	m_TextureDesc.Format = texture_format;
	m_TextureDesc.MipLevels  = 1;

	return LoadTextures();
}


bool CTextureRenderTarget::Init( const CTextureResourceDesc& texture_desc )
{
	m_TextureDesc = texture_desc;

	return LoadTextures();
}


bool CTextureRenderTarget::InitScreenSizeRenderTarget()
{
	m_bScreenSizeRenderTarget = true;

	return Init( GetScreenWidth(), GetScreenHeight(), TextureFormat::A8R8G8B8, 0 );
}


void CTextureRenderTarget::ReleaseGraphicsResources()
{
	ReleaseTextures();
}


void CTextureRenderTarget::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	if( m_bScreenSizeRenderTarget )
	{
		// resize the render target texture size for a new screen resolution
		m_TextureDesc.Width  = GetScreenWidth();
		m_TextureDesc.Height = GetScreenHeight();
	}

	LoadTextures();
}


boost::shared_ptr<CTextureRenderTarget> CTextureRenderTarget::Create()
{
	if( ms_pCreateTextureRenderTarget )
		return (*ms_pCreateTextureRenderTarget)();
	else
		return boost::shared_ptr<CTextureRenderTarget>();
}


} // namespace amorphous
