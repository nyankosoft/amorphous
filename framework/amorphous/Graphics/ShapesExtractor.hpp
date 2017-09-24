#ifndef __ShapesExtractor_HPP__
#define __ShapesExtractor_HPP__


#include <string>
#include <memory>
#include "../base.hpp"
#include "../Graphics/fwd.hpp"


namespace amorphous
{


class ShapeContainerSet;
class ShapeDetectionResults;


class ShapesExtractor
{
	void AddShapeContainer( General3DMesh& connected_mesh, const ShapeDetectionResults& results, ShapeContainerSet& shape_set );

public:

	ShapesExtractor(){}

	~ShapesExtractor(){}

	Result::Name ExtractShapes( const std::shared_ptr<General3DMesh> pSrcMesh, ShapeContainerSet& shape_set );

	/// \param output_filepath [in] pathname of output file.
	Result::Name ExtractShapesAndSaveToFile( const std::shared_ptr<General3DMesh> pSrcMesh, const std::string& output_filepath );
};

} // namespace amorphous



#endif /* __ShapesExtractor_HPP__ */
