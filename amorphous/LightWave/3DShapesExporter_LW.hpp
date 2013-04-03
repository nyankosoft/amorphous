#ifndef  __3DShapesExporter_LW_HPP__
#define  __3DShapesExporter_LW_HPP__


#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/LightWave/fwd.hpp"


namespace amorphous
{


/**
 * extract shapes from an lwo file
 * [in] a loaded lwo object or filename of an .lwo files
 * [out] descriptions of extracted shapes
 * Steps
 * - Create a general 3D mesh from the layers named "Shapes" in the input lwo object
 * - Separates the general 3D mesh into connected sets
 * - Detect a shape from each connected set
 */
class C3DShapesExporter_LW
{

	boost::shared_ptr<LWO2_Object> m_pObject;

public:

	C3DShapesExporter_LW() {}

	~C3DShapesExporter_LW() { Release(); }

	void Release();

	bool ExtractShapes( boost::shared_ptr<LWO2_Object> pObject, const std::vector<std::string>& output_filepaths );

	bool ExtractShapes( boost::shared_ptr<LWO2_Object> pObject, const std::string& output_filepath );
};


} // amorphous



#endif		/*  __3DShapesExporter_LW_HPP__  */
