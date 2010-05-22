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

//	Result::Name CreateArchive( const std::string& input_filepath, boost::shared_ptr<IArchiveObjectBase>& pArchive, CResourceArchiverUnitOutput& out )
	Result::Name CreateArchive( const std::string& input_filepath, const std::string& output_filepath, CResourceArchiverUnitOutput& out )
	{
		if( path(input_filepath).extension() != ".lwo" )
			return Result::INVALID_ARGS;

		C3DMeshModelExportManager_LW lw_mesh_exporter;
		lw_mesh_exporter.BuildMeshModels( input_filepath,
			  C3DMeshModelBuilder::BOF_OUTPUT_AS_TEXTFILE
			| C3DMeshModelBuilder::BOF_CHANGE_TEXTURE_FILE_EXTENSIONS_TO_IA );

		const int num_built_models = lw_mesh_exporter.GetNumBuiltMeshModelArchives();
		for( int i=0; i<num_built_models; i++ )
		{
			const vector< vector<string> > orig_tex_paths = lw_mesh_exporter.GetOriginalTextureFilepaths( i );

			C3DMeshModelArchive* pArchive = lw_mesh_exporter.GetBuiltMeshModelArchive( i );
			if( !pArchive )
				continue;

			const size_t num_materials = pArchive->GetMaterial().size();
			for( size_t mat=0; mat<num_materials; mat++ )
			{
				const CMMA_Material& material = pArchive->GetMaterial()[mat];
				const size_t num_textures = material.vecTexture.size();
				for( size_t tex=0; tex<num_textures; tex++ )
				{
					if( material.vecTexture[tex].type != CMMA_Texture::FILENAME )
						continue;

					path mesh_output_path = lw_mesh_exporter.GetOutputFilepath( i );

					const path output_path = path(input_filepath).parent_path() / mesh_output_path.parent_path() / material.vecTexture[tex].strFilename;
//					const path output_path = path(output_filepath).parent_path() / material.vecTexture[tex].strFilename;
					out.m_ExtraTargets[ orig_tex_paths[mat][tex] ] = output_path.string();
				}
			}
		}

		return (0 < num_built_models) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
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
	string root_desc_file;
	if( 1 < argc )
	{
		root_desc_file = argv[1];
	}
	else
	{
		bool res = GetFilename( root_desc_file );
	}

	if( root_desc_file.length() == 0 )
		return 0;

	// <<< release

	// >>> test
//	string root_desc_file( "../../../GameProjects/FlightGame/resources/images/ss_imgs" );
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
		{
			ara.StopMonitoring();
			ara.join();
			break;
		}
	}

	ara.SaveBuildInfoToDisk();

	return 0;
}
