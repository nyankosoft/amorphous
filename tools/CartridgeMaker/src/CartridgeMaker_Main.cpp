//#include <boost/progress.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/wavefront_obj_output.hpp"
#include "amorphous/Support/MiscAux.hpp"
#include "amorphous/Utilities/CartridgeMaker.hpp"
#include "amorphous/Utilities/BuiltinCartridges.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "amorphous/GameCommon/Caliber.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace amorphous;


enum ModelType
{
	MT_CASE,
	MT_BULLET,
	MT_CARTRIDGE,
	NUM_MODEL_TYPES
};


// declaration
Result::Name CreateMesh(
	const std::vector<Vector3>& points,
	const std::vector<Vector3>& normals,
	const std::vector< std::vector<int> >& polygons,
	General3DMesh& dest_mesh
	);


// definition
Result::Name CreateMesh(
	const std::vector<Vector3>& points,
	const std::vector<Vector3>& normals,
	const std::vector< std::vector<int> >& polygons,
	General3DMesh& dest_mesh
	)
{
	if( points.size() != normals.size() )
		return Result::INVALID_ARGS;

	boost::shared_ptr< std::vector<General3DVertex> > pVB = dest_mesh.GetVertexBuffer();
	if( !pVB )
		return Result::UNKNOWN_ERROR;

	vector<General3DVertex>& vertices = *pVB;
	vertices.resize( points.size() );
	for( size_t i=0; i<points.size(); i++ )
	{
		vertices[i].m_vPosition = points[i];
		vertices[i].m_vNormal   = normals[i];
	}

	dest_mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
//		|CMMA_VertexSet::VF_2D_TEXCOORD0
		);

	vector<IndexedPolygon>& polygon_buffer = dest_mesh.GetPolygonBuffer();

	dest_mesh.SetPolygons( polygons );

	// Recalculate the planes.
	dest_mesh.UpdatePolygonBuffer();

//	desh_mesh.ResetVertexNormalsWithPlaneNormals();

	vector<CMMA_Material>& materials = dest_mesh.GetMaterialBuffer();

	materials.resize( 1 );

	return Result::SUCCESS;
}


Result::Name MakeCase(
	const firearm::CaseDesc& src_desc,
	unsigned int num_sides,
	General3DMesh& dest_mesh
	)
{
	vector<Vector3> points, normals;
	vector<TEXCOORD2> tex_uvs;
	vector< vector<int> > polygons;

	points.reserve( num_sides * 10 );
	normals.reserve( num_sides * 10 );
	tex_uvs.reserve( num_sides * 10 );
	polygons.reserve( num_sides * 10 );

	firearm::CartridgeMaker maker;
	maker.MakeCase( src_desc, num_sides, points, normals, tex_uvs, polygons );

	return CreateMesh( points, normals, polygons, dest_mesh );
}


Result::Name MakeBullet( const firearm::BulletDesc& src_desc, unsigned int num_sides, float case_top_height, General3DMesh& dest_mesh )
{
	vector<Vector3> points, normals;
	vector<TEXCOORD2> tex_uvs;
	vector< vector<int> > polygons;

	points.reserve( num_sides * 10 );
	normals.reserve( num_sides * 10 );
	tex_uvs.reserve( num_sides * 10 );
	polygons.reserve( num_sides * 10 );

	firearm::CartridgeMaker maker;
	maker.MakeBullet( src_desc, num_sides, case_top_height, points, normals, tex_uvs, polygons );

	return CreateMesh( points, normals, polygons, dest_mesh );
}


Result::Name Make( const firearm::CartridgeDesc& src_desc, General3DMesh& dest_mesh )
{
	vector<Vector3> points, normals;
	vector<TEXCOORD2> tex_uvs;
	vector< vector<int> > polygons;

	points.reserve( src_desc.num_sides * 15 );
	normals.reserve( src_desc.num_sides * 15 );
	tex_uvs.reserve( src_desc.num_sides * 15 );
	polygons.reserve( src_desc.num_sides * 15 );

	unsigned int num_sides = src_desc.num_sides;

	firearm::CartridgeMaker maker;
	maker.MakeCase( src_desc.case_desc, num_sides, points, normals, tex_uvs, polygons );

	int num_bullet_sides
		= (src_desc.bullet_desc.num_sides == 0) ? src_desc.num_sides : src_desc.bullet_desc.num_sides;

	float case_top_height = 0;
	maker.MakeBullet( src_desc.bullet_desc, num_bullet_sides, case_top_height, points, normals, tex_uvs, polygons );

	return CreateMesh( points, normals, polygons, dest_mesh );
}

static Caliber::Name sg_cases_to_create[] =
{
	// handgun cartridges
	Caliber::_9X18MM_MAKAROV,
	Caliber::_9MM,
	Caliber::_40_SW,
	Caliber::_10MM_AUTO,
	Caliber::_45_ACP,
	Caliber::_500_SW_MAGNUM,
	Caliber::_7_62X25MM_TOKAREV,
	Caliber::_357_SIG,

	// rifle cartridges
	Caliber::HK_4_6X30,
	Caliber::_5_7X28,
	Caliber::_5_56X45,
	Caliber::_6_5MM_CREEDMOOR,
	Caliber::_5_45X39,
	Caliber::_7_62X39,
	Caliber::_7_62X54R,
	Caliber::_30_06_SPRINGFIELD,
	Caliber::_338_LAPUA_MAGNUM,
};


void Create(
	firearm::CartridgeDesc& src_cartridge_desc,
	const string& output_model_basename,
	ModelType model_type
//	bool create_case,
//	const std::string& case_suffix,
//	bool create_bullet,
//	const std::string& bullet_suffix,
//	bool create_cartridge
	)
{
	vector<Vector3> points, normals;
	vector<SFloatRGBAColor> diffuse_colors;
	vector< vector<int> > polygons;
	vector<TEXCOORD2> tex_uvs;
//	firearm::CaseDesc cd;// = ref_case_desc; firearm::GetCaseDesc() overwrites all the fields
//	bool obtained = firearm::GetCaseDesc( sg_cases_to_create[i], cd );
//	if( !obtained )
//		return;

//	cd.num_sides    = src_case_desc.num_sides;
//	cd.primer_model = src_case_desc.primer_model;

//	const string caliber_name = std::string(GetCaliberName(sg_cases_to_create[i]));

	// Reserve the dest buffers
	int num_vertices_to_reserve = src_cartridge_desc.num_sides * 10;
	points.reserve(   num_vertices_to_reserve );
	normals.reserve(  num_vertices_to_reserve );
	tex_uvs.reserve(  num_vertices_to_reserve );
	polygons.reserve( num_vertices_to_reserve );

	firearm::CartridgeMaker cm;

	Result::Name res = Result::SUCCESS;

	float case_top_height = 0.0f;

	switch( model_type )
	{
	case MT_CASE:
		res = cm.MakeCase( src_cartridge_desc.case_desc, 16, points, normals, tex_uvs, polygons );
		break;
	case MT_BULLET:
		res = cm.MakeBullet( src_cartridge_desc.bullet_desc, 16, case_top_height, points, normals, tex_uvs, polygons );
		break;
	case MT_CARTRIDGE:
		res = cm.Make( src_cartridge_desc, src_cartridge_desc.num_sides, points, normals, tex_uvs, polygons );
		break;
	default:
		break;
	}

	boost::shared_ptr<General3DMesh> pMesh( new General3DMesh );

	res = CreateMesh( points, normals, polygons, *pMesh );

//	diffuse_colors.resize( points.size(), SFloatRGBAColor::White() );
//	C3DMeshModelArchive dest_mesh_archive;
//	vector< vector<uint> > uint_polygons;
//	Result::Name res = CreateSingleSubsetMeshArchive(
//		points,
//		normals,
//		diffuse_colors,
//		tex_uvs,
//		uint_polygons,
//		dest_mesh_archive
//		);

	// Do not re-calculate the normals.
//	pMesh->CalculateVertexNormalsFromPolygonPlanes();

	// msh

	C3DMeshModelBuilder builder;
	builder.BuildMeshModelArchive( pMesh );
	builder.GetArchive().SaveToFile( output_model_basename + ".msh" );

	// obj

	tex_uvs.resize( points.size(), TEXCOORD2(0,0) );
	const string obj_pathname = output_model_basename + ".obj";
	wavefront_obj::write_to_obj_file( obj_pathname, points, tex_uvs, normals, polygons );
}


void CreateBullets( const string& output_model_directory )
{
	vector<Vector3> points, normals;
	vector< vector<int> > int_index_polygons;
	vector< vector<unsigned int> > polygons;
	vector<TEXCOORD2> tex_uvs;
	vector<SFloatRGBAColor> vertex_diffuse_colors;

	for( int i=0; i<numof(sg_cases_to_create); i++ )
	{
		firearm::BulletDesc bd;
		bool obtained = firearm::GetFMJBulletDesc( sg_cases_to_create[i], bd );
		if( !obtained )
			continue;

		const string caliber_name = std::string(GetCaliberName(sg_cases_to_create[i]));

//		string output_model_basename = output_model_directory + caliber_name + "-bullet";

//		firearm::CartridgeDesc src_cartridge_desc;
//		src_cartridge_desc.bullet_desc = bd;
//		Create( src_cartridge_desc, output_model_basename, MT_BULLET );

		// obj

		points.resize( 0 );
		normals.resize( 0 );
		tex_uvs.resize( 0 );
		int_index_polygons.resize( 0 );
		polygons.resize( 0 );
		float case_top_height = 0;
		firearm::CartridgeMaker cm;
		cm.MakeBullet( bd, 16, case_top_height, points, normals, tex_uvs, int_index_polygons );
//		tex_uvs.resize( points.size(), TEXCOORD2(0,0) );
		vertex_diffuse_colors.resize( points.size(), SFloatRGBAColor::White() );

		// Convert vertex indices from signed to unsigned.
		const size_t num_polygons = int_index_polygons.size();
		polygons.resize( num_polygons );
		for( size_t i=0; i<num_polygons; i++ )
		{
			const size_t num_vertices = int_index_polygons[i].size();
			polygons[i].resize( num_vertices );
			for( size_t j=0; j<num_vertices; j++ )
			{
				polygons[i][j] = (unsigned int)int_index_polygons[i][j];
			}
		}

		C3DMeshModelArchive dest_mesh;
		Result::Name res = CreateSingleSubsetMeshArchive( points, normals, vertex_diffuse_colors, tex_uvs, polygons, dest_mesh );
		dest_mesh.SaveToFile( output_model_directory + caliber_name + "-bullet.msh" );

		dest_mesh.WriteToTextFile( output_model_directory + "archive/" + caliber_name + "-bullet.txt" );

		// msh
		boost::shared_ptr<General3DMesh> pMesh( new General3DMesh );
		MakeBullet( bd, 16, case_top_height, *pMesh );
		C3DMeshModelBuilder builder;
		builder.BuildMeshModelArchive( pMesh );
		builder.GetArchive().WriteToTextFile( output_model_directory + "built/" + caliber_name + "-bullet.txt" );

		const string obj_pathname = output_model_directory + caliber_name + "-bullet.obj";
		wavefront_obj::write_to_obj_file( obj_pathname, points, tex_uvs, normals, polygons );
	}
}


void CreateCartridges( const firearm::CartridgeDesc& ref_cartridge_desc, const string& output_model_directory )
{
	for( int i=0; i<numof(sg_cases_to_create); i++ )
	{
		firearm::CartridgeDesc cartridge_desc;// = ref_case_desc; firearm::GetCaseDesc() overwrites all the fields

		bool cd_obtained = firearm::GetCaseDesc( sg_cases_to_create[i], cartridge_desc.case_desc );
		if( !cd_obtained )
			continue;

		bool bd_obtained = firearm::GetFMJBulletDesc( sg_cases_to_create[i], cartridge_desc.bullet_desc );
		if( !bd_obtained )
			continue;

//		cartridge_desc.case_desc.num_sides   = ref_cartridge_desc.num_sides;
//		cartridge_desc.bullet_desc.num_sides = ref_cartridge_desc.num_sides;

		cartridge_desc.num_sides              = ref_cartridge_desc.num_sides;
//		cartridge_desc.case_desc.num_sides    = ref_cartridge_desc.num_sides;
		cartridge_desc.case_desc.primer_model = ref_cartridge_desc.case_desc.primer_model;

		cartridge_desc.case_desc.drill_style = firearm::CaseDesc::DS_OPEN;

		const string caliber_name = std::string(GetCaliberName(sg_cases_to_create[i]));

		path output_model_pathname = path(output_model_directory) / path(caliber_name);

		Create( cartridge_desc, output_model_pathname.string(), MT_CARTRIDGE );
	}
}

/*
void CreateCase( const firearm::CaseDesc& src_case_desc, const string& output_model_basename )
{
	vector<Vector3> points, normals;
	vector< vector<int> > polygons;
	vector<TEXCOORD2> tex_uvs;
	firearm::CaseDesc cd;// = ref_case_desc; firearm::GetCaseDesc() overwrites all the fields
	bool obtained = firearm::GetCaseDesc( src_case_desc.caliber, cd );
	if( !obtained )
		return;

	cd.num_sides    = src_case_desc.num_sides;
	cd.primer_model = src_case_desc.primer_model;

	const string caliber_name = std::string(GetCaliberName(sg_cases_to_create[i]));

	// Clear the dest buffers
	points.resize( 0 );
	normals.resize( 0 );
	tex_uvs.resize( 0 );
	polygons.resize( 0 );

	firearm::CartridgeMaker cm;

	Result::Name res = cm.MakeCase( cd, 16, points, normals, tex_uvs, polygons );

	boost::shared_ptr<General3DMesh> pMesh( new General3DMesh );
//	MakeCase( cd, cd.num_sides, *pMesh );

	CreateMesh( points, normals, polygons, *pMesh );

	// Do not re-calculate the normals.
//	pMesh->CalculateVertexNormalsFromPolygonPlanes();

	// msh

	C3DMeshModelBuilder builder;
	builder.BuildMeshModelArchive( pMesh );
	builder.GetArchive().SaveToFile( output_model_basename + ".msh" );

	// obj

	tex_uvs.resize( points.size(), TEXCOORD2(0,0) );
	const string obj_pathname = output_model_basename + ".obj";
	wavefront_obj::write_to_obj_file( obj_pathname, points, tex_uvs, normals, polygons );
}
*/

void CreateCases( firearm::CaseDesc& ref_case_desc, const string& output_model_directory )
{
	vector<Vector3> points, normals;
	vector< vector<int> > polygons;
	vector<TEXCOORD2> tex_uvs;

	for( int i=0; i<numof(sg_cases_to_create); i++ )
	{
		firearm::CaseDesc cd;// = ref_case_desc; firearm::GetCaseDesc() overwrites all the fields
		bool obtained = firearm::GetCaseDesc( sg_cases_to_create[i], cd );
		if( !obtained )
			continue;

		cd.num_sides    = ref_case_desc.num_sides;
		cd.primer_model = ref_case_desc.primer_model;

		const string caliber_name = std::string(GetCaliberName(sg_cases_to_create[i]));

		path output_model_pathname = path(output_model_directory) / path( caliber_name + "-case" );

		firearm::CartridgeDesc cartridge_desc;
		cartridge_desc.case_desc = cd;
		cartridge_desc.num_sides = cd.num_sides;
		Create( cartridge_desc, output_model_pathname.string(), MT_CASE );

		// Clear the dest buffers
/*		points.resize( 0 );
		normals.resize( 0 );
		tex_uvs.resize( 0 );
		polygons.resize( 0 );

		firearm::CartridgeMaker cm;
		cm.MakeCase( cd, 16, points, normals, tex_uvs, polygons );

		boost::shared_ptr<General3DMesh> pMesh( new General3DMesh );
//		MakeCase( cd, cd.num_sides, *pMesh );

		CreateMesh( points, normals, polygons, *pMesh );

		// Do not re-calculate the normals.
//		pMesh->CalculateVertexNormalsFromPolygonPlanes();

		// msh

		C3DMeshModelBuilder builder;
		builder.BuildMeshModelArchive( pMesh );
		builder.GetArchive().SaveToFile( output_model_directory + caliber_name + "-case.msh" );

		// obj

		tex_uvs.resize( points.size(), TEXCOORD2(0,0) );
		const string obj_pathname = output_model_directory + caliber_name + "-case.obj";
		wavefront_obj::write_to_obj_file( obj_pathname, points, tex_uvs, normals, polygons );
*/	}
}


bool ProcessProgramOptions( int argc, const char *argv[],
	firearm::CartridgeDesc& cartridge_desc,
	string output_pathname,
	bool& create_all
	)
{
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("create_all",  po::value<int>(),    "Create the models of all the available calibers.")
		("caliber",     po::value<string>(), "the name of the caliber")
		("case",        po::value<string>(), "the name of the caliber for the case")
		("num_sides",   po::value<unsigned int>(),    "the number of polygon divisions on cylinder sides")
		("primer",      po::value<string>(), "")
		("output",      po::value<string>(), "the output model pathname. End the value with '.obj' to save the model in the Wavefront obj format.")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

//	if (vm.count("help")) {
//		cout << desc << "\n";
//		return 1;
//	}

	if(vm.count("create_all"))
		create_all = true;

	string caliber_name, case_caliber_name;

	if(vm.count("caliber"))
		caliber_name = vm["caliber"].as<string>();
	if(vm.count("case"))
		case_caliber_name = vm["case"].as<string>();
	if(vm.count("bullet"))
		case_caliber_name = vm["bullet"].as<string>();
	else if( !create_all )
	{
		// A valid caliber must be specified, unless the user is
		// trying to create all the calibers with the create_all option.
		return false;
	}

	if(vm.count("num_sides"))
	{
		unsigned int num_sides = vm["num_sides"].as<unsigned int>();
		cartridge_desc.num_sides = num_sides;
		cartridge_desc.case_desc.num_sides = num_sides;
		cartridge_desc.bullet_desc.num_sides = num_sides;
	}

	if(vm.count("primer"))
	{
		string primer = vm["primer"].as<string>();
		if( primer == "none" )
			cartridge_desc.case_desc.primer_model = firearm::CaseDesc::PM_NONE;
		else if( primer == "polygon_mesh" )
			cartridge_desc.case_desc.primer_model = firearm::CaseDesc::PM_POLYGON_MESH;
		else
			cartridge_desc.case_desc.primer_model = firearm::CaseDesc::PM_NONE;
	}

	if(vm.count("output"))
		output_pathname = vm["output"].as<string>();

	bool caliber_found = false;
	for( int i=0; i<Caliber::NUM_CALIBERS; i++ )
	{
		const Caliber::Name cal = (Caliber::Name)i;
		if( caliber_name == GetCaliberName(cal) )
		{
			firearm::GetCaseDesc( cal, cartridge_desc.case_desc );
			caliber_found = true;
			break;
		}
	}

	if( !caliber_found )
		return false;

	if (vm.count("num_sides"))
		cartridge_desc.num_sides = (unsigned int)get_clamped( vm["num_sides"].as<int>(), 3, 0xFFFF );

	return true;
}


int run( int argc, const char *argv[] )
{
	const string init_wd( lfs::get_cwd() );

	if( init_wd.find( "\\app" ) != init_wd.length() - 4
	 && init_wd.find( "/app" )  != init_wd.length() - 4 )
	{
		lfs::set_wd( "../../app" );
	}

	InitHTMLLog( "ammo_gen_log.html" );

	firearm::CartridgeDesc cartridge_desc;
	string output_pathname = "out.obj";
	bool create_all = false;
	bool proceed = ProcessProgramOptions(argc,argv,cartridge_desc,output_pathname,create_all);
//	if( !proceed )
//		return 0;

	string output_model_directory = "results." + string(GetBuildInfo()) + "/";
	boost::filesystem::create_directories( output_model_directory );

	if( create_all )
	{
		CreateCases( cartridge_desc.case_desc, output_model_directory );

		CreateBullets( output_model_directory );

		CreateCartridges( cartridge_desc, output_model_directory );
	}
	else
	{
		// Create the cartridge with the specified name
		std::vector<Vector3> points;
		std::vector<Vector3> normals;
		std::vector<TEXCOORD2> tex_uvs;
		std::vector< std::vector<int> > polygons;

		firearm::CartridgeMaker cm;
		cm.Make( cartridge_desc, cartridge_desc.num_sides,
			points, normals, tex_uvs, polygons );

		path ext = path(output_pathname).extension();
		if( ext == ".obj" )
		{
			wavefront_obj::write_to_obj_file( output_pathname, points, tex_uvs, normals, polygons );
		}
	}

	return 0;
}


int main( int argc, const char *argv[] )
{
	int ret = 0;
	try
	{
		ret = run( argc, argv );
	}
	catch( std::exception& e )
	{
		GlobalLog().Print( WL_WARNING, "exception: %s", e.what() );
	}

	return ret;
}
