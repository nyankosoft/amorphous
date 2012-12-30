#include "ScriptArchive.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/TextFileScanner.hpp"
#include "Support/lfs.hpp"

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"


namespace amorphous
{

using namespace std;
using namespace boost::filesystem;


static void CreateScriptFileListFromListFile( path script_dir_path,
						                      vector<string>& script_file_list )
{
	CTextFileScanner scanner( script_dir_path.string() + "/list" );
	if( !scanner.IsReady() )
	{
		LOG_PRINT_ERROR( "Invalid directory path: " + script_dir_path.string() );
		return;
	}

	string line;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.GetCurrentLine( line );

		// skip the rest of the string if '#' is found in the line
		size_t comment_start_pos = line.find( "#" );
		if( comment_start_pos != string::npos )
			line = line.substr( 0, comment_start_pos );

		// remove the newline char
		size_t nl_pos = line.find( "\n" );
		if( nl_pos != string::npos )
			line = line.substr( 0, nl_pos );

		string script_filepath = script_dir_path.string() + "/" + line;

		if( exists(script_filepath)
		 && path(script_filepath).extension() == ".py" )
		{
			script_file_list.push_back( script_filepath );
		}
	}
}


static void CreateScriptFileListBySearchingDirectory( path script_dir_path,
													  vector<string>& script_file_list )
{
	directory_iterator end_itr;
	for ( directory_iterator script_itr( script_dir_path );
	      script_itr != end_itr;
	      ++script_itr )
	{
		const path pathname = (*script_itr).path();

		if( is_directory( pathname ) )
			continue;

		// load the python script files
		if( lfs::get_ext(pathname.string()) != "py" )
			continue;

		script_file_list.push_back( pathname.string() );

//		MsgBox( string("script file: ") + (*script_itr).string() );
	}

}


bool g_LoadScriptFilesFromListFile = true;

/**
 * \param dir_path root directory for script directory
 */
void UpdateScriptArchives( const std::string & src_dir_path,
						   const std::string & output_dir_path )
{
	CScriptArchive script_archive;
	vector<string> script_file_list;

	directory_iterator end_itr;
	for ( directory_iterator itr( src_dir_path );
	      itr != end_itr;
	      ++itr )
	{
		// iterate sub-directories under the root script directory

		if ( !is_directory( *itr ) )
			continue;

		if( (*itr).path().string().find(".svn") != string::npos )
			continue;

		path script_dir_path = *itr;

//		MsgBox( string("script_dir_path: ") + script_dir_path.string() );

		// found a directory that is supposed contain a set of scripts for a stage

		script_file_list.resize( 0 );

		if( g_LoadScriptFilesFromListFile )
		{
			// load the script filepaths from the simple text file with the name 'list'
			// in each subdirectory.
			CreateScriptFileListFromListFile( script_dir_path, script_file_list );
		}
		else
		{
			// search each subdirectory and collect all files under it (non-recursive).
			CreateScriptFileListBySearchingDirectory( script_dir_path, script_file_list );
		}

		// create an archive of script files and save it to disk

		script_archive.m_vecBuffer.resize( 0 );
		script_archive.m_vecSourceFilename.resize( 0 );
		for( size_t i=0; i<script_file_list.size(); i++ )
		{
			// load script file
			script_archive.m_vecBuffer.push_back( CSerializableStream() );
			script_archive.m_vecBuffer.back().LoadTextFile( script_file_list[i] );

			// save the filepath to let the system reload scripts at runtime for debugging
			script_archive.m_vecSourceFilename.push_back( script_file_list[i] );
		}

		if( 0 < script_archive.m_vecBuffer.size() )
		{
			string output_filename = output_dir_path + script_dir_path.leaf().string() + ".bin";
//			MsgBox( string("output archive file: ") + output_filename );
			script_archive.SaveToFile( output_filename );
		}
		else
			LOG_PRINT( "no script file found in " + script_dir_path.string() );
	}
}


} // namespace amorphous
