#include "LogOutput_OnScreen.hpp"

#include "Support/SafeDelete.hpp"
#include "Support/StringAux.hpp"
#include "3DMath/MathMisc.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/Font.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"

using namespace std;


//=================================================================================
// CLogOutput_ScrolledTextBuffer
//=================================================================================

CLogOutput_ScrolledTextBuffer::CLogOutput_ScrolledTextBuffer( const string& font_name,
										  int font_width, int font_height,
                                          int num_rows, int num_chars )
:
m_NumRows(num_rows),
m_EndRowIndex(0),
m_NumOutputLines(0)
{

	Init(num_rows);
}


CLogOutput_ScrolledTextBuffer::CLogOutput_ScrolledTextBuffer( int num_rows, int num_chars )
:
m_NumRows(num_rows),
m_EndRowIndex(0),
m_NumOutputLines(0)
{
	Init(num_rows);
}


CLogOutput_ScrolledTextBuffer::~CLogOutput_ScrolledTextBuffer()
{
}


void CLogOutput_ScrolledTextBuffer::Init( int num_rows )
{
	m_TextBuffer.resize( num_rows );
	m_TextColor.resize( num_rows );

	m_NumOutputLines = 0;
}


static const DWORD s_LogTextColor[5] =
{
	0xFFE8E8E8,	///< normal
	0xFFF8F808,	///< caution
	0xFFFF8F06,	///< warning
	0xFFFF0000,	///< error
	0xFFB2B2B2	///< casual
};


void CLogOutput_ScrolledTextBuffer::Print( const CLogMessage& msg )
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


int CLogOutput_ScrolledTextBuffer::GetNumCurrentRows()
{
	if( m_NumOutputLines < m_NumRows )
		return m_NumOutputLines;
	else
		return m_NumRows;
}
