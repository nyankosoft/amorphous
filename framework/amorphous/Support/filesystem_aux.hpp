#ifndef __filesystem_aux_HPP__
#define __filesystem_aux_HPP__


#include <vector>
#include <experimental/filesystem>


namespace amorphous
{


inline void find_files_in_directory( const std::string& directory_path, std::vector<std::string>& found_files )
{
	using namespace std::experimental::filesystem;

	path p(directory_path);   // p reads clearer than argv[1] in the following code

	try
	{
		if(exists(p))    // does p actually exist?
		{
			if( is_regular_file(p) )        // is p a regular file?
				return;
//				cout << p << " size is " << file_size(p) << '\n';

			else if(is_directory(p))      // is p a directory?
			{
//				cout << p << " is a directory containing:\n";

				for( directory_iterator itr = directory_iterator(p);
					itr != directory_iterator();
					itr++ )
				{
					const path found_pathname = itr->path();
					if( is_regular_file(found_pathname) )
						found_files.push_back( found_pathname.string() );
				}


//				copy(directory_iterator(p), directory_iterator(), // directory_iterator::value_type
//					ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
				// converted to a path by the
				// path stream inserter
			}

			else
				return;
//				cout << p << " exists, but is neither a regular file nor a directory\n";
		}
		else
			return;
//			cout << p << " does not exist\n";
	}

	catch (const filesystem_error& ex)
	{
		return;
//		cout << ex.what() << '\n';
	}

	return;
}


} // amorphous



#endif /* __filesystem_aux_HPP__ */
