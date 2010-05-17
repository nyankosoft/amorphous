#include "gds/Support/AutoResourceArchiver.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"
#include "gds/LightWave.hpp"

using namespace std;
using namespace boost;
using namespace boost::filesystem;


class CMeshArchiverUnit : public CResourceArchiverUnit
{
public:

	Result::Name AddFileToDB( const std::string& keyname, const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db ) { return Result::UNKNOWN_ERROR; }
/*	Result::Name AddFileToDB( const boost::filesystem::path& input_path, CBinaryDatabase<std::string>& db )
	{
		using namespace std;
		using namespace MeshModel;

		string ext = input_path.extension();

		if( ext == ".lwo" )
		{
			int num_meshes = 0;
			for( int i=0; i<num_meshes; i++ )
				db.AddData( mesh_archive );
		}
	}*/

	Result::Name CreateArchive( const std::string& input_filepath, boost::shared_ptr<IArchiveObjectBase>& pArchive )
	{
		if( path(input_filepath).extension() != ".lwo" )
			return Result::INVALID_ARGS;

		C3DMeshModelExportManager_LW lw_mesh_exporter;
		lw_mesh_exporter.BuildMeshModels( input_filepath );

		return Result::UNKNOWN_ERROR;
	}
};


void Init( CAutoResourceArchiver& ara )
{
	shared_ptr<CResourceArchiverUnit> pUnit;

	pUnit.reset( new CImageArchiverUnit );
	ara.ResourceArchiver().AddArchiverUnit( pUnit );

	pUnit.reset( new CMeshArchiverUnit );
	ara.ResourceArchiver().AddArchiverUnit( pUnit );

//	pUnit.reset( new CMeshArchiverUnit );
//	ara.ResourceArchiver().AddArchiverUnit( pUnit );
}


int main( int argc, char *argv[] )
{
	// >>> release
//	if( argc <= 1 )
//		return 0;

//	const string root_desc_file( argv[1] );
	// <<< release

	// >>> test
	const string root_desc_file( "../../../GameProjects/FlightGame/resources/images/ss_imgs" );
	// <<< test

	CAutoResourceArchiver ara;

	Init( ara );

	ara.Init( root_desc_file );

	ara.StartMonitoring();

	string str;
	while( 1 )
	{
		getline (cin,str);
		if( str == "quit" )
			break;
	}

	ara.SaveBuildInfoToDisk();

	return 0;
}
