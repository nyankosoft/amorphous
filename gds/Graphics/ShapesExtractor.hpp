#ifndef __ShapesExtractor_HPP__
#define __ShapesExtractor_HPP__


#include <string>
#include <boost/shared_ptr.hpp>
#include "../base.hpp"
#include "../Graphics/fwd.hpp"


class CShapeSet;
class CShapeDetectionResults;


class CShapesExtractor
{
	void AddShapeContainer( CGeneral3DMesh& connected_mesh, const CShapeDetectionResults& results, CShapeSet& shape_set );

public:

	CShapesExtractor(){}

	~CShapesExtractor(){}

	Result::Name ExtractShapes( boost::shared_ptr<CGeneral3DMesh> pSrcMesh, CShapeSet& shape_set );

	Result::Name ExtractShapes( boost::shared_ptr<CGeneral3DMesh> pSrcMesh, const std::string& output_filepath );
};


#endif /* __ShapesExtractor_HPP__ */
