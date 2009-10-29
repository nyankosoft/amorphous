#include "TextureFont.hpp"
#include "Graphics/Direct3D9.hpp"

#include <string.h>
#include "Support/Log/DefaultLog.hpp"

using namespace std;


const std::string CTextureFont::ms_Characters = " !\"#$%&'()*+,-./0123456789:;<=>?`ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~";


void SetRenderStatesForTextureFont( AlphaBlend::Mode dest_alpha_blend )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	if( !pd3dDev )
		return;

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
/*	ZeroMemory(m_avTextBox, sizeof(TLVERTEX) * NUM_MAX_LETTERS * 6);
	for(int i=0; i<NUM_MAX_LETTERS * 6; i++)
	{
		m_avTextBox[i].rhw = 1.0f;
		m_avTextBox[i].color = 0xff000000;		// actually, these vertex colors are ignored
	}
*/
	m_fItalic = 0.0f;

	m_vShadowShift = Vector2(3,3);

	m_CacheIndex = 0;

	SetShadowColor( SFloatRGBAColor::Black() );
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
//	m_FontTextureFilepath = texture_filename;
	bool result = m_FontTexture.Load( texture_filename );
	if( !result )
	{
		LOG_PRINT_ERROR( "Failed to load a texture: " + texture_filename );
		return false;
	}

	m_NumTexDivisionsX = num_tex_divisions_x;
	m_NumTexDivisionsY = num_tex_divisions_y;
	m_strTextureFilename = texture_filename;

	SetFontSize( font_width, font_height );

//	base_char_height = 64;

	float su, sv, eu, ev;
	float fNumSegments_X = (float)m_NumTexDivisionsX;
	float fNumSegments_Y = (float)m_NumTexDivisionsY;
	float fLetterWidth_U  = 1.0f / fNumSegments_X;
	float fLetterHeight_V = 1.0f / fNumSegments_Y;

	// calc local positions and texture coords for fixed pitch font
	int i, num_chars = (int)ms_Characters.size();
	m_vecCharRect.resize( num_chars );
	for( i=0; i<num_chars; i++ )
	{
		int charcode = i + ' ';

		m_vecCharRect[i].rect.vMin = Vector2(0,0);
		m_vecCharRect[i].rect.vMax = Vector2(1,1);

		su = (float)(charcode % 16) / fNumSegments_X;
		sv = (float)(charcode / 16) / fNumSegments_Y;
		eu = su + fLetterWidth_U  - 0.0001f;
		ev = sv + fLetterHeight_V - 0.0001f;

		m_vecCharRect[i].tex_min = TEXCOORD2( su, sv );
		m_vecCharRect[i].tex_max = TEXCOORD2( eu, ev );

		m_vecCharRect[i].advance = 1.0f;
	}

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


int CTextureFont::GetTextWidth( const char *text ) const
{
	if( !text )
		return 0;

	float factor_x = GetHorizontalFactor();
	float text_width = 0;
	float max_text_width = 0;
	int char_index = 0;
	const int max_char_index = (int)m_vecCharRect.size();

	const size_t num_letters = strlen(text);
	for(size_t i=0; i<num_letters; i++)
	{
		if( text[i] == '\n' )
		{
			if( max_text_width < text_width )
				max_text_width = text_width;

			text_width = 0;

			continue;
		}

		int char_code = (int)text[i];
		char_index = char_code - ' ';
		if( char_index < 0 || max_char_index <= char_index )
			continue;

		const CharRect& char_rect = m_vecCharRect[char_index];

		text_width += char_rect.advance * factor_x;
	}

	if( max_text_width < text_width )
		max_text_width = text_width;

	return (int)max_text_width;
}


void CTextureFont::CacheText( const char* pcStr, const Vector2& vPos, U32 dwColor )
{
	if( m_vecCharRect.size() == 0 )
		return;

	if( !pcStr )
		return;

	const int num_letters = strlen(pcStr);

	if( NUM_MAX_LETTERS <= m_CacheIndex + num_letters )
	{
//		assert( !"CTextureFont - text data overflow" );
		LOG_PRINT_WARNING( " - text data overflow" );
		return;
	}

	float font_width  = (float)GetFontWidth();
	float font_height = (float)GetFontHeight();

	float italic = font_width * m_fItalic;
	
	Vector2 factor;
	factor.x = GetHorizontalFactor();
	factor.y = GetVerticalFactor();

	const int max_char_index = (int)m_vecCharRect.size();
	int iCharCode;
	float su, sv, eu, ev;
	float sx, sy, ex, ey;
	int iVert = m_CacheIndex * 6;
	int col=0;
	int char_index = 0;
	float current_y = vPos.y;
	float current_x = vPos.x;
	int rect_index = m_CacheIndex;
	int num_total_letters = m_CacheIndex + num_letters;

	if( m_TextBox.GetNumRects() < num_total_letters )
		m_TextBox.AddRects( num_total_letters - m_TextBox.GetNumRects() );

	for(int i=0; i<num_letters; i++, col++, rect_index++)
	{
		iCharCode = (int)pcStr[i];

		if( iCharCode == '\n' )
		{
			// line feed
			col = 0;
			current_x = vPos.x;
			current_y += font_height * factor.y;
			continue;
		}

		char_index = iCharCode-' ';
		if( char_index < 0 || max_char_index <= char_index )
			continue;

		const CharRect& char_rect = m_vecCharRect[char_index];

		su = char_rect.tex_min.u;
		sv = char_rect.tex_min.v;
		eu = char_rect.tex_max.u;
		ev = char_rect.tex_max.v;

//		sx = vPos.x + font_width * col;
//		ex = sx + font_width;
		sx = current_x + char_rect.rect.vMin.x * factor.x;
		ex = current_x + char_rect.rect.vMax.x * factor.x;
		sy = current_y + char_rect.rect.vMin.y * factor.y;
		ey = current_y + char_rect.rect.vMax.y * factor.y;

/*
		m_avTextBox[iVert  ].vPosition = D3DXVECTOR3( sx + italic, sy, 0 );
		m_avTextBox[iVert  ].SetTextureCoord( 0, 0, TEXCOORD2( su, sv ) );
		m_avTextBox[iVert  ].tv = sv;

		m_avTextBox[iVert+1].vPosition = D3DXVECTOR3( ex + italic, sy, 0 );
		m_avTextBox[iVert  ].SetTextureCoord( 1, 0, TEXCOORD2( eu, sv ) );

		m_avTextBox[iVert+2].vPosition = D3DXVECTOR3( sx, ey, 0 );
		m_avTextBox[iVert  ].SetTextureCoord( 1, 0, TEXCOORD2( su, ev ) );
		m_avTextBox[iVert+2].tu = su;
		m_avTextBox[iVert+2].tv = ev;

		m_avTextBox[iVert+3] = m_avTextBox[iVert+1];

		m_avTextBox[iVert+4].vPosition = D3DXVECTOR3( ex, ey, 0 );
		m_avTextBox[iVert+4].tu = eu;
		m_avTextBox[iVert+4].tv = ev;

		m_avTextBox[iVert+5] = m_avTextBox[iVert+2];
*/
		int vert_index = rect_index * 4;

		// positions
		m_TextBox.SetRectVertexPosition( rect_index, 0, Vector2( sx + italic, sy ) );
		m_TextBox.SetRectVertexPosition( rect_index, 1, Vector2( ex + italic, sy ) );
		m_TextBox.SetRectVertexPosition( rect_index, 2, Vector2( ex,          ey ) );
		m_TextBox.SetRectVertexPosition( rect_index, 3, Vector2( sx,          ey ) );

		m_TextBox.SetTextureCoordMinMax( rect_index, su, sv, eu, ev );

		current_x += char_rect.advance * factor.x;

		iVert += 6;
	}

//	m_vShadowShift = Vector2(3,3);
	if( m_TypeFlag & CFontBase::SHADOW )
	{
		if( m_ShadowTextBox.GetNumRects() < num_total_letters )
		{
			m_ShadowTextBox.AddRects( num_total_letters - m_ShadowTextBox.GetNumRects() );
			m_ShadowTextBox.SetColor( m_ShadowColor );
		}

		rect_index = m_CacheIndex;
		const Vector2 vShift = m_vShadowShift;
		for(int i=0; i<num_letters; i++, rect_index++)
		{
			for( int j=0; j<4; j++ )
			{
				m_ShadowTextBox.SetRectVertexPosition( rect_index, j,
					m_TextBox.GetRectVertexPosition(rect_index,j) + Vector2(vShift.x,vShift.y) );
			}

			m_ShadowTextBox.SetTextureCoordMinMax( rect_index,
				m_TextBox.GetTopLeftTextureCoord(rect_index),
				m_TextBox.GetBottomRightTextureCoord(rect_index)
				);
		}
	}

//	for(int i=0; i<iVert; i++)
//		m_avTextBox[i].color = dwColor;

	m_TextBox.SetColor( dwColor );

	m_CacheIndex += (int)num_letters;
}


void CTextureFont::DrawCachedText()
{
/*	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	// set the font texture
	hr = pd3dDev->SetTexture( 0, m_FontTexture.GetTexture() );

	if( FAILED(hr) )
		return;

	// enable alpha blending
    //pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    //pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    //pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// blend texture color and diffuse color
	//pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	//pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	//pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	//pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    //pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    //pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    //pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    //pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDev->SetFVF( D3DFVF_TLVERTEX );
*/
	// Draw the string
	const int num_letters = m_CacheIndex;
//	pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, num_letters * 2, m_avTextBox, sizeof(TLVERTEX) );

	m_TextBox.SetDestAlphaBlendMode( m_DestAlphaBlend );

	if( m_TypeFlag & CFontBase::SHADOW )
		m_ShadowTextBox.Draw( 0, num_letters, m_FontTexture );

	m_TextBox.Draw( 0, num_letters, m_FontTexture );
}


void CTextureFont::SetDefaultTextureStageStates()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	if( !pd3dDev )
		return;

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
