#include "LogOutput_OnScreen.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{

using namespace std;


//=================================================================================
// LogOutput_ScrolledTextBuffer
//=================================================================================

LogOutput_ScrolledTextBuffer::LogOutput_ScrolledTextBuffer( const string& font_name,
										  int font_width, int font_height,
                                          int num_rows, int num_chars )
:
m_NumRows(num_rows),
m_EndRowIndex(0),
m_NumOutputLines(0)
{

	Init(num_rows);
}


LogOutput_ScrolledTextBuffer::LogOutput_ScrolledTextBuffer( int num_rows, int num_chars )
:
m_NumRows(num_rows),
m_EndRowIndex(0),
m_NumOutputLines(0)
{
	Init(num_rows);
}


LogOutput_ScrolledTextBuffer::~LogOutput_ScrolledTextBuffer()
{
}


void LogOutput_ScrolledTextBuffer::Init( int num_rows )
{
	m_TextBuffer.resize( num_rows );
	m_TextColor.resize( num_rows );

	m_NumOutputLines = 0;
}


static const U32 s_LogTextColor[5] =
{
	0xFFE8E8E8,	///< normal
	0xFFF8F808,	///< caution
	0xFFFF8F06,	///< warning
	0xFFFF0000,	///< error
	0xFFB2B2B2	///< casual
};


void LogOutput_ScrolledTextBuffer::Print( const LogMessage& msg )
{
	m_TextBuffer[m_EndRowIndex] = fmt_string( "[%s] %s", msg.m_Time.c_str(), msg.m_Text.c_str() );

	int color_index = msg.m_FilterVal & 0x000000FF;	// take out the warning level index from the lowest 8-bits
	clamp( color_index, 0, NUM_LOGWARNINGLEVELS - 1 );
	m_TextColor[m_EndRowIndex] = s_LogTextColor[color_index];

	// wrap the row index if it goes over the last row
	m_EndRowIndex++;
	if( m_NumRows <= m_EndRowIndex )
		m_EndRowIndex = 0;

	m_NumOutputLines++;
}


int LogOutput_ScrolledTextBuffer::GetNumCurrentRows()
{
	if( m_NumOutputLines < m_NumRows )
		return m_NumOutputLines;
	else
		return m_NumRows;
}


} // namespace amorphous
