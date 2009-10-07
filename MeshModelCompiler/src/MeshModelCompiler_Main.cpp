//-----------------------------------------------------------------------------
// File: MeshModelCompiler_Main.cpp
//-----------------------------------------------------------------------------

#include <boost/progress.hpp>
#include "Support/FileOpenDialog_Win32.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/progress_display.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/lfs.hpp"
#include "Support/thread_starter.hpp"
#include "Support/Timer.hpp"

#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "LightWave/3DMeshModelExportManager_LW.hpp"

//#include <vld.h>


/// task to compiler mesh model from LWO2 (LightWave object) file
class CLWO2MeshModelCompilerTask : public thread_class
{
public:

	C3DMeshModelExportManager_LW m_Exporter;

	bool m_CompilationFinished;

	std::string m_TargetFilepath;

public:

	CLWO2MeshModelCompilerTask( const std::string& target_filepath )
		:
	m_TargetFilepath(target_filepath),
	m_CompilationFinished(false)
	{}

	void run()
	{
		m_Exporter.BuildMeshModels( m_TargetFilepath );

		m_CompilationFinished = true;
	}
};


int main( int argc, char *argv[] )
{
	string init_wd( lfs::get_cwd() );
	if( init_wd.find( "\\app" ) != init_wd.length() - 4 )
		lfs::set_wd( "../app" );

	string src_filepath;
	if( 2 <= argc )
	{
		src_filepath = argv[1];
	}
	else
	{
		GetFilename( src_filepath );
	}

	if( src_filepath.length() == 0 )
		return 0;

	CLWO2MeshModelCompilerTask mesh_compiler(src_filepath);

	mesh_compiler.start_thread();

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);

//	const shared_ptr<morph::progress_display> pLoadingProgress;
//	while( !pLoadingProgress )
//		pLoadingProgress = mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress();

	// wait until the progress display is ready
	while( mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress().get_total_units() <= 1 )
	{
		if( mesh_compiler.m_CompilationFinished )
			return 0;
	}

	const morph::progress_display& loading_progress = mesh_compiler.m_Exporter.GetSourceObjectLoadingProgress();
	boost::progress_display progress_bar( loading_progress.get_total_units() );
	int last_units = 0;//pLoadingProgress->get_num_current_units();
	int added = 0;

	while( 1 )
	{
//		printf( "checking progress: %d\n", loading_progress.get_current_units() );

		int current_units = loading_progress.get_current_units();
		added += (current_units - last_units);
		if( last_units < current_units )
			progress_bar += (current_units - last_units);
		last_units = current_units;

		if( mesh_compiler.m_CompilationFinished
		 && loading_progress.get_total_units() <= loading_progress.get_current_units() )
		{
			break;
		}

		xt.sec += 1; // 1 [sec]
		boost::thread::sleep(xt);

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
}
