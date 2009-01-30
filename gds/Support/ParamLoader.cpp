
#include "ParamLoader.hpp"

using namespace std;


bool CParamLoader::LoadParam( const char *tag, string& str1 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, str1 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, string& str1, string& str2 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, str1, str2 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, float& f )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, f );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, double& f )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, f );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, float& f1, float& f2 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, f1, f2 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, float& f1, float& f2, float& f3 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, f1, f2, f3 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, string& strEntry, float& f )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, strEntry, f );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, int &i )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, i );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, int &i1, int &i2 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, i1, i2 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, int &i1, int &i2, int &i3 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, i1, i2, i3 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, Vector3& v )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, v );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, Vector3& v1, Vector3& v2 )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, v1, v2 );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, int &i, string& strEntry )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, i, strEntry );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, float &f, string& strEntry )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, f, strEntry );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadParam( const char *tag, string& str, Vector3 &v )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanLine( str_tag, str, v );
		return true;
	}
	else
		return false;
}


bool CParamLoader::LoadBoolParam( const char *tag, const std::string& bool_tag_str, bool& b )
{
	if( m_Scanner.FindLineWithTag(tag) )
	{
		string str_tag;
		m_Scanner.ScanBool( str_tag, bool_tag_str, b );
		return true;
	}
	else
		return false;
}
