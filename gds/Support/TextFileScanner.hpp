#ifndef  __TextFileScanner_HPP__
#define  __TextFileScanner_HPP__


#include <stdio.h>
#include "StringAux.hpp"
#include "../Platform.hpp"


#define separate_strings SeparateStrings

/*
class CTextFileScannerAux
{
	const char *m_pTrueWord;
	const char *m_pFalseWord;

public:

	CTextFileScannerAux()
		:
	m_pTrueWord(""),
	m_pFalseWord("")
	{}

	CTextFileScannerAux( const *pTrueWord, const *pFalseWord )
		:
	m_pTrueWord(pTrueWord),
	m_pFalseWord(pFalseWord)
	{}

	~CTextFileScannerAux() {}

	const char *GetTrueWord() const { return m_pTrueWord; }

	const char *GetFalseWord() const { return m_pFalseWord; }
};
*/


inline void conv_to_x( std::vector<std::string>& src, int& index, int& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = to_int(src[index].c_str());
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, uint& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		uint val = 0;
		STRAUX_SSCANF( src[index].c_str(), "%d", &val );
//		dest = to_uint(src);
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, short& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = (short)atoi(src[index].c_str());
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, ushort& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = (unsigned short)atoi(src[index].c_str());
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, float& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = (float)atof(src[index].c_str());
//		dest = 0.0f;
//		STRAUX_SSCANF( src[index].c_str(), "%f", &dest );
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, double& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = atof(src[index].c_str());
//		dest = 0.0f;
//		STRAUX_SSCANF( src[index].c_str(), "%f", &dest );
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, std::string& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		dest = src[index];
		index += 1;
	}
}


inline void conv_to_x( std::vector<std::string>& src, int& index, bool& dest )
{
	if( 1 <= (int)src.size() - index )
	{
		if( src[index] == "true" )//???.GetTrueWord() )
			dest = true;
		else if( src[index] == "false" )//???.GetTrueWord() )
			dest = false;
		index += 1;
	}
}



/**
A Usage Example

- text file to load
----------------------------------------------

name xxx
size 20 30
height 50


- C++ source code
----------------------------------------------

int ProcessTextFile( std::string& filepath )
{
	CTextFileScanner scanner( filepath );

	if( !scanner.IsReady() )
	{
//		printf( "Failed to open file: %s\n", filepath.c_str() );
		return -1;
	}

	string name;
	int size_x=0, size_y=0,
	int height=0;

	string line;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.GetCurrentLine( line );

		scanner.TryScanLine( "name", name );
		scanner.TryScanLine( "size", size_x, size_y );
		scanner.TryScanLine( "height", height );
	}

	return 0;
}
*/
class CTextFileScanner
{
	enum param { MAX_LINE_LENGTH = 2048 };

	FILE* m_fp;

	/// buffer for current line
	char m_acCurrentLine[MAX_LINE_LENGTH];

	/// buffer to store the tag of the current line
	std::string m_strTag;

	inline void Init();

	inline void UpdateTagStr();

	inline bool GetBoolTagStr( std::string& tag_true, std::string& tag_false, const std::string& bool_tag_str );

	inline void BreakString( std::vector<std::string>& dest, const std::string& src )
	{
		separate_strings( dest, src.c_str(), " \t\n" );
	}

public:

	CTextFileScanner() { Init(); }

	CTextFileScanner( const std::string& strFilename ) { Init(); OpenFile(strFilename); }
	
	~CTextFileScanner() { CloseFile(); }

	inline bool OpenFile( const std::string& strFilename );

	void CloseFile() { if( m_fp ) fclose(m_fp); m_fp = NULL; }

	/// returns true if the file is open and ready to scan
	bool IsReady() const { return (m_fp != NULL); }

	/// move to the next line
	/// returns false if the scanner reached the end of the file
	inline bool NextLine();

	/// returns if the scanning has reached the end of the file
	inline bool End();

	/// get the current entire line
	inline void GetCurrentLine( std::string& str ) { str = m_acCurrentLine; }

	inline const std::string GetCurrentLine() const { std::string str = m_acCurrentLine; return str; }

	/// get the first std::string component of the current line
	inline bool GetTagString( std::string& strTag );

	const std::string& GetTagString() const { return m_strTag; }

	const char *GetTagStr() const { return m_strTag.c_str(); }

	/// scan the current line in various formats

//	inline bool ScanBool( const std::string& strTag, bool& b, const char *true_word = "true", const char *false_word = "false" );
	inline bool ScanBool( const std::string& strTag, const std::string& bool_str, bool& b );

	inline bool ScanSentence( std::string& strTag, std::string& text );


	/// loads the params and returns true if first arg 'tag' matches the tag on the current line
	/// otherwise just returns false

//	inline bool TryScanBool( const char *tag, const char *true_word = "true", const char *false_word = "false" );
	inline bool TryScanBool( const char *tag, const std::string& bool_str, bool& b );

//	inline bool TryScanSentence( const char *tag, std::string& text );

	/// searchs a line that begins with the specified tag.
	/// The search will stop at the first hit, and the subsequenet ScanLine()
	/// or TryScanLine() call will be applied to the line which contains the tag.
	/// The file pointer position is reset when this method is called.
	inline bool FindLineWithTag( const std::string& tag );


/*	inline void conv_to_x( std::vector<std::string>& src, int& index, bool& dest )
	{
		if( 1 <= (int)src.size() - index )
		{
			if( src[index] == "true" )
				dest = true;
			else if( src[index] == "false" )
				dest = false;
			index += 1;
		}
	}*/

	template<typename T>
	int ScanLine( const char *tag, T& t )
	{
		std::vector<std::string> strings;
		BreakString( strings, m_acCurrentLine );
		if( strings.size() <= 1 )
			return 0;

		int index = 1;
		conv_to_x( strings, index, t );
		return index - 1;
	}

	template<typename T0, typename T1>
	int ScanLine( const char *tag, T0& t0, T1& t1 )
	{
		std::vector<std::string> strings;
		BreakString( strings, m_acCurrentLine );
		if( strings.size() <= 1 )
			return 0;

		int index = 1;
		conv_to_x( strings, index, t0 );
		conv_to_x( strings, index, t1 );
		return index - 1;
	}

	template<typename T0, typename T1, typename T2>
	int ScanLine( const char *tag, T0& t0, T1& t1, T2& t2 )
	{
		std::vector<std::string> strings;
		BreakString( strings, m_acCurrentLine );
		if( strings.size() <= 1 )
			return 0;

		int index = 1;
		conv_to_x( strings, index, t0 );
		conv_to_x( strings, index, t1 );
		conv_to_x( strings, index, t2 );
		return index - 1;
	}

	template<typename T0, typename T1, typename T2, typename T3>
	int ScanLine( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3 )
	{
		std::vector<std::string> strings;
		BreakString( strings, m_acCurrentLine );
		if( strings.size() <= 1 )
			return 0;

		int index = 1;
		conv_to_x( strings, index, t0 );
		conv_to_x( strings, index, t1 );
		conv_to_x( strings, index, t2 );
		conv_to_x( strings, index, t3 );
		return index - 1;
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	int ScanLine( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3, T4& t4 )
	{
		std::vector<std::string> strings;
		BreakString( strings, m_acCurrentLine );
		if( strings.size() <= 1 )
			return 0;

		int index = 1;
		conv_to_x( strings, index, t0 );
		conv_to_x( strings, index, t1 );
		conv_to_x( strings, index, t2 );
		conv_to_x( strings, index, t3 );
		conv_to_x( strings, index, t4 );
		return index - 1;
	}

	template<typename T>
	int ScanLine( const std::string& tag, T& t ) { return ScanLine( tag.c_str(), t ); }

	template<typename T0, typename T1>
	int ScanLine( const std::string& tag, T0& t0, T1& t1 ) { return ScanLine( tag.c_str(), t0, t1 ); }

	template<typename T0, typename T1, typename T2>
	int ScanLine( const std::string& tag, T0& t0, T1& t1, T2& t2 ) { return ScanLine( tag.c_str(), t0, t1, t2 ); }

	template<typename T0, typename T1, typename T2, typename T3>
	int ScanLine( const std::string& tag, T0& t0, T1& t1, T2& t2, T3& t3 ) { return ScanLine( tag.c_str(), t0, t1, t2, t3 ); }

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	int ScanLine( const std::string& tag, T0& t0, T1& t1, T2& t2, T3& t3, T4& t4 ) { return ScanLine( tag.c_str(), t0, t1, t2, t3, t4 ); }

	template<typename T>
	int TryScanLine( const char *tag, T& t )
	{
		if( strcmp( GetTagStr(), tag ) != 0 )
			return 0;

		return ScanLine( tag, t );
	}

	template<typename T0, typename T1>
	int TryScanLine( const char *tag, T0& t0, T1& t1 )
	{
		if( strcmp( GetTagStr(), tag ) != 0 )
			return 0;

		return ScanLine( tag, t0, t1 );
	}

	template<typename T0, typename T1, typename T2>
	int TryScanLine( const char *tag, T0& t0, T1& t1, T2& t2 )
	{
		if( strcmp( GetTagStr(), tag ) != 0 )
			return 0;

		return ScanLine( tag, t0, t1, t2 );
	}

	template<typename T0, typename T1, typename T2, typename T3>
	int TryScanLine( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3 )
	{
		if( strcmp( GetTagStr(), tag ) != 0 )
			return 0;

		return ScanLine( tag, t0, t1, t2, t3 );
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	int TryScanLine( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3, T4& t4 )
	{
		if( strcmp( GetTagStr(), tag ) != 0 )
			return 0;

		return ScanLine( tag, t0, t1, t2, t3, t4 );
	}
};

//======================= inline implementations =======================

inline void CTextFileScanner::Init()
{
	m_fp = NULL;
	m_strTag = "";
	memset( m_acCurrentLine, 0, sizeof(char) * MAX_LINE_LENGTH );
}


inline void CTextFileScanner::UpdateTagStr()
{
	std::vector<std::string> strings;
	separate_strings( strings, m_acCurrentLine, " \t\n" );
	if( 0 < strings.size() )
		m_strTag = strings[0];
	else
		m_strTag = "";
}


inline bool CTextFileScanner::GetBoolTagStr( std::string& tag_true,  std::string& tag_false,  const std::string& bool_tag_str )
{
	size_t divider_pos = bool_tag_str.find( "/" );

	if( divider_pos == std::string::npos || divider_pos == 0 || divider_pos == bool_tag_str.length() - 1 )
		return false;

	tag_true  = bool_tag_str.substr( 0, divider_pos );
	tag_false = bool_tag_str.substr( divider_pos + 1, bool_tag_str.length() - divider_pos - 1);

	return true;
}


inline bool CTextFileScanner::OpenFile( const std::string& strFilename )
{
	FILE *fp = fopen( strFilename.c_str(), "r" );

	if( !fp )
		return false;
	else
	{
		// close if a file is currently open
		CloseFile();

		// initialize the instance
		Init();

		m_fp = fp;

		// load the first line
		fgets( m_acCurrentLine, MAX_LINE_LENGTH - 1, m_fp );
		UpdateTagStr();
		return true;
	}

	// perform a floating-point assignment to cause floating-point support to be loaded
	float f = 1.0f;
}


inline bool CTextFileScanner::NextLine()
{
	if( !m_fp )
		return false;

	m_acCurrentLine[0] = '\0';

	char *p = fgets( m_acCurrentLine, MAX_LINE_LENGTH - 1, m_fp );

	// reset & update the tag string
	UpdateTagStr();

	if( p )
		return true;
	else
		return false;
}


inline bool CTextFileScanner::End()
{
	if( !m_fp )
		return false;

	if( feof(m_fp) && strlen(m_acCurrentLine) == 0 )
		return true;
	else
		return false;
}


inline bool CTextFileScanner::GetTagString( std::string& strTag )
{
	if( !m_fp || strlen(m_acCurrentLine) == 0 )
		return false;

	strTag = m_strTag;

	return true;
}


inline bool CTextFileScanner::ScanBool( const std::string& strTag, const std::string& bool_tag_str, bool& b )
{
	std::string tag_true, tag_false, bool_tag;
	if( !GetBoolTagStr( tag_true, tag_false, bool_tag_str ) )
		return false;

	ScanLine( strTag.c_str(), bool_tag );

	if( bool_tag == tag_true )		{ b = true;		return true; }
	else if( bool_tag == tag_false ){ b = false;	return true; }
	else return false;
}


inline bool CTextFileScanner::ScanSentence( std::string& strTag, std::string& text )
{
	std::string line_buffer = this->GetCurrentLine();
	size_t pos = GetTagString().length();

	while( pos < line_buffer.length() )
	{
		if( line_buffer[pos] != '\t'
		 && line_buffer[pos] != ' ' )
		 break;

		pos++;
	}

	for( ; pos < line_buffer.length(); pos++ )
	{
		if( line_buffer.substr(pos,2) == "\\n" )
		{
			text.push_back( '\n' );
			pos++;
		}
		else
			text.push_back( line_buffer[pos] );
	}

	return true;
/*
	if( pos < line_buffer.length() )
	{
		text = line_buffer.substr( pos, line_buffer.length() - pos );
        return true;
	}
	else
        return true;*/
}


inline bool CTextFileScanner::TryScanBool( const char *tag, const std::string& bool_str, bool& b )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		std::string str_tag;
		return ScanBool( str_tag, bool_str, b );
	}
	else
		return false;
}

/*
inline bool CTextFileScanner::TryScanSentence( const char *tag, string& text )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		std::string str_tag;
		ScanSentence( str_tag, text );
		return true;
	}
	else
		return false;
}*/


inline bool CTextFileScanner::FindLineWithTag( const std::string& tag )
{
	if( !m_fp )
		return false;

	// reset the file pointer
	// TDOO: check 2nd & 3rd arguments
	fseek( m_fp, 0, SEEK_SET );

	// load the first line
	fgets( m_acCurrentLine, MAX_LINE_LENGTH - 1, m_fp );
	UpdateTagStr();

	for( ; !End(); NextLine() )
	{
		if( GetTagString() == tag )
			return true;
	}

	return false;
}



#endif		/*  __TextFileScanner_HPP__  */
