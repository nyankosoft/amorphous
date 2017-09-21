#include "TextureRenderTarget.hpp"
#include "amorphous/Support/Profile.hpp"


namespace amorphous
{

boost::shared_ptr<TextureRenderTarget> (*TextureRenderTarget::ms_pCreateTextureRenderTarget)(void);


TextureRenderTarget::TextureRenderTarget()
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


TextureRenderTarget::TextureRenderTarget( const TextureResourceDesc& texture_desc )
:
m_TextureDesc(texture_desc),
m_OptionFlags(0),
m_BackgroundColor( SFloatRGBAColor( 0.449f, 0.398f, 0.738f, 1.0f ) ),
m_bScreenSizeRenderTarget( false )
{
	LoadTextures();
}


TextureRenderTarget::~TextureRenderTarget()
{
	ReleaseGraphicsResources();
}


bool TextureRenderTarget::Init( int texture_width, int texture_height, TextureFormat::Format texture_format, U32 option_flags )
{
	m_TextureDesc.Width  = texture_width;
	m_TextureDesc.Height = texture_height;
	m_TextureDesc.Format = texture_format;
	m_TextureDesc.MipLevels  = 1;

	m_OptionFlags = option_flags;

	return LoadTextures();
}


bool TextureRenderTarget::Init( const TextureResourceDesc& texture_desc, U32 option_flags )
{
	m_TextureDesc = texture_desc;

	m_OptionFlags = option_flags;

	return LoadTextures();
}


bool TextureRenderTarget::InitScreenSizeRenderTarget()
{
	m_bScreenSizeRenderTarget = true;

	return Init( GetScreenWidth(), GetScreenHeight(), TextureFormat::A8R8G8B8, 0 );
}


void TextureRenderTarget::ReleaseGraphicsResources()
{
	ReleaseTextures();
}


void TextureRenderTarget::LoadGraphicsResources( const GraphicsParameters& rParam )
{
	if( m_bScreenSizeRenderTarget )
	{
		// resize the render target texture size for a new screen resolution
		m_TextureDesc.Width  = GetScreenWidth();
		m_TextureDesc.Height = GetScreenHeight();
	}

	LoadTextures();
}


boost::shared_ptr<TextureRenderTarget> TextureRenderTarget::Create()
{
	if( ms_pCreateTextureRenderTarget )
		return (*ms_pCreateTextureRenderTarget)();
	else
		return boost::shared_ptr<TextureRenderTarget>();
}


} // namespace amorphous
