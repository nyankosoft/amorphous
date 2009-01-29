
#ifndef  __PARAMLOADER_H__
#define  __PARAMLOADER_H__


#include <stdio.h>

#include <string>

#include "3DMath/Vector3.h"

#include "TextFileScanner.h"


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
/*
	void ScanLine( std::string& strTag, std::string& str1 );
	void ScanLine( std::string& strTag, std::string& str1, std::string& str2 );

	void ScanLine( std::string& strTag, float& f );
	void ScanLine( std::string& strTag, float& f1, float& f2 );
	void ScanLine( std::string& strTag, float& f1, float& f2, float& f3 );

	void ScanLine( std::string& strTag, std::string& strEntry, float& f );

	void ScanLine( std::string& strTag, int &i );
	void ScanLine( std::string& strTag, int &i1, int &i2 );
	void ScanLine( std::string& strTag, int &i1, int &i2, int &i3 );

	void ScanLine( std::string& strTag, Vector3& v );
	void ScanLine( std::string& strTag, Vector3& v1, Vector3& v2 );

	void ScanLine( std::string& strTag, int &i, std::string& strEntry );
	void ScanLine( std::string& strTag, float &f, std::string& strEntry );

	void ScanLine( std::string& strTag, std::string& str, Vector3& v );
*/

	/// try to load params
	/// \retval true the specified tag was found and params have been loaded if they are properly formatted
	/// \retval false the specified tag was not found and params have not been loaded
	bool LoadParam( const char *tag, std::string& str1 );
	bool LoadParam( const char *tag, std::string& str1, std::string& str2 );

	bool LoadParam( const char *tag, double& f );

	bool LoadParam( const char *tag, float& f );
	bool LoadParam( const char *tag, float& f1, float& f2 );
	bool LoadParam( const char *tag, float& f1, float& f2, float& f3 );

	bool LoadParam( const char *tag, std::string& strEntry, float& f );

	bool LoadParam( const char *tag, int &i );
	bool LoadParam( const char *tag, int &i1, int &i2 );
	bool LoadParam( const char *tag, int &i1, int &i2, int &i3 );

	bool LoadParam( const char *tag, Vector3& v );
	bool LoadParam( const char *tag, Vector3& v1, Vector3& v2 );

	bool LoadParam( const char *tag, int &i, std::string& strEntry );
	bool LoadParam( const char *tag, float &f, std::string& strEntry );

	bool LoadParam( const char *tag, std::string& str, Vector3& v );

	bool LoadBoolParam( const char *tag, const std::string& bool_tag_str, bool& b );
};



#endif		/*  __PARAMLOADER_H__  */