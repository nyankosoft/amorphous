#include "3DShapesExporter_LW.hpp"
#include "3DMeshModelBuilder_LW.hpp"
#include "LWO2_Object.hpp"
#include "Graphics/ShapesExtractor.hpp"

using namespace boost;


bool C3DShapesExporter_LW::ExtractShapes( boost::shared_ptr<CLWO2_Object> pObject, const std::string& output_filepath )
{
	if( !pObject )
		return false;

	// Find layeres whose name begins with "Shapes"
	SLayerSet layer_set;
	layer_set.vecpMeshLayer
		= pObject->GetLayersWithKeyword( "Shapes", CLWO2_NameMatchCond::START_WITH );

	if( layer_set.vecpMeshLayer.empty() )
	{
		LOG_PRINT( " Found no layers whose name starts with 'Shapes'" );
		return false;
	}

	// Build general 3D mesh(es)
	boost::shared_ptr<C3DMeshModelBuilder_LW> pLoader( new C3DMeshModelBuilder_LW( pObject ) );
	pLoader->BuildMeshModel( layer_set );

	shared_ptr<CGeneral3DMesh> pSrcMesh = pLoader->GetGeneral3DMeshSharedPtr();
	if( !pSrcMesh )
		return false;

	CShapesExtractor shapes_extractor;
	Result::Name res = shapes_extractor.ExtractShapes( pSrcMesh, output_filepath );

	return (res == Result::SUCCESS) ? true : false;
}


void C3DShapesExporter_LW::Release()
{
	m_pObject.reset();
}
