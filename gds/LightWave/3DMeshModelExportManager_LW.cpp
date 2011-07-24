#include "3DMeshModelExportManager_LW.hpp"
#include "3DMeshModelBuilder_LW.hpp"
using namespace MeshModel;

#include "LWO2_Common.hpp"
#include "LWO2_Object.hpp"
#include "gds/Support/memory_helpers.hpp"
#include "gds/Support/progress_display.hpp"
#include "gds/Support/lfs.hpp"
#include <boost/filesystem.hpp>

using std::string;
using std::vector;
using namespace boost;
using namespace boost::filesystem;


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


bool C3DMeshModelExportManager_LW::BuildMeshModels( const string& lwo_filename, U32 build_option_flags )
{
	Release();

	// load light wave model data
	m_pObject = shared_ptr<CLWO2_Object>( new CLWO2_Object() );

	if( !m_pObject->LoadLWO2Object( lwo_filename.c_str() ) )
	{
		Release();
		return false;
	}

	string strOutFilename, strBaseOutFilename = lwo_filename;

	// set the default output filename
	// if no output filename is specified for the mesh, the original filename
	// will be used as the body filename
	lfs::change_ext( strBaseOutFilename, "msh" );

	m_strBaseOutFilename = strBaseOutFilename;

	// search through layers.
	// layers whose names begin with "CreateMesh..." are the target layers.
	// they are processed into mesh models

	vector<SLayerSet> vecLayerSet;

	string mesh_tag = "CreateMesh";
	size_t i, j, num_layer_sets = 0;

	vector<CLWO2_Layer *> vecpMeshLayer = m_pObject->GetLayersWithKeyword( "CreateMesh", CLWO2_NameMatchCond::START_WITH );
	size_t num_tgt_layers = vecpMeshLayer.size();
	vector<string> words;

	// layers with names that start with "CreateMesh" - registered as mesh layers
	for( i=0; i<num_tgt_layers; i++ )
	{
		CLWO2_Layer *layer = vecpMeshLayer[i];

		SeparateStrings( words, layer->GetName().c_str(), " \t" );

		GetOutputFilename( strOutFilename, layer->GetName() );

		int group_number = -1;
		for( j=0; j<words.size(); j++ )
		{
			// is the word a group option, i.e. -g0, -g1, ...?
			if( words[j].find( "-g" ) == 0 )
			{
				// The program assumes that the "-g" is immediately followed by a group number,
				// and nothing comes after the group number
				group_number = atoi( words[j].substr( 2 ).c_str() );
				break;
			}
		}

		for( j=0; j<num_layer_sets; j++ )
		{
			if( vecLayerSet[j].strOutputFilename == strOutFilename )
			{
				// A layer set with the same output filepath already exists
				// - add the layer to this layer set.
				vecLayerSet[j].vecpMeshLayer.push_back( layer );
				break;
			}
			else if( vecLayerSet[j].GroupNumber == group_number )
			{
				// A layer with the same group number already exists.
				// - add the layer to this layer set.
				vecLayerSet[j].vecpMeshLayer.push_back( layer );

				if( vecLayerSet[j].strOutputFilename == "" )
					vecLayerSet[j].strOutputFilename = strOutFilename;

				break;
			}
		}

		if( j == num_layer_sets )
		{
			// create a new layer set for an output mesh file
			vecLayerSet.push_back( SLayerSet(strOutFilename) );
			vecLayerSet.back().vecpMeshLayer.push_back( layer );
			vecLayerSet.back().GroupNumber = group_number;
			num_layer_sets++;
		}
	}

	if( vecLayerSet.size() == 1 && vecLayerSet[0].strOutputFilename == "" )
	{
		vecLayerSet[0].strOutputFilename = m_strBaseOutFilename;
	}


	vector<CLWO2_Layer *> vecpSkeletonLayer = m_pObject->GetLayersWithKeyword( "Skeleton", CLWO2_NameMatchCond::START_WITH );

	// if a layer with the name that begins with "Skeleton" was not found, for skeletons, target the layers named with either "CreateMesh" or "Skeleton"
	vecpSkeletonLayer.insert( vecpSkeletonLayer.end(), vecpMeshLayer.begin(), vecpMeshLayer.end() );

//	num_tgt_layers = vecpSkeletonLayer.size();

	const size_t num_skeleton_layers = vecpSkeletonLayer.size();
	num_layer_sets = vecLayerSet.size();
	for( i=0; i<num_layer_sets; i++ )
	{
		SLayerSet& layer_set = vecLayerSet[i];

		if( layer_set.vecpMeshLayer.empty() )
			continue; // No mesh data for skeleton: skip this set

		if( vecpSkeletonLayer.empty() )
		{
			// Assumes that the first layer contains the skelegons
			layer_set.pSkelegonLayer = layer_set.vecpMeshLayer.front();
			continue;
		}

		for( j=0; j<num_skeleton_layers; j++ )
		{
			CLWO2_Layer *pSkeletonLayer = vecpSkeletonLayer[j];
			GetOutputFilename( strOutFilename, pSkeletonLayer->GetName() );

			if( layer_set.strOutputFilename == strOutFilename )
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

	num_layer_sets = vecLayerSet.size();

	m_OutputFilepaths.clear();
	m_OutputFilepaths.resize( num_layer_sets );
	for( i=0; i<num_layer_sets; i++ )
	{
//		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder_LW( m_pObject ) );

//		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder() );

//		m_vecpModelBuilder.back()->SetTextureFilenameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

//		m_vecpModelBuilder.back()->BuildMeshModel( vecLayerSet[i] );

		shared_ptr<C3DMeshModelBuilder_LW> pModelLoader
			= shared_ptr<C3DMeshModelBuilder_LW>( new C3DMeshModelBuilder_LW( m_pObject ) );

		pModelLoader->SetTexturePathnameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

		pModelLoader->BuildMeshModel( vecLayerSet[i] );

		m_vecpModelBuilder.push_back( new C3DMeshModelBuilder() );

		// Convert a general 3D mesh to a mesh archive?
		m_vecpModelBuilder.back()->BuildMeshModel( pModelLoader, build_option_flags );

		string loader_output_filepath = pModelLoader->GetOutputFilePath();
		path output_path = path(lwo_filename).parent_path() / loader_output_filepath;
		if( 0 < loader_output_filepath.length() )
		{
			bool saved = m_vecpModelBuilder.back()->GetArchive().SaveToFile( output_path.string() );
		}

		m_OutputFilepaths[i] = output_path.string();
	}

	return true;
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


const morph::progress_display& C3DMeshModelExportManager_LW::GetSourceObjectLoadingProgress() const
{
	static morph::progress_display invalid_progress(0);
	if( m_pObject )
		return m_pObject->GetProgressDisplay();
	else
		return invalid_progress;
}
