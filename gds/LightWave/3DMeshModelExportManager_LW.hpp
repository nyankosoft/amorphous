#ifndef  __3DMESHMODELEXPORTMANAGER_LW_H__
#define  __3DMESHMODELEXPORTMANAGER_LW_H__


#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "gds/base.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/LightWave/fwd.hpp"


namespace morph
{
class progress_display;
};


namespace MeshModel
{


class C3DMeshModelBuilder;

/**
 * creates mesh file(s) from an .lwo model file
 * mesh compile options can be specified in layer name strings
 * - CreateMesh
 * -- creates a standard mesh object
 * -- -f specifies output filename (e.g. CreateMesh -f box.msh)
 * 
 */
class C3DMeshModelExportManager_LW
{

	boost::shared_ptr<CLWO2_Object> m_pObject;

	std::vector<C3DMeshModelBuilder *> m_vecpModelBuilder;

	std::string m_strBaseOutFilename;

	std::vector<std::string> m_OutputFilepaths;

	void GetOutputFilename( std::string& dest_filename, const std::string& src_layer_name );

public:

	C3DMeshModelExportManager_LW() {}

	~C3DMeshModelExportManager_LW() { Release(); }

	void Release();

	/// creates mesh model(s) from a .lwo file
	/// a valid LWO2 model file has to be specified
	bool BuildMeshModels( const std::string& lwo_filename, U32 build_option_flags = 0 );

	int GetNumBuiltMeshModelArchives() const { return (int)m_vecpModelBuilder.size(); }

	C3DMeshModelArchive *GetBuiltMeshModelArchive( uint model_index );

	std::string GetOutputFilepath( uint model_index );

	std::vector< std::vector<std::string> > GetOriginalTextureFilepaths( uint model_index );

	const morph::progress_display& GetSourceObjectLoadingProgress() const;
};


}	/*  MeshModel  */


#endif		/*  __3DMESHMODELEXPORTMANAGER_LW_H__  */
