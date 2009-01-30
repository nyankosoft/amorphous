
#include "3DMeshModelExportManager_LW.hpp"
#include "3DMeshModelBuilder_LW.hpp"
using namespace MeshModel;

#include "LWO2_Common.hpp"
#include "LWO2_Object.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/fnop.hpp"

using namespace boost;


void C3DMeshModelExportManager_LW::Release()
{
	m_pObject.reset();

	size_t i, num = m_vecpModelBuilder.size();
	for( i=0; i<num; i++ )
	{
		SafeDelete( m_vecpModelBuilder[i] );
	}
}


void C3DMeshModelExportManager_LW::GetOutputFilename( string& dest_filename, const string& src_layer_name )
{
	size_t pos;
	string strFileTag = "-f ";

	pos = src_layer_name.find( strFileTag ); 

	if( pos == string::npos )
	{
		dest_filename = m_strBaseOutFilename;
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


bool C3DMeshModelExportManager_LW::BuildMeshModels( const string& lwo_filename )
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
	fnop::change_ext( strBaseOutFilename, "msh" );

	m_strBaseOutFilename = strBaseOutFilename;

	// search through layers.
	// layers whose names begin with "CreateMesh..." are the target layers.
	// they are processed into mesh models

	vector<SLayerSet> vecLayerSet;

	string mesh_tag = "CreateMesh";
	size_t i, j, num_layer_sets = 0;

	vector<CLWO2_Layer *> vecpTargetLayer = m_pObject->GetLayersWithKeyword( "CreateMesh", CLWO2_NameMatchCond::START_WITH );
	size_t num_tgt_layers = vecpTargetLayer.size();

	// layers with names that start with "CreateMesh" - registered as mesh layers
	for( i=0; i<num_tgt_layers; i++ )
	{
		CLWO2_Layer *layer = vecpTargetLayer[i];

		GetOutputFilename( strOutFilename, layer->GetName() );

		for( j=0; j<num_layer_sets; j++ )
		{
			if( vecLayerSet[j].strOutputFilename == strOutFilename )
			{
				vecLayerSet[j].vecpMeshLayer.push_back( layer );
				break;
			}
		}

		if( j == num_layer_sets )
		{
			// create a new layer set for an output mesh file
			vecLayerSet.push_back( SLayerSet(strOutFilename) );
			vecLayerSet.back().vecpMeshLayer.push_back( layer );
			num_layer_sets++;
		}
	}

	vecpTargetLayer = m_pObject->GetLayersWithKeyword( "Skeleton", CLWO2_NameMatchCond::START_WITH );
	num_tgt_layers = vecpTargetLayer.size();

	// collect skelegon layers
	for( i=0; i<num_tgt_layers; i++ )
	{
		CLWO2_Layer *layer = vecpTargetLayer[i];

		// register as a skeleton layer
		// skelegons for a mesh are supposed to be collected in one layer
		// and not to spread across different layers
		GetOutputFilename( strOutFilename, layer->GetName() );

		num_layer_sets = vecLayerSet.size();
		for( j=0; j<num_layer_sets; j++ )
		{
			if( vecLayerSet[j].strOutputFilename == strOutFilename )
			{
				vecLayerSet[j].pSkelegonLayer = layer;
				break;
			}
		}

		if( j == num_layer_sets )
		{
			vecLayerSet.push_back( SLayerSet(strOutFilename) );
			vecLayerSet.back().pSkelegonLayer = layer;
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

		m_vecpModelBuilder.back()->BuildMeshModel(pModelLoader);
	}

	return true;
}