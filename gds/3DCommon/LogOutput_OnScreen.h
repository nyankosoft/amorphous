#ifndef  __LogOutput_ScrolledTextBuffer_H__
#define  __LogOutput_ScrolledTextBuffer_H__

#include <vector>
#include <string>

#include "../base.h"
#include "Support/Log/LogOutputBase.h"
#include "Support/2DArray.h"
#include "3DMath/Vector2.h"


/**
 * displays log texts on screen using D3D
 *
 */
class CLogOutput_ScrolledTextBuffer : public CLogOutputBase
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

	CLogOutput_ScrolledTextBuffer( const std::string& font_name, int font_width, int font_height,
                         int num_rows = 16, int num_chars = 64 );

	CLogOutput_ScrolledTextBuffer( int num_rows = 16, int num_chars = 64 );	///< use this with a borrowed font

	~CLogOutput_ScrolledTextBuffer();

	virtual void Print( const CLogMessage& msg );

	inline const char *GetText( int row );

	inline U32 GetTextColor( int row );

	int GetNumCurrentRows();
};


//------------------------- inline implementations -------------------------

inline const char *CLogOutput_ScrolledTextBuffer::GetText( int row )
{
	if( row < 0 || m_NumRows <= row )
		return "";

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextBuffer[row_index].c_str();
}


inline U32 CLogOutput_ScrolledTextBuffer::GetTextColor( int row )
{
	if( row < 0 || m_NumRows <= row )
		return 0xFFFFFFFF;

	const int row_index = ( m_EndRowIndex + row ) % m_NumRows;

	return m_TextColor[row_index];
}


#endif		/*  __LogOutput_ScrolledTextBuffer_H__  */
