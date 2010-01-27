#include "LWSMotionCompiler.hpp"
#include "MotionSynthesis/BVHMotionDatabaseCompiler.hpp"
#include "XML/XMLDocumentLoader.hpp"
#include "gds/base.hpp"
#include "gds/Support/lfs.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"


using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace msynth;






/*
bool ends_with( const std::string& target, const std::string& end )
{
	if( target.substr( target.length() - end.length(), end.length() );
}
*/

int main( int argc, char *argv[] )
{
	using namespace filesystem;

	path init_wd = lfs::get_cwd();

	if( init_wd.leaf() != "app" )
		lfs::set_wd( "../app" );

	path app_wd = lfs::get_cwd();

	string filepath;
	if( 2 <= argc )
		filepath  = argv[1];
	else
	{
		GetFilename( filepath );
	}

	if( filepath.length() == 0 )
		return 0;

	// Open a file for logging
	path dirpath = path(filepath).parent_path();
	path html_log_filepath = dirpath / "log.html";
	CLogOutput_HTML html_log( html_log_filepath.string() );
	g_Log.AddLogOutput( &html_log );

	shared_ptr<CMotionPrimitiveCompilerCreator> pBVHCompilerCreator( new CBVHMotionPrimitiveCompilerCreator );
	RegisterMotionPrimitiveCompilerCreator( pBVHCompilerCreator );

	shared_ptr<CMotionPrimitiveCompilerCreator> pLWSCompilerCreator( new CLWSMotionPrimitiveCompilerCreator );
	RegisterMotionPrimitiveCompilerCreator( pLWSCompilerCreator );

	/// init the xml parser (calls Initialize() in ctor)
	CXMLParserInitReleaseManager xml_parser_mgr;

	msynth::CMotionDatabaseBuilder mdb;
	mdb.Build( filepath );

//	CLWSMotionCompiler test;
//	test.BuildFromDescFile( filepath );

	return 0;
}
