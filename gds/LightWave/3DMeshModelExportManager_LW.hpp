#ifndef  __3DMESHMODELEXPORTMANAGER_LW_H__
#define  __3DMESHMODELEXPORTMANAGER_LW_H__


#include <string>
#include <vector>
using namespace std;

#include <boost/shared_ptr.hpp>

#include "LightWave/fwd.hpp"


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

	vector<C3DMeshModelBuilder *> m_vecpModelBuilder;

	string m_strBaseOutFilename;

	void GetOutputFilename( string& dest_filename, const string& src_layer_name );

public:

	C3DMeshModelExportManager_LW() {}

	~C3DMeshModelExportManager_LW() { Release(); }

	void Release();

	/// creates mesh model(s) from a .lwo file
	/// a valid LWO2 model file has to be specified
	bool BuildMeshModels( const string& lwo_filename );

};


}	/*  MeshModel  */


#endif		/*  __3DMESHMODELEXPORTMANAGER_LW_H__  */