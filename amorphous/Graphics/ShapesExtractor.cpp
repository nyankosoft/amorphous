#include "ShapesExtractor.hpp"
#include "ShapeDetector.hpp"
#include "ShapeContainers.hpp"
#include "MeshModel/General3DMesh.hpp"
#include "MeshModel/General3DMeshFunctions.hpp"
#include "MeshModel/PrimitiveShapeMeshes.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using namespace boost;


void CreatePhysicsShapes( ShapeContainerSet& shape_set )
{
}


void ShapesExtractor::AddShapeContainer( General3DMesh& connected_mesh,
											  const ShapeDetectionResults& results,
											  ShapeContainerSet& shape_set )
{
	AABB3 aabb;

	switch( results.shape )
	{
	case MeshShape::AXIS_ALIGNED_BOX:
		shape_set.m_pShapes.push_back( new AABB3Container( results.aabb ) );
		break;

	case MeshShape::ORIENTED_BOX:
		shape_set.m_pShapes.push_back( new OBB3Container( OBB3( results.pose, results.radii ) ) );
		break;

	case MeshShape::CAPSULE:
		shape_set.m_pShapes.push_back( new CapsuleContainer( results.capsule ) );
		break;

	case MeshShape::SPHERE:
		shape_set.m_pShapes.push_back( new SphereContainer( Sphere( results.pose.vPosition, results.radii[0] ) ) );
		break;

	case MeshShape::CONVEX:
		LOG_PRINT( " The convex shape is not supported yet." );
//		shape_set.m_pShapes.push_back( new CConvexContainer() );
		break;

	default:
		LOG_PRINT( " An unsupported shape: " + to_string( (int)results.shape ) );
		break;
	}
}


Result::Name ShapesExtractor::ExtractShapes( const shared_ptr<General3DMesh> pSrcMesh, ShapeContainerSet& shape_set )
{
	if( !pSrcMesh )
		return Result::INVALID_ARGS;

	// Divide general 3D mesh(es) into connected  sets
	vector< const shared_ptr<General3DMesh> > pSrcMeshes;
	pSrcMeshes.push_back( pSrcMesh );
	vector< shared_ptr<General3DMesh> > pConnectedMeshes;
	pConnectedMeshes.reserve( 16 );
	for( int i=0; i<(int)pSrcMeshes.size(); i++ )
	{
		if( !pSrcMeshes[i] )
			continue;

		vector< shared_ptr<General3DMesh> > pConnected;
		GetConnectedSets( *(pSrcMeshes[i]), pConnected );

		if( pConnected.empty() )
			continue;

		pConnectedMeshes.assign( pConnected.begin(), pConnected.end() );
	}

	LOG_PRINT( fmt_string(" Found %d connected meshes.", (int)pConnectedMeshes.size() ) );

	// For each connected sets, detect a shape
//	ShapeContainerSet shape_set;
	const int num_connected_meshes = (int)pConnectedMeshes.size();
	for( int i=0; i<num_connected_meshes; i++ )
	{
		if( !pConnectedMeshes[i] )
			continue;

		ShapeDetector shape_detector;
		ShapeDetectionResults results;
		bool detected = shape_detector.DetectShape( *(pConnectedMeshes[i]), results );

		if( !detected )
			continue;

		AddShapeContainer( *(pConnectedMeshes[i]), results, shape_set );
	}

	return Result::SUCCESS;
}


Result::Name ShapesExtractor::ExtractShapesAndSaveToFile( const shared_ptr<General3DMesh> pSrcMesh, const std::string& output_filepath )
{
	ShapeContainerSet shape_set;
	Result::Name res = ExtractShapes( pSrcMesh, shape_set );

	bool saved = shape_set.SaveToFile( output_filepath );
	if( !saved )
	{
		LOG_PRINT( " Failed to save shapes to a file: " + output_filepath );
		return Result::UNKNOWN_ERROR;
	}

	return Result::SUCCESS;
}


} // namespace amorphous
