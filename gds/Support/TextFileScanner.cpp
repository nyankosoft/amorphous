
#include "TextFileScanner.h"

using namespace std;


bool CTextFileScanner::GetBoolTagStr( std::string& tag_true,  std::string& tag_false,  const std::string& bool_tag_str )
{
	size_t divider_pos = bool_tag_str.find( "/" );

	if( divider_pos == string::npos || divider_pos == 0 || divider_pos == bool_tag_str.length() - 1 )
		return false;

	tag_true  = bool_tag_str.substr( 0, divider_pos );
	tag_false = bool_tag_str.substr( divider_pos + 1, bool_tag_str.length() - divider_pos - 1);

	return true;
}


void CTextFileScanner::Init()
{
	m_fp = NULL;
	memset( m_acTag, 0, sizeof(char) * MAX_TAG_LENGTH );
	memset( m_acCurrentLine, 0, sizeof(char) * MAX_LINE_LENGTH );
}


bool CTextFileScanner::OpenFile( const string& strFilename )
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


bool CTextFileScanner::NextLine()
{
	if( !m_fp )
		return false;

	m_acCurrentLine[0] = '\0';

	char *p = fgets( m_acCurrentLine, MAX_LINE_LENGTH - 1, m_fp );

	// reset & update the tag string
	memset( m_acTag, 0, sizeof(char) * MAX_TAG_LENGTH );
	UpdateTagStr();

	if( p )
		return true;
	else
		return false;
}


bool CTextFileScanner::End()
{
	if( !m_fp )
		return false;

	if( feof(m_fp) && strlen(m_acCurrentLine) == 0 )
		return true;
	else
		return false;
}


bool CTextFileScanner::GetTagString( string& strTag )
{
	if( !m_fp || strlen(m_acCurrentLine) == 0 )
		return false;

	strTag = m_acTag;

	return true;
}


void CTextFileScanner::ScanLine( string& strTag, string& str1 )
{
	char acStr1[256];

	sscanf( m_acCurrentLine, "%s %s\n", m_acTag, acStr1 );

	strTag = m_acTag;
	str1 = acStr1;
}


void CTextFileScanner::ScanLine( string& strTag, string& str1, string& str2 )
{
	char acStr1[256], acStr2[256];

	sscanf( m_acCurrentLine, "%s %s %s\n", m_acTag, acStr1, acStr2 );

	strTag = m_acTag;
	str1 = acStr1;
	str2 = acStr2;
}


void CTextFileScanner::ScanLine( string& strTag, double& f )
{
	float _f = 0;
	sscanf( m_acCurrentLine, "%s %f\n", m_acTag, &_f );
	f = _f;
}


void CTextFileScanner::ScanLine( string& strTag, float& f )
{
	sscanf( m_acCurrentLine, "%s %f\n", m_acTag, &f );
}


void CTextFileScanner::ScanLine( string& strTag, float& f1, float& f2 )
{
	sscanf( m_acCurrentLine, "%s %f %f\n", m_acTag, &f1, &f2 );
}


void CTextFileScanner::ScanLine( string& strTag, float& f1, float& f2, float& f3 )
{
	sscanf( m_acCurrentLine, "%s %f %f %f\n", m_acTag, &f1, &f2, &f3 );
}


void CTextFileScanner::ScanLine( string& strTag, float& f1, float& f2, float& f3, float& f4 )
{
	sscanf( m_acCurrentLine, "%s %f %f %f %f\n", m_acTag, &f1, &f2, &f3, &f4 );
}


void CTextFileScanner::ScanLine( string& strTag, string& strEntry, float& f )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f\n", m_acTag, acEntry, &f );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( string& strTag, string& strEntry, float& f1, float& f2 )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f %f\n", m_acTag, acEntry, &f1, &f2 );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( string& strTag, string& strEntry, float& f1, float& f2, float& f3 )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f %f %f\n", m_acTag, acEntry, &f1, &f2, &f3 );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( string& strTag, string& strEntry, float& f1, float& f2, float& f3, float& f4 )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f %f %f %f\n", m_acTag, acEntry, &f1, &f2, &f3, &f4 );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( string& strTag, int &i )
{
	sscanf( m_acCurrentLine, "%s %d\n", m_acTag, &i );
}


void CTextFileScanner::ScanLine( string& strTag, int &i1, int &i2 )
{
	sscanf( m_acCurrentLine, "%s %d %d\n", m_acTag, &i1, &i2 );
}


void CTextFileScanner::ScanLine( string& strTag, int &i1, int &i2, int &i3 )
{
	sscanf( m_acCurrentLine, "%s %d %d %d\n", m_acTag, &i1, &i2, &i3 );
}


void CTextFileScanner::ScanLine( string& strTag, int &i1, int &i2, int &i3, int &i4 )
{
	sscanf( m_acCurrentLine, "%s %d %d %d %d\n", m_acTag, &i1, &i2, &i3, &i4 );
}


void CTextFileScanner::ScanLine( string& strTag, int &i1, int &i2, int &i3, int &i4, int &i5 )
{
	sscanf( m_acCurrentLine, "%s %d %d %d %d %d\n", m_acTag, &i1, &i2, &i3, &i4, &i5 );
}


void CTextFileScanner::ScanLine( string& strTag, Vector3& v )
{
	ScanLine( strTag, v[0], v[1], v[2] );

	//	sscanf( m_acCurrentLine, "%s %f %f %f\n", m_acTag, &(v[0]), &(v[1]), &(v[2]) );
}


void CTextFileScanner::ScanLine( string& strTag, Vector3& v1, Vector3& v2 )
{
	sscanf( m_acCurrentLine, "%s %f %f %f %f %f %f\n", m_acTag, &v1[0], &v1[1], &v1[2], &v2[0], &v2[1], &v2[2] );
}


void CTextFileScanner::ScanLine( std::string& strTag, int& i, Vector3& v )
{
	sscanf( m_acCurrentLine, "%s %d %f %f %f\n", m_acTag, &i, &v[0], &v[1], &v[2] );
}


void CTextFileScanner::ScanLine( string& strTag, int &i, string& strEntry )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %d %s\n", m_acTag, &i, acEntry );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( string& strTag, float &f, string& strEntry )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %f %s\n", m_acTag, &f, acEntry );
	strEntry = acEntry;
}


void CTextFileScanner::ScanLine( std::string& strTag, std::string& str, Vector3& v )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f %f %f\n", m_acTag, acEntry, &v[0], &v[1], &v[2] );
	str = acEntry;
}


void CTextFileScanner::ScanLine( std::string& strTag, std::string& str, Vector3& v1, Vector3& v2 )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %f %f %f %f %f %f\n", m_acTag, acEntry,
		&v1[0], &v1[1], &v1[2], &v2[0], &v2[1], &v2[2] );
	str = acEntry;
}


void CTextFileScanner::ScanLine( std::string& strTag, std::string& str, int &i0, int &i1 )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %d %d\n", m_acTag, acEntry, &i0, &i1 );
	str = acEntry;
}


void CTextFileScanner::ScanLine( std::string& strTag, std::string& str, int &i )
{
	char acEntry[512];
	sscanf( m_acCurrentLine, "%s %s %d\n", m_acTag, acEntry, &i );
	str = acEntry;
}


bool CTextFileScanner::ScanBool( std::string& strTag, const std::string& bool_tag_str, bool& b )
{
	string tag_true, tag_false, bool_tag;
	if( !GetBoolTagStr( tag_true, tag_false, bool_tag_str ) )
		return false;

	ScanLine( strTag, bool_tag );

	if( bool_tag == tag_true )		{ b = true;		return true; }
	else if( bool_tag == tag_false ){ b = false;	return true; }
	else return false;
}


bool CTextFileScanner::ScanSentence( std::string& strTag, std::string& text )
{
	string line_buffer = this->GetCurrentLine();
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


bool CTextFileScanner::TryScanLine( const char *tag, string& str1 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str1 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& str1, string& str2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str1, str2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, double& f )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, float& f )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, float& f1, float& f2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f1, f2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, float& f1, float& f2, float& f3 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f1, f2, f3 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, float& f1, float& f2, float& f3, float& f4 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f1, f2, f3, f4 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& strEntry, float& f )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, strEntry, f );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& strEntry, float& f1, float& f2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, strEntry, f1, f2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& strEntry, float& f1, float& f2, float& f3 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, strEntry, f1, f2, f3 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& strEntry, float& f1, float& f2, float& f3, float& f4 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, strEntry, f1, f2, f3, f4 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i1, int &i2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i1, i2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i1, int &i2, int &i3 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i1, i2, i3 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i1, int &i2, int &i3, int &i4 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i1, i2, i3, i4 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i1, int &i2, int &i3, int &i4, int &i5 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i1, i2, i3, i4, i5 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, Vector3& v )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, v );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, Vector3& v1, Vector3& v2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, v1, v2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int& i, Vector3& v )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i, v );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& str, Vector3 &v )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str, v );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& str, Vector3 &v1, Vector3 &v2 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str, v1, v2 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, int &i, string& strEntry )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, i, strEntry );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, float &f, string& strEntry )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, f, strEntry );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& str, int &i0, int &i1 )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str, i0, i1 );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanLine( const char *tag, string& str, int &i )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanLine( str_tag, str, i );
		return true;
	}
	else
		return false;
}


bool CTextFileScanner::TryScanBool( const char *tag, const string& bool_str, bool& b )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		return ScanBool( str_tag, bool_str, b );
	}
	else
		return false;
}


bool CTextFileScanner::TryScanSentence( const char *tag, string& text )
{
	if( !strcmp( GetTagStr(), tag ) )
	{
		string str_tag;
		ScanSentence( str_tag, text );
		return true;
	}
	else
		return false;
}



bool CTextFileScanner::FindLineWithTag( const string& tag )
{

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

