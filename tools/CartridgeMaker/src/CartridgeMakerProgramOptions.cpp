#include "CartridgeMakerProgramOptions.hpp"
#include "CartridgeMaker.hpp"
#include "BuiltinCartridges.hpp"
#include "../GameCommon/Caliber.hpp"
#include "../Support/Log/DefaultLog.hpp"
#include "../Support/wavefront_obj_output.hpp"
#include <iostream>

using namespace std;
namespace po = boost::program_options;


void AddProgramOptionsToDesc( po::options_description& desc )
{
	desc.add_options()
		("my_command", "produce message")
		("option1", "")
		("option2", "")
		("case",               po::value<string>(), "" )
		("output",             po::value<string>(), "" )
		("num_sides",          po::value<string>(), "" )
		("descfile",           po::value<string>(), "" )
	;
}


Caliber::Name GetCartridgeIDfromName( const string& caliber_name )
{
	for( int i=0; i<Caliber::NUM_CALIBERS; i++ )
	{
		Caliber::Name cal = (Caliber::Name)i;

		if( caliber_name == GetCaliberName(cal) )
			return cal;
		else if( caliber_name == GetAbbreviatedCaliberName(cal) )
			return cal;
	}

	return Caliber::OTHER;
}


bool ParseProgramOptions( po::options_description& src_desc, po::variables_map& vm, firearm::CartridgeDesc& desc )
{
	LOG_PRINTF_VERBOSE(( "Parsing program options..." ));

//	if( vm.count("help") )
//	{
//		cout << desc << "\n";
//		cout << "Usage example 1: using arguments\n";
//		cout << "-------------------------------------------\n";
//		cout << "Recursively extract functions, classes and structs from source files in /projects/name/source. Target files with extensions c, cpp, cxx, h, hpp, or inl. Save the results in /projects/analysis_results/ Use multithreading to expedite the analysis.\n";
//		cout << "\n";
//		cout << "FunctionExtractor --input=/projects/name/source --strip-from-input=/projects/ --output=/projects/analysis_results/ --include-extensions=\"c cpp cxx h hpp inl\" --multithread\n";
//		return false;
//	}

	if( vm.count("case") )
	{
		Caliber::Name cal_id = GetCartridgeIDfromName( vm["case"].as<string>() );
		if( cal_id == Caliber::OTHER )
			return false;
		else
		{
			bool res = GetCaseDesc( cal_id, desc.case_desc );
			if( !res )
				return false;
		}
	}

	string output_model_pathname;
	if( vm.count("output") )
		output_model_pathname               =  vm["output"].as<string>();
	else
		output_model_pathname               =  "out.obj";

	if( vm.count("num_sides") )
		desc.case_desc.num_sides = vm["num_sides"].as<int>();

	return true;
}


int run( int argc, char *argv[] )
{
	firearm::CartridgeDesc cartridge_desc;

	po::options_description desc("Allowed options");
	po::variables_map vm;
//	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	ParseProgramOptions( desc, vm, cartridge_desc );

	vector<Vector3> positions, normals;
	vector<TEXCOORD2> tex_coords;
	vector< vector<int> > polygons;

	unsigned int num_sides = cartridge_desc.num_sides;

	firearm::CartridgeMaker cm;
	Result::Name res = cm.Make( cartridge_desc, num_sides, positions, normals, polygons );

//	C3DMeshModelArchive dest_mesh;

//	CreateMeshArchive(
//		positions,
//		normals,
//		vector<SFloatRGBAColor>(),
//		vector<TEXCOORD2>(),
//		polygons
//		dest_mesh
//		);

	wavefront_obj::write_to_obj_file( output_pathname, positions, tex_coords, normals, polygons );

	return 0;
}
