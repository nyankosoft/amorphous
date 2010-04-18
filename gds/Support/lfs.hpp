#ifndef __LFS_HPP__
#define __LFS_HPP__


#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <vector>

#ifdef _MSC_VER
#include <direct.h>
#include <assert.h>
#define getcwd _getcwd
#define chdir  _chdir
#else /* _MSC_VER */
#include <unistd.h>
#endif /* _MSC_VER */


/**
 lfs.hpp
   - Various Light FileSystem operations
*/

// NOTE:
// using chdir(), getcwd() under Win32 causes LNK2005 error
// use _chdir(), _getcwd() instead

namespace lfs
{

#define LFS_MAX_PATH 1024


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


inline std::string get_leaf( const std::string& filename )
{
	size_t pos = filename.rfind("\\");

	if( pos != std::string::npos
	 && pos != filename.length() - 1 ) 
		return filename.substr( pos+1 );
	else
	{
		pos = filename.rfind("/");
		if( pos != std::string::npos
		 && pos != filename.length() - 1 ) 
			return filename.substr( pos+1 );
	}

	return std::string();
}


/// returns the current working directory (absolute path to the current directory)
inline std::string get_cwd()
{
	char cwd[LFS_MAX_PATH];
	memset( cwd, 0, sizeof(cwd) );
	getcwd( cwd, LFS_MAX_PATH );
	return std::string(cwd);
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


/// Return the parent path of the given path
/// The returned path does not contain the separator charactor ( '/' or '\\' )
/// at the end.
/// Does not check if src_path exists.
inline std::string get_parent_path( const std::string& src_path )
{
	if( src_path.length() == 0 )
		return std::string();

	const char *sep_str[2] = { "\\", "/" };

	// extract the parent path from the filepath

	size_t last_sep_pos = 0;
	for( int i=0; i<2; i++ )
	{
		size_t pos = std::string::npos;
		if( src_path[src_path.length()-1] == sep_str[i][0] )
			pos = src_path.substr( 0, src_path.length()-1 ).rfind( sep_str[i] ); // the last character is '/' or '\\'
		else
			pos = src_path.rfind( sep_str[i] );

		if( pos != std::string::npos
		 && last_sep_pos < pos )
		{
			last_sep_pos = pos;
		}
	}

	if( last_sep_pos == 0 )
		return std::string(); // 'filepath' is a leaf - return an empty string
	else
		return src_path.substr( 0, last_sep_pos );
}


inline bool is_abspath( const std::string& filename )
{
	if( filename.find( ":\\" ) == 1 || filename.find( ":/" ) == 1 )
		return true;
	else
		return false;
}


/// returns a filename without path
inline std::string get_path_after_dirpath( const std::string& filename )
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
 - example: string filename = "readme.txt"; append_before_ext( filename, "_en" );
 - result: filename = "readme_en.txt"
*/
inline void append_before_ext( std::string& dest_filename, const std::string& str )
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
	else
		return false; // some unknown error
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
		push_cwd_and_chdir( path );
	}
	
	~dir_stack()
	{
	}

	/// Push the current working directory to the stack and
	/// set the working directory specified by the argument 'path'
	bool push_cwd_and_chdir( const std::string& path )
//	bool setdir( const std::string& path )
	{
		if( getcwd( m_buffer, PATH_BUFFER_SIZE-1 ) )
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

	/// Pop the directory path from the stack and change
	/// the working directory to it.
	bool pop_and_chdir()
//	bool prevdir()
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


} // namespace lfs


#endif  /*  __LFS_HPP__  */
