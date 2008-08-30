
#include "LogOutput_OnScreen.h"

#include "Support/SafeDelete.h"
#include "Support/StringAux.h"
#include "3DMath/MathMisc.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/Font.h"
#include "3DCommon/2DRect.h"

using namespace std;


//=================================================================================
// CLogOutput_OnScreen
//=================================================================================

CLogOutput_OnScreen::CLogOutput_OnScreen( const string& font_name,
										  int font_width, int font_height,
                                          int num_rows, int num_chars )
:
m_pBorrowedFont(NULL),
m_NumRows(num_rows),
m_EndRowIndex(0)
{
	if( font_name.find(".dds") != string::npos
	 || font_name.find(".bmp") != string::npos
	 || font_name.find(".tga") != string::npos
	 || font_name.find(".jpg") != string::npos )
	{
		m_pFont = new CTextureFont( font_name, font_width, font_height );
	}
	else
	{
		m_pFont = new CFont( font_name, font_width, font_height );
	}

	Init(num_rows);
}


CLogOutput_OnScreen::CLogOutput_OnScreen( int num_rows, int num_chars )
:
m_pFont(NULL),
m_pBorrowedFont(NULL),
m_NumRows(num_rows),
m_EndRowIndex(0)
{
	Init(num_rows);
}


CLogOutput_OnScreen::~CLogOutput_OnScreen()
{
	SafeDelete( m_pFont );
}


void CLogOutput_OnScreen::Init( int num_rows )
{
//	m_TextBuffer.resize();
	m_TextBuffer.resize( num_rows );
	m_TextColor.resize( num_rows );

	m_NumOutputLines = 0;

	SetTopLeftPos( Vector2(5,5) );
}


void CLogOutput_OnScreen::ReleaseGraphicsResources()
{
	if( m_pFont )
		m_pFont->Release();

	m_pBorrowedFont = NULL;
}


void CLogOutput_OnScreen::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	if( m_pFont )
		m_pFont->Reload();
}


static const DWORD s_LogTextColor[5] =
{
	0xFFE8E8E8,	///< normal
	0xFFF8F808,	///< caution
	0xFFFF8F06,	///< warning
	0xFFFF0000,	///< error
	0xFFB2B2B2	///< casual
};


void CLogOutput_OnScreen::Print( const CLogMessage& msg )
{
	m_TextBuffer[m_EndRowIndex] = fmt_string( "[%s] %s", msg.m_Time.c_str(), msg.m_Text.c_str() );

	int color_index = msg.m_FilterVal & 0x000000FF;	// take out the warning level index from the lowest 8-bits
	Limit( color_index, 0, NUM_LOGWARNINGLEVELS - 1 );
	m_TextColor[m_EndRowIndex] = s_LogTextColor[color_index];

	// wrap the row index if it goes over the last row
	m_EndRowIndex++;
	if( m_NumRows <= m_EndRowIndex )
		m_EndRowIndex = 0;

	m_NumOutputLines++;
}


void CLogOutput_OnScreen::Render()
{
	CFontBase *pFont = NULL;
	if( m_pFont )
		pFont = m_pFont;
	else if( m_pBorrowedFont )
		pFont = m_pBorrowedFont;
	else
		return;

	int font_width = pFont->GetFontWidth();
	int font_height = pFont->GetFontHeight();

	// draw rect for background
	int sx = (int)m_vTopLeftPos.x;//5;
	int sy = (int)m_vTopLeftPos.y;//5;
	C2DRect bg_rect( sx, sy, sx + font_width * 72/*m_NumChars*/, sy + font_height * m_NumRows, 0x80000000 );
	bg_rect.Draw();

	D3DXVECTOR2 pos;
	pos.x = (float)( sx + font_width * 0.5f );
	int i, row_index;
	int num_rows = m_NumRows;
//	const int end_row_index = m_EndRowIndex;
	int start_row_index = (m_EndRowIndex);// % m_NumRows;
//	if( m_TextBuffer[start_row_index].length() == 0 )
	if( m_NumOutputLines < num_rows )
	{
		// not wraps yet - start rendering from the first row of the buffer
		num_rows = start_row_index;
		start_row_index = 0;
	}

	for( i=0, row_index = start_row_index;
		 i<num_rows;
		 i++, row_index = (row_index+1) % num_rows )	// wrap the row index
	{
		pos.y = (float)(sy + i * font_height);
//		pFont->CacheText( m_TextBuffer[i].c_str(), pos, m_TextColor[i] );
		pFont->DrawText( m_TextBuffer[row_index].c_str(), pos, m_TextColor[row_index] );
	}
}

/*
const char *CLogOutput_OnScreen::GetText( int row )
{
	if( row < 0 || m_NumRows <= row )
		return "";

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextBuffer[row_index];
}


U32 CLogOutput_OnScreen::GetTextColor( int row )
{
	if( row < 0 || m_NumRows <= row )
		return 0xFFFFFFFF;

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextColor[row_index];
}
*/
