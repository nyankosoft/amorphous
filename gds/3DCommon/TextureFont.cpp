#include "TextureFont.h"
#include "Direct3D9.h"

#include <string.h>
#include "Support/msgbox.h"
#include "Support/Log/DefaultLog.h"

using namespace std;



void SetRenderStatesForTextureFont( AlphaBlend::Mode dest_alpha_blend )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// set render states for texture font
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, g_dwD3DBlendMode[dest_alpha_blend] );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
}



CTextureFont::CTextureFont()
{
	InitInternal();
}


CTextureFont::CTextureFont( const std::string texture_filename,
 		                    int font_width, int font_height,
                            int num_tex_divisions_x, int num_tex_divisions_y )

{
	InitInternal();
	InitFont( texture_filename, font_width, font_height, num_tex_divisions_x, num_tex_divisions_y );
}


CTextureFont::~CTextureFont()
{
	Release();
}


void CTextureFont::InitInternal()
{
	ZeroMemory(m_avTextBox, sizeof(TLVERTEX) * NUM_MAX_LETTERS * 6);
	for(int i=0; i<NUM_MAX_LETTERS * 6; i++)
	{
		m_avTextBox[i].rhw = 1.0f;
		m_avTextBox[i].color = 0xff000000;		// actually, these vertex colors are ignored
	}

	m_fItalic = 0.0f;

	m_CacheIndex = 0;
}


void CTextureFont::Release()
{
}


void CTextureFont::Reload()
{
	// When the graphics device is released and re-created,
	// textures are automatically released and reloaded by GraphicsResourceManager
//	m_FontTexture.Load();
}


bool CTextureFont::InitFont( const std::string texture_filename,
 		                     int font_width, int font_height,
                             int num_tex_divisions_x, int num_tex_divisions_y )
{
	m_FontTexture.filename = texture_filename;
	bool result = m_FontTexture.Load();
	if( !result )
	{
		LOG_PRINT_ERROR( "Failed to load a texture: " + texture_filename );
		return false;
	}

	m_NumTexDivisionsX = num_tex_divisions_x;
	m_NumTexDivisionsY = num_tex_divisions_y;
	m_strTextureFilename = texture_filename;

	SetFontSize( font_width, font_height );

	return true;
}


void CTextureFont::SetFontSize(int font_width, int font_height)
{
	m_FontWidth  = font_width;
	m_FontHeight = font_height;
}



// vPos : top left corner of the text box
// font_width : width of a letter (in screen coodinate)
// font_height : height of a letter (in screen coodinate)
void CTextureFont::DrawText( const char* pcStr, const Vector2& vPos, U32 dwColor )
{
	m_CacheIndex = 0;
	CacheText( pcStr, vPos, dwColor );

	DrawCachedText();
}


void CTextureFont::CacheText( const char* pcStr, const Vector2& vPos, U32 dwColor )
{
	if( !pcStr )
		return;

	size_t num_letters = strlen(pcStr);

	if( NUM_MAX_LETTERS <= m_CacheIndex + num_letters )
	{
//		assert( !"CTextureFont - text data overflow" );
		LOG_PRINT_WARNING( " - text data overflow" );
		return;
	}

	float font_width  = (float)GetFontWidth();
	float font_height = (float)GetFontHeight();

	float italic = font_width * m_fItalic;

	float fNumSegments_X = (float)m_NumTexDivisionsX;
	float fNumSegments_Y = (float)m_NumTexDivisionsY;
	float fLetterWidth_U  = 1.0f / fNumSegments_X;
	float fLetterHeight_V = 1.0f / fNumSegments_Y;
	int iCharCode;
	float su, sv, eu, ev;
	float sx, ex;
	int iVert = m_CacheIndex * 6;
	int col=0;
	float pos_y = vPos.y;
	for(size_t i=0; i<num_letters; i++, col++)
	{
		iCharCode = (int)pcStr[i];

		if( iCharCode == '\n' )
		{
			col = 0;
			pos_y += font_height;
			continue;
		}

		sx = vPos.x + font_width * col;
		ex = sx + font_width;

		su = (float)(iCharCode % 16) / fNumSegments_X;
		sv = (float)(iCharCode / 16) / fNumSegments_Y;
		eu = su + fLetterWidth_U  - 0.0001f;
		ev = sv + fLetterHeight_V - 0.0001f;

		m_avTextBox[iVert  ].vPosition = D3DXVECTOR3( sx + italic, pos_y, 0 );
		m_avTextBox[iVert  ].tu = su;
		m_avTextBox[iVert  ].tv = sv;

		m_avTextBox[iVert+1].vPosition = D3DXVECTOR3( ex + italic, pos_y, 0 );
		m_avTextBox[iVert+1].tu = eu;
		m_avTextBox[iVert+1].tv = sv;

		m_avTextBox[iVert+2].vPosition = D3DXVECTOR3( sx, pos_y + font_height, 0 );
		m_avTextBox[iVert+2].tu = su;
		m_avTextBox[iVert+2].tv = ev;

		m_avTextBox[iVert+3] = m_avTextBox[iVert+1];

		m_avTextBox[iVert+4].vPosition = D3DXVECTOR3( ex, pos_y + font_height, 0 );
		m_avTextBox[iVert+4].tu = eu;
		m_avTextBox[iVert+4].tv = ev;

		m_avTextBox[iVert+5] = m_avTextBox[iVert+2];

		iVert += 6;
	}

	for(int i=0; i<iVert; i++)
		m_avTextBox[i].color = dwColor;

	m_CacheIndex += (int)num_letters;
}


void CTextureFont::DrawCachedText()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// set the font texture
	hr = pd3dDev->SetTexture( 0, m_FontTexture.GetTexture() );

	if( FAILED(hr) )
		return;
/*
	// enable alpha blending
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// blend texture color and diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
*/
	pd3dDev->SetFVF( D3DFVF_TLVERTEX );

	// Draw the string
	const int num_letters = m_CacheIndex;
	pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, num_letters * 2, m_avTextBox, sizeof(TLVERTEX) );
}


void CTextureFont::SetDefaultTextureStageStates()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// blend texture color and diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}