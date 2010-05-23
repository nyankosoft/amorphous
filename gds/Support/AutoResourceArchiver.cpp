#include "AutoResourceArchiver.hpp"
#include "AutoResourceArchiver.hpp"
#include <boost/exception/get_error_info.hpp>

using namespace std;
using namespace boost;
using namespace boost::filesystem;

/*
class CClothSimArchiverUnit : public CResourceArchiverUnit
{
public:

	Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db ) { return Result::UNKNOWN_ERROR; }

	Result::Name CreateArchive( const std::string& input_filepath, boost::shared_ptr<IArchiveObjectBase>& pArchive )
	{
		return Result::UNKNOWN_ERROR;
	}
};
*/

void CAutoResourceArchiver::MainLoop( const std::string& desc_filepath )
{
	if( m_BuildInfo.m_CompileInfo.empty() )
		return;

	int resource_index = 0;
//	int num_compile_info = (int)m_BuildInfo.m_CompileInfo.size();
//	while(1)
	for(; !m_StopMonitoring; resource_index = (resource_index+1) % (int)m_BuildInfo.m_CompileInfo.size() )
	{
		CResourceCompileInfo& compile_info = m_BuildInfo.m_CompileInfo[resource_index];

		bool updated = false;
		map<string,CResourceFileInfo>::iterator itr = compile_info.m_mapInputFilepathToModTime.begin();
		for( ; itr != compile_info.m_mapInputFilepathToModTime.end(); itr ++ )
		{
			const std::string& input_filepath = itr->first;
//			time_t prev_mod_time = itr->second;
			time_t prev_mod_time = itr->second.m_LastModifiedTime;

			// Use lfs::path_exists instead of boost::filesystem::exists
			// rationale: boost::filesystem::exists throws an exception when a path points to a file
			// in an empty optical drive.
//			if( exists( input_filepath ) )
			if( lfs::path_exists( input_filepath ) )
			{
				time_t last_mod_time = lfs::get_last_modified_time( input_filepath );
				if( prev_mod_time < last_mod_time )
				{
					// Need to update the resource
					updated = true;
					itr->second.m_LastModifiedTime = last_mod_time;
				}
			}
		}

		if( updated )
		{
			printf( "Updating a resource file...\n" );
			compile_info.PrintInfo();
//			m_ResourceArchiver.CreateArchive( compile_info );
			CResourceArchiverUnitOutput out;
			Result::Name res = m_ResourceArchiver.CompileResource( compile_info, out );
			if( res == Result::SUCCESS )
			{
				printf( "Updated a resource file.\n" );
				map<string,string>::iterator itr;
				for( itr = out.m_ExtraTargets.begin();
					 itr != out.m_ExtraTargets.end();
					 itr++)
				{
					CResourceFileInfo *pInfo = m_BuildInfo.GetResourceFileInfo( itr->first );
					if( !pInfo )
					{
						m_BuildInfo.m_CompileInfo.push_back( CResourceCompileInfo(CResourceCompileInfo::TYPE_ARCHIVE) );
						m_BuildInfo.m_CompileInfo.back().m_mapInputFilepathToModTime[ itr->first ] = CResourceFileInfo();
						m_BuildInfo.m_CompileInfo.back().m_OutputPath = itr->second;
					}
				}

			}
			else
				printf( "Failed to update a resource file.\n" );
		}

		// sleep to avoid using too much CPU resources with the loop
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += 1; // 1 [sec]
		boost::thread::sleep(xt);
	}
}


void CAutoResourceArchiver::UpdateBuildInfo( const CResourceBuildInfo& last_build_info )
{
	const int num_elements = (int)m_BuildInfo.m_CompileInfo.size();
	for( int i=0; i<num_elements; i++ )
	{
		CResourceCompileInfo& compile_info = m_BuildInfo.m_CompileInfo[i];
//		const int num_input_files = (int)compile_info.m_mapInputFilepathToModTime.size();
//		m_mapInputFilepathToModTime.begin();
//		for( int j=0; j<num_input_files; j++ )
		map<string,CResourceFileInfo>::iterator itr = compile_info.m_mapInputFilepathToModTime.begin();
		for( ; itr != compile_info.m_mapInputFilepathToModTime.end(); itr++ )
		{
			const std::string& input_filepath = itr->first;

			itr->second.m_LastModifiedTime = last_build_info.GetLastCompiledTime( input_filepath );
		}
	}
}


void CAutoResourceArchiver::Init( const std::string& resource_make_file )
{
	using namespace std;

//	m_InputPathToCompileInfo;
	Result::Name res = m_ResourceArchiver.LoadCompileInfo( resource_make_file, m_BuildInfo );
	if( res != Result::SUCCESS || m_BuildInfo.m_CompileInfo.empty() )
	{
		return;
	}

	m_RootDescFilepath = resource_make_file;

	// load the last build info
	string lbi_filepath = resource_make_file + ".lbi";

	CResourceBuildInfo last_build_info;
	bool loaded = last_build_info.LoadFromFile( lbi_filepath );
	if( loaded )
	{
		UpdateBuildInfo( last_build_info );
	}
}


void CAutoResourceArchiver::SaveBuildInfoToDisk()
{
	if( m_RootDescFilepath.length() == 0 )
		return;

	m_BuildInfo.SaveToFile( m_RootDescFilepath + ".lbi" );
}

void CAutoResourceArchiver::run()
{
	try
	{
		MainLoop( m_RootDescFilepath );
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_WARNING, "exception: %s", e.what() );
	}
	catch( boost::exception& e )
	{
		// compiled on VisualC++ 2005 Express, error on GCC 4.1.1
#ifdef _MSC_VER
		shared_ptr<throw_line::value_type const> pLine         = get_error_info<throw_line>(e);
		shared_ptr<throw_file::value_type const> pFile         = get_error_info<throw_file>(e);
		shared_ptr<throw_function::value_type const> pFunction = get_error_info<throw_function>(e);
		int line             = *pLine.get();
		const char *file     = *pFile.get();
		const char *function = *pFunction.get();
		g_Log.Print( WL_WARNING, "boost::exception: at %s (%s, L%d)\n", function, file, line );
#else
		// compiled on GCC 4.1.1, error on VisualC++ 2005 Express
		const int * const line      = get_error_info<throw_line>(e);
		const char * const* file     = get_error_info<throw_file>(e);
		const char * const* function = get_error_info<throw_function>(e);
		printf( "exception: at %s (%s, L%d)\n", *function, *file, *line );
#endif /* _MSC_VER */
	}

}
