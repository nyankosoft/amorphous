#ifndef __amorphous_recursive_file_finder_HPP__
#define __amorphous_recursive_file_finder_HPP__


#include "recursive_directory_processor.hpp"


namespace amorphous
{


/// Recursively searches files and saves them to m_found_file_pathnames
/// NOTE: Skips directories named ".git" or ".svn" by default.
class recursive_file_finder : public recursive_directory_processor
{
public:

	recursive_file_finder()
		:
	m_ignore_git_directory(true),
	m_ignore_svn_directory(true)
	{
	}

	recursive_directory_processor::ret process_directory( const boost::filesystem::path & directory_path )
	{
		if( m_ignore_git_directory && directory_path.string().find( ".git" ) != std::string::npos
		 || m_ignore_svn_directory && directory_path.string().find( ".svn" ) != std::string::npos )
		{
			return recursive_directory_processor::STOP;
		}

		return recursive_directory_processor::CONTINUE;
	}

	recursive_directory_processor::ret process_file( const boost::filesystem::path & file_path )
	{
		const boost::filesystem::path ext = file_path.extension();

		bool should_be_included = false;

		if( m_include_extensions.empty() )
			should_be_included = true; // All the files are included if not include extensions are specified.
		else
		{
			for( size_t i=0; i<m_include_extensions.size(); i++ )
			{
				if( ext == m_include_extensions[i] )
				{
					should_be_included = true;
					break;
				}
			}
		}

		if( !should_be_included )
			return recursive_directory_processor::CONTINUE;

		if( m_exclude_extensions.empty() )
			should_be_included = true; // All the files are included if not exclude extensions are specified.
		else
		{
			for( size_t i=0; i<m_exclude_extensions.size(); i++ )
			{
				if( ext == m_exclude_extensions[i] )
				{
					should_be_included = false;
					break;
				}
			}
		}

		if( should_be_included )
		{
			m_found_file_pathnames.push_back( file_path );
		}

		return recursive_directory_processor::CONTINUE;
	}

	/// Stores the pathnames of the found files.
	std::vector<boost::filesystem::path> m_found_file_pathnames;

	/// Include "." when specifying extensions
	/// e.g. ".txt"
	std::vector<std::string> m_include_extensions;

	/// Include "." when specifying extensions
	/// e.g. ".txt"
	std::vector<std::string> m_exclude_extensions;

	bool m_ignore_git_directory; ///< true by default

	bool m_ignore_svn_directory; ///< true by default
};


} // amorphous


#endif /* __amorphous_recursive_file_finder_HPP__ */
