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


/**
 * \brief Detects and extracts primitive shapes, such as boxes and closed convex shapes, from a polygon mesh.
 *
 *
 */
class ShapesExtractor
{
	void AddShapeContainer( General3DMesh& connected_mesh, const ShapeDetectionResults& results, ShapeContainerSet& shape_set );

public:

	ShapesExtractor(){}

	~ShapesExtractor(){}

	/**
	 * \brief Extracts primitive shapes found in the input mesh.
	 *
	 * \param[in] pSrcMesh A polygon mesh containing primitive shapes such as boxes and closed convex shapes.
	 * \param[out] shape_set Shapes found in the input mesh
	 */
	Result::Name ExtractShapes( const std::shared_ptr<General3DMesh> pSrcMesh, ShapeContainerSet& shape_set );

	/// \param output_filepath [in] pathname of output file.
	Result::Name ExtractShapesAndSaveToFile( const std::shared_ptr<General3DMesh> pSrcMesh, const std::string& output_filepath );
};

} // namespace amorphous



#endif /* __ShapesExtractor_HPP__ */
