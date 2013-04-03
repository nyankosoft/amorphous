#include "3DShapesExporter_LW.hpp"
#include "3DMeshModelBuilder_LW.hpp"
#include "LWO2_Object.hpp"
#include "gds/Graphics/ShapesExtractor.hpp"


namespace amorphous
{

using namespace boost;


/// each i-th element of output_filepaths corresponds to the i-th mesh group
bool C3DShapesExporter_LW::ExtractShapes( boost::shared_ptr<LWO2_Object> pObject, const std::vector<std::string>& output_filepaths )
{
	if( output_filepaths.empty() )
		return false;

	if( !pObject )
		return false;

	// Find layeres whose name begins with "Shapes"
	SLayerSet layer_set;
	layer_set.vecpMeshLayer;
	std::vector<LWO2_Layer *> pLayers
		= pObject->GetLayersWithKeyword( "Shapes", LWO2_NameMatchCond::START_WITH );

	if( pLayers.empty() )
	{
		LOG_PRINT( " Found no layers whose name starts with 'Shapes'" );
		return false;
	}

	// Note that each layer set may or may not have models for shapes
	std::vector<SLayerSet> layer_sets;
	layer_sets.resize( output_filepaths.size() );

	for( int i=0; i<(int)pLayers.size(); i++ )
	{
		if( !pLayers[i] )
			continue;

		const std::string layer_name = pLayers[i]->GetName();

		std::vector<std::string> words;
		if( layer_name.length() == 0 )
			continue;

		SeparateStrings( words, layer_name.c_str(), " \t" );

		int group_number = GetGroupNumber( words );
		if( 0 <= group_number )
		{
			if( 0 <= group_number && group_number < (int)output_filepaths.size() )
				layer_sets[group_number].vecpMeshLayer.push_back( pLayers[i] );
		}
		else
		{
			// The layer is simply named "Shapes", and has not group index;
			// Add it to the first layer set.
			layer_sets.front().vecpMeshLayer.push_back( pLayers[i] );
		}
	}

	Result::Name res = Result::SUCCESS;

	for( int i=0; i<(int)layer_sets.size(); i++ )
	{
		SLayerSet& layer_set = layer_sets[i];

		if( layer_set.vecpMeshLayer.empty() )
			continue;

		// Build general 3D mesh(es)
		shared_ptr<C3DMeshModelBuilder_LW> pLoader( new C3DMeshModelBuilder_LW( pObject ) );
		pLoader->BuildMeshModel( layer_set );

		shared_ptr<General3DMesh> pSrcMesh = pLoader->GetGeneral3DMeshSharedPtr();
		if( !pSrcMesh )
			return false;

		ShapesExtractor shapes_extractor;
		Result::Name extract_result = shapes_extractor.ExtractShapesAndSaveToFile( pSrcMesh, output_filepaths[i] );
		if( extract_result != Result::SUCCESS )
			res = extract_result;
	}

	return (res == Result::SUCCESS) ? true : false;
}


bool C3DShapesExporter_LW::ExtractShapes( boost::shared_ptr<LWO2_Object> pObject, const std::string& output_filepath )
{
	std::vector<std::string> output_filepaths;
	output_filepaths.push_back( output_filepath );

	return ExtractShapes( pObject, output_filepaths );
}


void C3DShapesExporter_LW::Release()
{
	m_pObject.reset();
}


} // amorphous
