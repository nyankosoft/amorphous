#ifndef  __LogOutput_ScrolledTextBuffer_H__
#define  __LogOutput_ScrolledTextBuffer_H__

#include <vector>
#include <string>

#include "../base.hpp"
#include "Support/Log/LogOutputBase.hpp"
#include "Support/2DArray.hpp"
#include "3DMath/Vector2.hpp"


namespace amorphous
{


/**
 * displays log texts on screen using D3D
 *
 */
class LogOutput_ScrolledTextBuffer : public CLogOutputBase
{
protected:

	std::vector<std::string> m_TextBuffer;
	std::vector<U32> m_TextColor;

	int m_EndRowIndex;

	int m_NumRows;

	/// how many logs have been taken in total
	int m_NumOutputLines;

private:

	void Init( int num_rows );

public:

	LogOutput_ScrolledTextBuffer( const std::string& font_name, int font_width, int font_height,
                         int num_rows = 16, int num_chars = 64 );

	LogOutput_ScrolledTextBuffer( int num_rows = 16, int num_chars = 64 );	///< use this with a borrowed font

	~LogOutput_ScrolledTextBuffer();

	virtual void Print( const CLogMessage& msg );

	inline const char *GetText( int row );

	inline U32 GetTextColor( int row );

	int GetNumCurrentRows();
};


//------------------------- inline implementations -------------------------

inline const char *LogOutput_ScrolledTextBuffer::GetText( int row )
{
	if( row < 0 || m_NumRows <= row )
		return "";

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextBuffer[row_index].c_str();
}


inline U32 LogOutput_ScrolledTextBuffer::GetTextColor( int row )
{
	if( row < 0 || m_NumRows <= row )
		return 0xFFFFFFFF;

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextColor[row_index];
}

} // namespace amorphous



#endif		/*  __LogOutput_ScrolledTextBuffer_H__  */
