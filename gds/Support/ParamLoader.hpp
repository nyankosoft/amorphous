#ifndef  __PARAMLOADER_H__
#define  __PARAMLOADER_H__


#include "TextFileScanner.hpp"


namespace amorphous
{


/*  --- template 1 (inside a function) ---

	CParamLoader loader( filename );
	if( loader.IsReady() )
	{
		loader.LoadParam( "INPUT", input_filename );
		loader.LoadParam( "OUTPUT", output_filename );
	}
*/


/*  --- template 2 (a function) ---

bool LoadFromFile( const string& filename )
{
	CParamLoader loader( filename );

	if( !loader.IsReady() )
		return false;

	loader.LoadParam( "INPUT", input_filename );
	loader.LoadParam( "OUTPUT", output_filename );
}
*/

class CParamLoader
{
	CTextFileScanner m_Scanner;

public:

	CParamLoader() {}

	CParamLoader( const std::string& filename ) { m_Scanner.OpenFile(filename); }
	
	~CParamLoader() { CloseFile(); }

	bool OpenFile( const std::string& filename ) { m_Scanner.OpenFile( filename ); }

	void CloseFile() { m_Scanner.CloseFile(); }

	bool IsReady() const { return m_Scanner.IsReady(); }

//	bool LoadBoolParam( const char *tag, bool& b, const char *true_word = "true", const char *false_word = "false" );
	bool LoadBoolParam( const char *tag, const std::string& bool_tag_str, bool& b )
	{
		if( m_Scanner.FindLineWithTag(tag) )
		{
			std::string str_tag;
			m_Scanner.ScanBool( str_tag, bool_tag_str, b );
			return true;
		}
		else
			return false;
	}

	template<typename T>
	int LoadParam( const char *tag, T& t )
	{
		if( m_Scanner.FindLineWithTag(tag) )
			return m_Scanner.ScanLine( tag, t );
		else
			return 0;
	}

	template<typename T0, typename T1>
	int LoadParam( const char *tag, T0& t0, T1& t1 )
	{
		if( m_Scanner.FindLineWithTag(tag) )
			return m_Scanner.ScanLine( tag, t0, t1 );
		else
			return 0;
	}

	template<typename T0, typename T1, typename T2>
	int LoadParam( const char *tag, T0& t0, T1& t1, T2& t2 )
	{
		if( m_Scanner.FindLineWithTag(tag) )
			return m_Scanner.ScanLine( tag, t0, t1, t2 );
		else
			return 0;
	}

	template<typename T0, typename T1, typename T2, typename T3>
	int LoadParam( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3 )
	{
		if( m_Scanner.FindLineWithTag(tag) )
			return m_Scanner.ScanLine( tag, t0, t1, t2, t3 );
		else
			return 0;
	}

	template<typename T0, typename T1, typename T2, typename T3, typename T4>
	int LoadParam( const char *tag, T0& t0, T1& t1, T2& t2, T3& t3, T4& t4 )
	{
		if( m_Scanner.FindLineWithTag(tag) )
			return m_Scanner.ScanLine( tag, t0, t1, t2, t3, t4 );
		else
			return 0;
	}
};



template<typename T>
inline void LoadParamFromFile( const std::string& filepath, const char *param_name, T& param )
{
	CParamLoader loader( filepath );
	if( loader.IsReady() )
		loader.LoadParam( param_name, param );
}


template<typename T0, typename T1>
inline void LoadParamFromFile( const std::string& filepath, const char *param_name, T0& param0, T1& param1 )
{
	CParamLoader loader( filepath );
	if( loader.IsReady() )
		loader.LoadParam( param_name, param0, param1 );
}


template<typename T0, typename T1, typename T2>
inline void LoadParamFromFile( const std::string& filepath, const char *param_name, T0& param0, T1& param1, T2& param2 )
{
	CParamLoader loader( filepath );
	if( loader.IsReady() )
		loader.LoadParam( param_name, param0, param1, param2 );
}


template<typename T>
inline T LoadParamFromFile( const std::string& filepath, const char *param_name )
{
	T param;
	CParamLoader loader( filepath );
	if( loader.IsReady() )
		loader.LoadParam( param_name, param );
	return param;
}


/// If you use this function, you need to include "Support/lfs.hpp".
#define UPDATE_PARAM( filepath, param_name, variable ) \
{\
	static time_t s_LastModifiedTime = 0;\
	time_t last_modified_time = lfs::get_last_modified_time( filepath );\
	if( s_LastModifiedTime < last_modified_time )\
	{\
		s_LastModifiedTime = last_modified_time;\
		LoadParamFromFile( filepath, param_name, variable );\
	}\
}\



} // namespace amorphous



#endif		/*  __PARAMLOADER_H__  */
