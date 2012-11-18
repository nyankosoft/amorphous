#ifndef __ShapesExtractor_HPP__
#define __ShapesExtractor_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "../base.hpp"
#include "../Graphics/fwd.hpp"


class CShapeContainerSet;
class CShapeDetectionResults;


class CShapesExtractor
{
	void AddShapeContainer( CGeneral3DMesh& connected_mesh, const CShapeDetectionResults& results, CShapeContainerSet& shape_set );

public:

	CShapesExtractor(){}

	~CShapesExtractor(){}

	Result::Name ExtractShapes( const boost::shared_ptr<CGeneral3DMesh> pSrcMesh, CShapeContainerSet& shape_set );

	/// \param output_filepath [in] pathname of output file.
	Result::Name ExtractShapesAndSaveToFile( const boost::shared_ptr<CGeneral3DMesh> pSrcMesh, const std::string& output_filepath );
};


#endif /* __ShapesExtractor_HPP__ */
