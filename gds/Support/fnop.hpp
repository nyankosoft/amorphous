#ifndef __FNOP_H__
#define __FNOP_H__


#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <direct.h>
#include <string>
#include <vector>


/**
 fnop.hpp
   - functions for File & fileName OPerations
*/

// NOTE:
// using chdir(), getcwd() under Win32 causes LNK2005 error
// use _chdir(), _getcwd() instead

namespace fnop
{

#define FNOP_MAX_PATH 1024


inline void change_ext( std::string& target_filename, const std::string& dest_ext )
{
	size_t pos = target_filename.rfind( "." );

	if( pos == std::string::npos )
		return;
	else
        target_filename = target_filename.substr( 0, pos + 1 ) + dest_ext;
}


/// returns the extension(suffix) of the filename
inline std::string get_ext( const std::string& filename )
{
	size_t pos = filename.rfind( "." );

	if( pos == std::string::npos )
		return std::string("");
	else
        return filename.substr( pos + 1, filename.size() - (pos + 1) );
}


/// returns the extension(suffix) of the filename
inline std::string get_nopathfilename( const std::string& filename )
{
	size_t pos = filename.rfind("\\");

	if( pos != std::string::npos
	 && pos != filename.length() - 1 ) 
		return filename.substr( pos+1, filename.length() - (pos+1) );
	else
	{
		pos = filename.rfind("/");
		if( pos != std::string::npos
		 && pos != filename.length() - 1 ) 
			return filename.substr( pos+1, filename.length() - (pos+1) );
	}

	return std::string("");
}


/// returns the current working directory (absolute path to the current directory)
inline std::string get_cwd()
{
	char strCWD[FNOP_MAX_PATH];
//	GetCurrentDirectory( FNOP_MAX_PATH, strCWD );
	getcwd( strCWD, FNOP_MAX_PATH );
	return std::string(strCWD);
}


/// set the working directory
inline bool set_wd( const std::string& workdir )
{
//	SetCurrentDirectory( workdir.c_str() );
	if( _chdir( workdir.c_str() ) == 0 )
		return true;
	else
		return false;
}


/// TODO: determine the return value when the filename has no directory path
inline std::string get_path( const std::string& filename )
{
    char str[FNOP_MAX_PATH];
	strcpy( str, filename.c_str() );
	char* pLastSlash = strrchr( str, L'\\' );
	if( pLastSlash )
		*(pLastSlash + 1) = 0;
	else
	{
		pLastSlash = strrchr( str, '/' );
		if( pLastSlash )
			*(pLastSlash + 1) = 0;
		else
			strcpy( str, "" );	// the filename has no directory path
//			strcpy( str, "." );	// the filename has no directory path
	}

	return std::string(str);
}


inline bool has_abspath( const std::string& filename )
{
	if( filename.find( ":\\" ) == 1 || filename.find( ":/" ) == 1 )
		return true;
	else
		return false;
}


/// returns a filename without path
inline std::string get_nopath( const std::string& filename )
{
	size_t pos = filename.rfind( "/" );
	if( pos == std::string::npos )
	{
		pos = filename.rfind( "\\" );
	}

	if( pos == std::string::npos )
	{
		return filename;
	}

	return filename.substr( pos+1, filename.length() - (pos+1) );
}


/// returns a filename without suffix
inline std::string get_no_ext( const std::string& filename )
{
	size_t pos = filename.rfind( "." );
	if( pos == std::string::npos )
		return filename;

	return filename.substr( 0, pos );
}

/**
 insert a string before the suffix
 - example: string filename = "readme.txt"; append_to_body( filename, "_en" );
 - result: filename = "readme_en.txt"
*/
inline void append_to_body( std::string& dest_filename, const std::string& str )
{
	size_t dot_pos = dest_filename.rfind( "." );
	if( dot_pos == std::string::npos )
	{
		dest_filename += str;
	}

	dest_filename.insert( dot_pos, str );
}



inline int get_filesize( const std::string& filename )
{
	struct stat results;

	if( stat(filename.c_str(), &results) == 0 )
		return (int)(results.st_size);
	else
		return -1;
}


/// returns true if the file exists
inline bool file_exists( const std::string& filename )
{
	FILE* fp = fopen( filename.c_str(), "r" );
	if( fp )
	{
		fclose(fp);
		return true;
	}
	else
		return false;
}


/// returns true if the path exists
inline bool path_exists( const std::string& pathname )
{
	struct stat results;

	int ret = stat(pathname.c_str(), &results);

	if( ret == 0 )
		return true;
	else if( ret == -1 )
		return false;
	else if( ret == EINVAL )
	{
		assert( !"path_exists() - EINVAL returned from stat()" );
		return false;
	}
}


/// returns the last modified time of the file
inline time_t get_last_modified_time( const std::string& filename )
{
	struct stat obj_info;
	int ret = stat( filename.c_str(), &obj_info );

	if( ret == 0 )
		return obj_info.st_mtime;
	else
		return 0;	// error
}


inline void get_filedata( const std::string& filename, std::vector<unsigned char>& rDestBuffer )
{
	FILE *fp = fopen( filename.c_str(), "rb" );
	if( !fp )
		return;

	int filesize = get_filesize(filename);
	if( filesize < 0 )
		return;

	rDestBuffer.resize(filesize);
	fread( &rDestBuffer[0], 1, filesize, fp );

	fclose(fp);
}


class dir_stack
{
	enum params { PATH_BUFFER_SIZE = 512 };

	std::vector<std::string> m_vecDirectory;

	char m_buffer[PATH_BUFFER_SIZE];

public:

	dir_stack() {}

	dir_stack( const std::string& path )
	{
		setdir( path );
	};

	bool setdir( const std::string& path )
	{
		if( _getcwd( m_buffer, PATH_BUFFER_SIZE-1 ) )
		{
			// save the current directory
			m_vecDirectory.push_back( std::string( m_buffer ) );

			// set the new directory
			if( _chdir( path.c_str() ) == 0 )
			{
				return true;
			}
			else
			{
				// the specified path was not found
				m_vecDirectory.pop_back();
				return false;
			}
		}
		else
			return false;
	}

	bool prevdir()
	{
		if( m_vecDirectory.size() == 0 )
			return false;	// no directory is in the stack

		const std::string prev_path = m_vecDirectory.back();
		if( _chdir( prev_path.c_str() ) == 0 )
		{
			m_vecDirectory.pop_back();	// delete the last path at the top of the stack
			return true;
		}
		else
			return false;
	}
};


}  /* namespace fnop */

#endif  /*  __FNOP_H__  */


