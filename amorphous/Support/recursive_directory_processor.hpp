#ifndef  __amorphous_recursive_directory_processor_HPP__
#define  __amorphous_recursive_directory_processor_HPP__


#include <string>
#include <vector>
#include <boost/filesystem.hpp>


namespace amorphous
{


class recursive_directory_processor
{
protected:

	enum ret
	{
		STOP,
		CONTINUE,
		NUM_RETURN_TYPES
	};

	std::vector<boost::filesystem::path> m_pathnames_unable_to_process;

	inline void process_r( const boost::filesystem::path pathname );

public:

	virtual ~recursive_directory_processor() {}

//	virtual recursive_directory_processor::ret process_directory( const boost::filesystem::path & directory_path ) {}
//	virtual recursive_directory_processor::ret process_file( const boost::filesystem::path & file_path ) {}

	virtual recursive_directory_processor::ret process_directory( const boost::filesystem::path & directory_path ) { return recursive_directory_processor::CONTINUE; }

	virtual recursive_directory_processor::ret process_file( const boost::filesystem::path & file_path ) { return recursive_directory_processor::CONTINUE; }

	virtual bool halt() { return false; }

	inline void process( const boost::filesystem::path pathname );

	const std::vector<boost::filesystem::path>& get_pathnames_unable_to_process() const { return m_pathnames_unable_to_process; }
};


inline void recursive_directory_processor::process_r( const boost::filesystem::path pathname )
{
	using namespace boost::filesystem;

	if ( !exists( pathname ) )
		return;

	directory_iterator end_itr; // default construction yields past-the-end
	boost::system::error_code ec;
	directory_iterator itr( pathname, ec );
	if( ec != 0 )
	{
		m_pathnames_unable_to_process.push_back( pathname );
		return;
	}

	for ( ; itr != end_itr; ++itr )
	{
		if ( is_directory(itr->status()) )
		{
			recursive_directory_processor::ret r
				= process_directory( itr->path() );

			if( r == recursive_directory_processor::CONTINUE )
				process_r( itr->path() );
		}
		else
		{
			process_file( itr->path() );
		}
	}
}


inline void recursive_directory_processor::process( const boost::filesystem::path pathname )
{
	m_pathnames_unable_to_process.clear();
	process_r( pathname );
}


} // amorphous


#endif /* __amorphous_recursive_directory_processor_HPP__ */
