//-----------------------------------------------------------------------------
// File: MeshModelCompiler_Main.cpp
//-----------------------------------------------------------------------------

// Commented out because boost/progress is incompatible with boost/timer usd by LogInputBase.
//#include <boost/progress.hpp>

//#include <filesystem> Have to wait for the C++17 support
#include <chrono>
#include <thread>
#include "amorphous/Support/FileOpenDialog_Win32.hpp"
#include "amorphous/Support/progress_display.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/Log/LogOutput.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/thread_starter.hpp"
#include "amorphous/Support/ParamLoader.hpp"

#include "amorphous/Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "amorphous/LightWave/3DMeshModelExportManager_LW.hpp"
#include "amorphous/LightWave/3DShapesExporter_LW.hpp"

//#include <vld.h>


using namespace std;
using namespace amorphous;


/// task to compiler mesh model from LWO2 (LightWave object) file
class CLWO2MeshModelCompilerTask : public thread_class
{
public:

	C3DMeshModelExportManager_LW m_Exporter;

	C3DShapesExporter_LW m_ShapesExporter;

	bool m_CompilationFinished;

	bool m_ObfuscateTexture;

	std::string m_TargetFilepath;

public:

	CLWO2MeshModelCompilerTask( const std::string& target_filepath, bool obfuscate_texture )
		:
	m_TargetFilepath(target_filepath),
	m_ObfuscateTexture(obfuscate_texture),
	m_CompilationFinished(false)
	{}

	bool MakeShapesFile()
	{
		// Decide on the filename of the shapes file
		vector<string> shape_files;
		for( uint i=0; i<m_Exporter.GetNumOutputFilepaths(); i++ )
		{
			string shapes_file = m_Exporter.GetOutputFilepath(i);

			if( shapes_file.length() == 0 )
				shapes_file = m_TargetFilepath;

			if( shapes_file.length() == 0 )
				continue;

			if( shapes_file.rfind( "." ) == string::npos )
				continue;

			lfs::change_ext( shapes_file, "sd" );

			if( m_TargetFilepath == shapes_file )
				continue; // Avoid overwriting the source model file.

			shape_files.push_back( shapes_file );
		}

		// Extract shapes and save to disk
		return m_ShapesExporter.ExtractShapes( m_Exporter.GetLWO2Object(), shape_files );
	}

	void run()
	{
		U32 build_option_flags = C3DMeshModelBuilder::BOF_OUTPUT_AS_TEXTFILE;

		// Ignored if the file named "params.txt" does not exist
		if( lfs::path_exists( "params.txt" ) )
		{
			// Save the texture as image archive files or not (false by default).
			int save_textures_as_ia = 0;
			LoadParamFromFile( "params.txt", "SAVE_TEXTURES_AS_IMAGE_ARCHIVES", save_textures_as_ia );

			m_ObfuscateTexture = save_textures_as_ia ? true :false;
		}

		if( m_ObfuscateTexture )
			build_option_flags |= C3DMeshModelBuilder::BOF_SAVE_TEXTURES_AS_IMAGE_ARCHIVES;

		// Build mesh archive(s) and save to disk
		m_Exporter.BuildMeshModels( m_TargetFilepath, build_option_flags );

		bool res = MakeShapesFile();

		m_CompilationFinished = true;
	}
};


int main( int argc, char *argv[] )
{
	const string init_wd( lfs::get_cwd() );

	if( init_wd.find( "\\app" ) != init_wd.length() - 4
	 && init_wd.find( "/app" )  != init_wd.length() - 4 )
	{
		lfs::set_wd( "../../app" );
	}

	bool obfuscate_texture = true;
	string src_filepath;
	if( 2 <= argc )
	{
		// Take the first command argument as the pathname of the input model file
		// Commented out in order to support the -b (obfuscate texture) option.
//		src_filepath = argv[1];

		for( int i=1; i<argc; i++ )
		{
			if( string(argv[i]) == "-b" )
				obfuscate_texture = true;
			else
			{
				// Anything other than "-b" is interpreted as a pathname of the input model file.
				src_filepath = argv[i];
			}
		}

		//using namespace boost::filesystem;
		lfs::path src_directory_path = lfs::path(src_filepath).parent_path();
		if( 0 < src_directory_path.string().length() )
			lfs::set_wd( src_directory_path.string() );
	}
	else
	{
		GetFilename( src_filepath );
	}

	// Open a file for logging

	lfs::path logfile_dirpath = lfs::path(src_filepath).parent_path();
	lfs::path logfile_basepath = logfile_dirpath / ("mesh_model_compiler-" + lfs::path(src_filepath).leaf().string());

	InitHTMLLog( logfile_basepath.string() + ".html" );

	LOG_PRINT( "cwd: " + lfs::get_cwd() );

	if( src_filepath.length() == 0 )
		return 0;

	CLWO2MeshModelCompilerTask mesh_compiler(src_filepath,obfuscate_texture);

	mesh_compiler.start_thread();

//	const shared_ptr<morph::progress_display> pLoadingProgress;
//	while( !pLoadingProgress )
//		pLoadingProgress = mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress();

	// wait until the progress display is ready
	while( mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress().get_total_units() <= 1 )
	{
		if( mesh_compiler.m_CompilationFinished )
			return 0;
	}

	const progress_display& loading_progress = mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress();
//	boost::progress_display progress_bar( loading_progress.get_total_units() );
	int last_units = 0;//pLoadingProgress->get_num_current_units();
	int added = 0;

	while( 1 )
	{
//		printf( "checking progress: %d\n", loading_progress.get_current_units() );

		int current_units = loading_progress.get_current_units();
		added += (current_units - last_units);
//		if( last_units < current_units )
//			progress_bar += (current_units - last_units);
		last_units = current_units;

		if( mesh_compiler.m_CompilationFinished
		 && loading_progress.get_total_units() <= loading_progress.get_current_units() )
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));

//		printf( "checking progress - done.\n" );
	}

//	printf( "progress: %d / %d\n", loading_progress.get_current_units(), loading_progress.get_total_units() );

	mesh_compiler.join();

/*
	// load a LightWave model and export mesh file(s)
	C3DMeshModelExportManager_LW exporter;
	bool mesh_compiled = exporter.BuildMeshModels( src_filepath );

	if( !mesh_compiled )
		LOG_PRINT_ERROR( "Failed to compile mesh" );
*/

	return 0;
}
