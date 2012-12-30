#ifndef  __CSimpleBitmapFontData_HPP__
#define  __CSimpleBitmapFontData_HPP__


#include <stdlib.h>


namespace amorphous
{


/// Stores a fixed pitch, 16-level grayscale font.
/// - Used to store hardcoded font
class CSimpleBitmapFontData
{
public:

//	CSimpleBitmapFontData() {}

	CSimpleBitmapFontData( const char *pName = "", int width = 256, int height = 256, int start_row = 64, int end_row = 255, const char **papTexelRow = NULL )//const char *paTexel = "" )
		:
	m_pName(pName),
	m_Width(width),
	m_Height(height),
	m_StartRow(start_row),
	m_EndRow(end_row),
	m_papTexelRow(papTexelRow)
//	m_paTexel(paTexel)
	{
//		m_vecpTexelRow.resize( end_row - start_row + 1 );
	}

	~CSimpleBitmapFontData() {}

	const char *m_pName;
	const int m_Width;
	const int m_Height;
	const int m_StartRow;
	const int m_EndRow;
//	const char *m_paTexel;
	const char **m_papTexelRow;
//	std::vector<char *> m_vecpTexelRow;
};


} // namespace amorphous



#endif // __CSimpleBitmapFontData_HPP__
