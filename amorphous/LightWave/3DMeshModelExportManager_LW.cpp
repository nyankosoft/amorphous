#include "3DMeshModelExportManager_LW.hpp"
#include "3DMeshModelBuilder_LW.hpp"
#include "LWO2_Common.hpp"
#include "LWO2_Object.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/progress_display.hpp"
#include "amorphous/Support/lfs.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;


class sort_by_group_number
{
public:
	bool operator()( const SLayerSet& lhs, const SLayerSet& rhs ) const { return lhs.GroupNumber < rhs.GroupNumber; }
};


void C3DMeshModelExportManager_LW::Release()
{
	m_pObject.reset();

	size_t i, num = m_vecpModelBuilder.size();
	for( i=0; i<num; i++ )
	{
		SafeDelete( m_vecpModelBuilder[i] );
	}

	m_vecpModelBuilder.clear();
}


void C3DMeshModelExportManager_LW::GetOutputFilename( string& dest_filename, const string& src_layer_name )
{
	size_t pos;
	string strFileTag = "-f ";

	pos = src_layer_name.find( strFileTag ); 

	if( pos == string::npos )
	{
//		dest_filename = m_strBaseOutFilename;
	}
	else
	{
		dest_filename = src_layer_name.substr( pos + strFileTag.length(), 1024 );

		// interpret space char as the end of the output filename
		// this is necessary when other option strings follow the filename
		pos = dest_filename.find( ' ' );

		// find the end of the output filename
		if( pos != string::npos )
		{
			dest_filename = dest_filename.substr( 0, pos );
		}
//		else
//			dest_filename = dest_filename;
	}
}


void C3DMeshModelExportManager_LW::FindMeshLayers( vector<LWO2_Layer *>& mesh_layers, vector<SLayerSet>& layer_sets )
{
	string mesh_tag = "CreateMesh";
	size_t i, j, num_layer_sets = 0;

	mesh_layers = m_pObject->GetLayersWithKeyword( "CreateMesh", LWO2_NameMatchCond::START_WITH );
	size_t num_tgt_layers = mesh_layers.size();
	vector<string> words;

	// layers with names that start with "CreateMesh" - registered as mesh layers
	for( i=0; i<num_tgt_layers; i++ )
	{
		LWO2_Layer *layer = mesh_layers[i];

		words.resize( 0 );
		SeparateStrings( words, layer->GetName().c_str(), " \t" );

		string out_filename;
		GetOutputFilename( out_filename, layer->GetName() );

		int group_number = GetGroupNumber( words );

		for( j=0; j<num_layer_sets; j++ )
		{
			if( layer_sets[j].strOutputFilename == out_filename )
			{
				// A layer set with the same output filepath already exists
				// - add the layer to this layer set.
				layer_sets[j].vecpMeshLayer.push_back( layer );
				break;
			}
			else if( group_number != -1
			 && layer_sets[j].GroupNumber == group_number )
			{
				// A valid group number has been specified
				// && a layer with the same group number already exists.
				// - add the layer to this layer set.
				layer_sets[j].vecpMeshLayer.push_back( layer );

				if( layer_sets[j].strOutputFilename == "" )
					layer_sets[j].strOutputFilename = out_filename;

				break;
			}
		}

		if( j == num_layer_sets )
		{
			// create a new layer set for an output mesh file
			layer_sets.push_back( SLayerSet(out_filename) );
			layer_sets.back().vecpMeshLayer.push_back( layer );
			layer_sets.back().GroupNumber = group_number;
			num_layer_sets++;
		}
	}

	if( layer_sets.size() == 1 && layer_sets[0].strOutputFilename == "" )
	{
		layer_sets[0].strOutputFilename = m_strBaseOutFilename;
	}

	// This ensures that the output filepaths stored in m_OutputFilepaths are in the same orders
	// as the group numbers.
	if( 1 < layer_sets.size() )
		std::sort( layer_sets.begin(), layer_sets.end(), sort_by_group_number() );
}


void C3DMeshModelExportManager_LW::FindSkeletonLayersAndSetToLayerSets( vector<LWO2_Layer *>& vecpMeshLayer, vector<SLayerSet>& layer_sets )
{
	vector<LWO2_Layer *> vecpSkeletonLayer = m_pObject->GetLayersWithKeyword( "Skeleton", LWO2_NameMatchCond::START_WITH );

	// if a layer with the name that begins with "Skeleton" was not found, for skeletons,
	// target the layers named with either "CreateMesh" or "Skeleton"
	vecpSkeletonLayer.insert( vecpSkeletonLayer.end(), vecpMeshLayer.begin(), vecpMeshLayer.end() );

//	num_tgt_layers = vecpSkeletonLayer.size();

	const size_t num_skeleton_layers = vecpSkeletonLayer.size();
	const size_t num_layer_sets = layer_sets.size();
	for( size_t i=0; i<num_layer_sets; i++ )
	{
		SLayerSet& layer_set = layer_sets[i];

		if( layer_set.vecpMeshLayer.empty() )
			continue; // No mesh data for skeleton: skip this set

		if( vecpSkeletonLayer.empty() )
		{
			// Assumes that the first layer contains the skelegons
			layer_set.pSkelegonLayer = layer_set.vecpMeshLayer.front();
			continue;
		}

		for( size_t j=0; j<num_skeleton_layers; j++ )
		{
			LWO2_Layer *pSkeletonLayer = vecpSkeletonLayer[j];
			string out_filename;
			GetOutputFilename( out_filename, pSkeletonLayer->GetName() );

			if( layer_set.strOutputFilename == out_filename )
			{
				// Found a skeleton layer that has the same output pathname
				// as that of the current layer set, layer_set
				// - Register pSkeletonLayer as a skeleton layer for this layer set.
				layer_set.pSkelegonLayer = pSkeletonLayer;
				break;
			}
		}

		if( !layer_set.pSkelegonLayer )
		{
			// The skeleton layer for this layer set was not found in vecpSkeletonLayer
			// - Use the first mesh layer as the skeleton layer.
			layer_set.pSkelegonLayer = layer_set.vecpMeshLayer.front();
		}
	}
}


bool C3DMeshModelExportManager_LW::BuildMeshesAndSaveToFiles( const string& lwo_filename, U32 build_option_flags, vector<SLayerSet>& layer_sets )
{
	const size_t num_layer_sets = layer_sets.size();

	m_OutputFilepaths.clear();
	m_OutputFilepaths.resize( num_layer_sets );
	for( size_t i=0; i<num_layer_sets; i++ )
	{
//		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder_LW( m_pObject ) );

//		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder() );

//		m_vecpModelBuilder.back()->SetTextureFilenameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

//		m_vecpModelBuilder.back()->BuildMeshModel( vecLayerSet[i] );

		shared_ptr<C3DMeshModelBuilder_LW> pModelLoader;
		pModelLoader.reset( new C3DMeshModelBuilder_LW( m_pObject ) );

		pModelLoader->SetTexturePathnameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

		pModelLoader->BuildMeshModel( layer_sets[i] );

		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder() );

		// Convert a general 3D mesh to a mesh archive?
		m_vecpModelBuilder.back()->BuildMeshModel( pModelLoader, build_option_flags );

		string loader_output_filepath = pModelLoader->GetOutputFilePath();
		path output_path = path(lwo_filename).parent_path() / loader_output_filepath;

		// Create output directory/directories if they are not present.
		path output_directory = output_path.parent_path();
		if( !exists(output_directory) )
			create_directories( output_directory );

		if( 0 < loader_output_filepath.length() )
		{
			bool saved = m_vecpModelBuilder.back()->GetArchive().SaveToFile( output_path.string() );
			if( !saved )
				LOG_PRINT_ERROR( "Failed to save the mesh archive to the file: " + output_path.string() );
		}

		m_OutputFilepaths[i] = output_path.string();
	}

	return true;
}


bool C3DMeshModelExportManager_LW::BuildMeshModels( const string& lwo_filename, U32 build_option_flags )
{
	LOG_PRINTF(( "lwo_filename: %s, build_option_flags: %u", lwo_filename.c_str(), (unsigned int)build_option_flags ));

	Release();

	// load light wave model data
	m_pObject.reset( new LWO2_Object() );

	if( !m_pObject->LoadLWO2Object( lwo_filename.c_str() ) )
	{
		Release();
		return false;
	}

	string strBaseOutFilename = lwo_filename;

	// set the default output filename
	// if no output filename is specified for the mesh, the original filename
	// will be used as the body filename
	lfs::change_ext( strBaseOutFilename, "msh" );

	m_strBaseOutFilename = strBaseOutFilename;

	// search through layers.
	// layers whose names begin with "CreateMesh..." are the target layers.
	// they are processed into mesh models

	vector<SLayerSet> layer_sets;
	vector<LWO2_Layer *> mesh_layers;

	FindMeshLayers( mesh_layers, layer_sets );

	FindSkeletonLayersAndSetToLayerSets( mesh_layers, layer_sets );

/*	for(itrLayer = rlstLayer.begin();
		itrLayer != rlstLayer.end();
		itrLayer++)
	{
		num_layer_sets = vecLayerSet.size();

//		pos = itrLayer->GetName().find( "MassSpringCollisionShape" );
//		if( pos != string::npos )
//			itrMSpringCollision = itrLayer;

//		pos = itrLayer->GetName().find( "ShadowVolume" );
//		if( pos != string::npos )
//			itrShadowMeshLayer = itrLayer;

	}*/

	return BuildMeshesAndSaveToFiles( lwo_filename, build_option_flags, layer_sets );
}


C3DMeshModelArchive *C3DMeshModelExportManager_LW::GetBuiltMeshModelArchive( uint model_index )
{
	if( model_index<(uint)m_vecpModelBuilder.size() && m_vecpModelBuilder[model_index] )
		return &m_vecpModelBuilder[model_index]->GetArchive();
	else
		return NULL;
}


std::string C3DMeshModelExportManager_LW::GetOutputFilepath( uint model_index )
{
	if( model_index<(uint)m_OutputFilepaths.size() )
		return m_OutputFilepaths[model_index];
	else
		return string();
}


vector< vector<string> > C3DMeshModelExportManager_LW::GetOriginalTextureFilepaths( uint model_index )
{
	if( model_index<(uint)m_vecpModelBuilder.size() && m_vecpModelBuilder[model_index] )
		return m_vecpModelBuilder[model_index]->GetOriginalTextureFilepaths();
	else
		return vector< vector<string> >();
}


const progress_display& C3DMeshModelExportManager_LW::GetSourceObjectLoadingProgress() const
{
	static progress_display invalid_progress(0);
	if( m_pObject )
		return m_pObject->GetProgressDisplay();
	else
		return invalid_progress;
}


} // amorphous
