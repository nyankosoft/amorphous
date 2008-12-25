#include "ScriptArchive.h"
#include <iostream>

#include "Support/fnop.h"
#include "Support/Log/DefaultLog.h"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;
using namespace fs;
using namespace std;


/**
 * \param dir_path root directory for script directory
 */
void UpdateScriptArchives( const path & dir_path,
						   const std::string & output_dir_path )
{
	vector<string> script_files;

	CScriptArchive script_archive;

	directory_iterator end_itr;
	for ( directory_iterator itr( dir_path );
	      itr != end_itr;
	      ++itr )
	{
		// iterate sub-directories under the root script directory

		if ( !is_directory( *itr ) )
			continue;

		if( (*itr).string().find(".svn") != string::npos )
			continue;

//		MsgBox( string("script_dir_path: ") + (*itr).string() );

		// found a directory for a script set
		script_archive.m_vecBuffer.resize(0);
		script_archive.m_vecSourceFilename.resize(0);
		path script_dir_path = *itr;
		for ( directory_iterator script_itr( script_dir_path );
		      script_itr != end_itr;
		      ++script_itr )
		{
			if ( is_directory( *script_itr ) )
				continue;

			// load the python script files
			if( fnop::get_ext((*script_itr).string()) != "py" )
				continue;

//			MsgBox( string("script file: ") + (*script_itr).string() );

			// load script file
			script_archive.m_vecBuffer.push_back( CSerializableStream() );
			script_archive.m_vecBuffer.back().LoadTextFile( (*script_itr).string() );

			script_archive.m_vecSourceFilename.push_back( (*script_itr).string() );
		}

		if( 0 < script_archive.m_vecBuffer.size() )
		{
			string output_filename = output_dir_path + script_dir_path.leaf() + ".bin";
//			MsgBox( string("output archive file: ") + output_filename );
			script_archive.SaveToFile( output_filename );
		}
		else
			g_Log.Print( "no script file found in '%s'", script_dir_path.string().c_str() );
	}
}
