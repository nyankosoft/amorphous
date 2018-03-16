#ifndef __LFS_HPP__
#define __LFS_HPP__


#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <string>
#include <cstring> // std::memset
#include <vector>
#include <regex>

#ifdef _MSC_VER
#include <direct.h>
#define getcwd _getcwd
#define chdir  _chdir
#else /* _MSC_VER */
#include <unistd.h>
#endif /* _MSC_VER */


namespace amorphous
{


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


inline std::string get_leaf( const std::string& pathname )
{
	using std::string;

	struct local
	{
		static size_t rfind_separator( const std::string& pathname, size_t reverse_search_start_pos = std::string::npos )
		{
			using std::string;

			const size_t slash_pos     = pathname.rfind( "/",  reverse_search_start_pos );
			const size_t backslash_pos = pathname.rfind( "\\", reverse_search_start_pos );
			if( slash_pos == string::npos )
			{
				if( backslash_pos == string::npos )
					return string::npos;
				else
					return backslash_pos;
			}
			else
			{
				if( backslash_pos == string::npos )
					return slash_pos;
				else
					return (slash_pos < backslash_pos) ? backslash_pos : slash_pos;
			}
		}
	};

	if( pathname.length() == 0 )
		return string();

	const size_t separator_pos = local::rfind_separator( pathname );

	if( separator_pos == string::npos )
	{
		// Has no path separator - pathname is a leaf
		return pathname;
	}
	else
	{
		if( separator_pos == pathname.length() - 1 )
		{
			// The pathname ends with a separator character ('/' or '\\').
			return string();
		}
		else
		{
			return pathname.substr( separator_pos + 1 );
		}
	}

	return std::string();
}


/// returns the current working directory (absolute path to the current directory)
inline std::string get_cwd()
{
	char cwd[LFS_MAX_PATH];
	std::memset( cwd, 0, sizeof(cwd) );
	getcwd( cwd, LFS_MAX_PATH );
	return std::string(cwd);
}


/// set the working directory
inline bool set_wd( const std::string& workdir )
{
//	SetCurrentDirectory( workdir.c_str() );
	if( chdir( workdir.c_str() ) == 0 )
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
inline void insert_before_extension( std::string& dest_filename, const std::string& str )
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


inline std::string connect_path( const std::string& lhs, const std::string& rhs )
{
	if( lhs.length() == 0 )
		return rhs;

	if( rhs.length() == 0 )
		return lhs;

	char lhs_last_char = lhs[ lhs.length()-1 ];
	char rhs_first_char = rhs[0];
	if( lhs_last_char == '\\' || lhs_last_char == '/' )
	{
		if( rhs_first_char == '\\' || rhs_first_char == '/' )
			return lhs + rhs.substr( 1 );
		else
			return lhs + rhs;
	}
	else
	{
		if( rhs_first_char == '\\' || rhs_first_char == '/' )
			return lhs + rhs;
		else
			return lhs + "/" + rhs;
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


class path
{
	std::string pathname;

public:

	path() {}

	path(const char *_pathname)
		:
		pathname(_pathname)
	{}

	path(const std::string& _pathname)
		:
		pathname(_pathname)
	{}

	path operator/(const path& rhs) const
	{
		if( rhs.pathname.find(".\\") == 0 || rhs.pathname.find("./") == 0 )
		{
			// rhs starts with ".\"
			return path(this->pathname + "/" + rhs.pathname.substr(2));
		}
		else
			return path(this->pathname + "/" + rhs.pathname);
	}

	path parent_path() const
	{
		return get_parent_path(pathname);
	}

	path filename() const
	{
		return path(get_leaf(pathname));
	}

	// Deprecated in boost::filesystem
	path leaf() const
	{
		return filename();
	}

	path stem() const
	{
		path leaf = this->leaf();
		auto dot_pos = leaf.pathname.rfind(".");
		if(dot_pos != std::string::npos)
			return path(leaf.pathname.substr(0,dot_pos));
		else
			return leaf;
	}

	std::string extension() const
	{
		auto dot_pos = pathname.rfind(".");
		if(dot_pos != std::string::npos)
			return pathname.substr(dot_pos);
		else
			return std::string();
	}

	path replace_extension(const path& replacement = path() )
	{
		auto dot_pos = pathname.rfind(".");
		std::string dest = replacement.pathname;

		if(0 < dest.length() && dest[0] != '.')
			dest = "." + dest;

		if(dot_pos != std::string::npos)
			pathname.replace(dot_pos, pathname.length() - dot_pos, dest);

		return *this;
	}
		
	std::string string() const
	{
		return this->pathname;
	}

	bool operator==(const path& rhs) const
	{
		return (this->pathname == rhs.pathname);
	}
};


inline int create_directory( const std::string& pathname ) {

	// mkdir under Windows MSVC only takes the path argument
#ifdef _MSC_VER
	int ret = _mkdir(pathname.c_str());
#else /* _MSC_VER */
	// Whereas everywhere else in the world, we have permission bits
	// Since I'm coding and compiling on VS, I'll come back and
	// work on this later
	mode_t mode_flags = S_IRUSR | S_IWUSR | S_IXUSR;
	int ret = mkdir(pathname.c_str(), mode_flags);
#endif /* _MSC_VER */

	return ret;
}


inline bool find_contiguous_slash_or_backslash_block(
	const std::string& pathname,
	size_t current_offset,
	size_t& separator_start,
	size_t& separator_end )
{
	std::regex myregex("[/\\\\]+");
	std::smatch results;
	std::string path_to_search = pathname.substr(current_offset);
	std::regex_search(path_to_search, results, myregex);

	if( 0 < results.size() )
	{
		separator_start = results.prefix().length();
		separator_end   = results.prefix().length() + results[0].length();

		return true;
	}
	else
		return false;
}


/**
TODO: Support permisson flags
*/
inline int create_directories( const path& pathname ) {

	const std::string& _pathname = pathname.string();
	size_t current_offset = 0;
	std::string dir_to_create;

	size_t separator_start = std::string::npos;
	size_t separator_end = std::string::npos;

	bool found = find_contiguous_slash_or_backslash_block(_pathname,current_offset,separator_start,separator_end);
	if(found && separator_start == 0) // The pathname starts with separator(s)
		current_offset = separator_end + 1;

	while(1) {
		// Example:
		// pathname[in] = abc//\\/de
		// separator_start[out] = 3
		// separator_end[out] = 7 (/ before 'd')
		separator_start = std::string::npos;
		separator_end = std::string::npos;
		found = find_contiguous_slash_or_backslash_block(_pathname,current_offset,separator_start,separator_end);

		if( found )
		{
			// Found a separator, or a block of separators

			// Get the substring that ends just before the separator character
			dir_to_create = _pathname.substr( 0, separator_start );

			int ret = create_directory( dir_to_create );

			current_offset = separator_end + 1;

			if( _pathname.length() <= current_offset )
				break; // pathname ends with a separator block; no more directories to create.
		}
		else
		{
			// No more slashes/backslashes found; create the leaf directory.
			dir_to_create = _pathname;

			// Remember that it is possible 'dir_to_create' is not a valid directory path.
			// For instance, the all the previous directory paths already exist and
			// dir_to_create is a file pathname in the innermost directory.
			create_directory( dir_to_create );

			break;
		}
	}

	return 0;
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
			if( chdir( path.c_str() ) == 0 )
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
		if( chdir( prev_path.c_str() ) == 0 )
		{
			m_vecDirectory.pop_back();	// delete the last path at the top of the stack
			return true;
		}
		else
			return false;
	}
};


} // namespace lfs

} // namespace amorphous



#endif  /*  __LFS_HPP__  */
