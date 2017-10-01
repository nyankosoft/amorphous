#include "amorphous/Support/ImageSplitter.hpp"
#include "amorphous/Support/lfs.hpp"
#include "tbb/task_scheduler_init.h"

using namespace std;
using namespace tbb;
using namespace amorphous;


void InitGlobalTextLog( const std::string& output_text_log_pathname )
{
}


int test_ImageSplitter(int argc, char *argv[])
{
	tbb::task_scheduler_init tbb_init;

	// Go up the directory if run from the IDE
	if( !lfs::path_exists( "images" ) )
		lfs::set_wd( "../" );

//	string image_filepath = "images/input/test30_tex02b.bmp";
	string image_filepath = "images/input/color_table_4x4.png";
	const int num_edge_splits = 4;

//	string image_filepath = "data/images/test30_tex03b.bmp";
//	const int num_edge_splits = 8;

	std::shared_ptr<BitmapImage> pSrcImg( new BitmapImage() );
	bool loaded = pSrcImg->LoadFromFile( image_filepath );

	StdSplitImageFilepathPrinter dest_filepath_printer(image_filepath);

	ImageSplitter is(
		num_edge_splits,
		num_edge_splits,
		image_filepath,
		pSrcImg,
		&dest_filepath_printer );

	is.SplitMT();

	return 0;
}
