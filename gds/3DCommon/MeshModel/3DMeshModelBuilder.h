#ifndef  __3DMESHMODELBUILDER_H__
#define  __3DMESHMODELBUILDER_H__

#include <vector>
using namespace std;

#include <boost/shared_ptr.hpp>

#include "3DMath/Vector3.h"

#include "3DCommon/IndexedPolygon.h"
#include "3DCommon/TextureCoord.h"
#include "3DCommon/FloatRGBAColor.h"
#include "3DCommon/General3DVertex.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
#include "3DCommon/MeshModel/General3DMesh.h"

#include "Support/FixedVector.h"


namespace MeshModel
{


class TexturePathnameOption
{
public:

	enum Option
	{
		ORIGINAL_FILENAME = 0,
		BODY_FILENAME_ONLY,
		RELATIVE_PATH_AND_BODY_FILENAME,
		FIXEDPATH_AND_FILENAME,
		NO_TEXTURE_FILENAME,	///< do nothing to texture filename. use this if you want to set the texture filename completely by the model-specific mesh builder
		NUM_OPTIONS
	};
};


class CPolygonGroup
{
public:

	std::string GroupName;
	vector<int> vecPolygonIndex;
};


/**
 - Hold a general 3d mesh 
 - Each modeler-specific derived class needs to implement LoadFromFile()


*/
class C3DModelLoader
{
private:
	
	std::string m_DefaultInputDirPath;
	std::string m_DefaultOutputFilePath;
	std::string m_DefaultSurfaceOption;

protected:

	boost::shared_ptr<CGeneral3DMesh> m_pMesh;

	/// used to specify the way of exporting the texture filename
	TexturePathnameOption::Option m_TextureFilePathOption;

	/// hold a user-defined fixed path for texture filenames
	string m_strTexPath;

//	std::vector<CPolygonGroup> m_vecPolygonGroup;

	std::vector<int> m_vecPolygonGroupIndex;

	std::vector<std::string> m_vecPolygonGroupName;

public:

	C3DModelLoader();

	virtual ~C3DModelLoader() {}

	int GetNumMaterials() const { return m_pMesh->GetNumMaterials(); }

	std::vector<CGeneral3DVertex>& GetVertexBuffer() { return m_pMesh->GetVertexBuffer(); }

	std::vector<CMMA_Material>& GetMaterialBuffer() { return m_pMesh->GetMaterialBuffer(); }

	std::vector<CIndexedPolygon>& GetPolygonBuffer() { return m_pMesh->GetPolygonBuffer(); }

//	CMMA_Bone& GetSkeletonRootBoneBuffer() { return ; }

	virtual const std::string GetInputDirectoryPath() const { return m_DefaultInputDirPath; }

	virtual const std::string GetOutputFilePath() const { return m_DefaultOutputFilePath; }

	/// returns a material option expressed in a command line-style string
	virtual const std::string& GetSurfaceMaterialOption( int material_index ) const { return m_DefaultSurfaceOption; }

	TexturePathnameOption::Option GetTexturePathnameOption() const { return m_TextureFilePathOption; }

	void SetTexturePathnameOption( TexturePathnameOption::Option option ) { m_TextureFilePathOption = option; }

	void SetFixedPathForTextureFilename( const char *pPath ) { m_strTexPath = pPath; }

	const std::string& GetFixedPathForTextureFilename() const { return m_strTexPath; }

	/// Returns a copy of the instance of general 3d mesh currently stored in the loader
	void GetGeneral3DMesh( CGeneral3DMesh& dest ) { dest = *m_pMesh; }

	CGeneral3DMesh& GetGeneral3DMesh() { return *m_pMesh; }

	boost::shared_ptr<CGeneral3DMesh> GetGeneral3DMeshSharedPtr() { return m_pMesh; }

	void SetVertexFormatFlags( unsigned int flags ) { m_pMesh->m_VertexFormatFlag = flags; }

	void RaiseVertexFormatFlags( unsigned int flags ) { m_pMesh->m_VertexFormatFlag |= flags; }

	/// Returns true on success
	virtual bool LoadFromFile( const std::string& model_filepath, const CGeometryFilter& geometry_filter ) = 0;
};



/**
  Converts a general 3d mesh into a 3d mesh archive

 [in] A General 3D Mesh loaded by C3DModelLoader which has
   - non-triangulated polygons
   - verties represented as an array of CGeneral3DVertex
 [out] a mesh archive
   - 

 common routines to create mesh archives.
 NOTE: vertex buffer in CIndexedPolygon is not used. Vertices are
 stored and managed separately by the mesh model builder
 */
class C3DMeshModelBuilder
{
public:

	enum eMeshFlag
	{
		MESH_SHADOWVOLUME = (1 << 0),
//		MESH_ = (1 << 1),
//		MESH_ = (1 << 2),
	};

protected:

	boost::shared_ptr<C3DModelLoader> m_pModelLoader;

	boost::shared_ptr<CGeneral3DMesh> m_pMesh;

	/// file name of the original model data
	string m_strSrcFilename;

	/// file name of the ouput mesh model built from the input model data
	string m_strDestFilename;

	/// holds the mesh models
	/// these models are serialized and save into files
	C3DMeshModelArchive m_MeshModelArchive;

	std::vector<CIndexedPolygon> m_vecNonTriangulatedPolygon;

	std::vector<CIndexedPolygon> m_vecTriangulatedPolygon;

protected:

	/// called after the model is loaded by a modeler-specific routine
	virtual void CreateMeshArchive();

	void Triangulate();

	void CalculateTangentSpace();

	/// create a vertex set in mesh the archive. vertex weights are normalized in this function
	void CreateVertices();

	void CreateTriangleSets();

	/// TODO: create as a separate class?
	void ProcessTextureFilenames();

	void CheckShadowVolume();

	/// make sure that every vertex has the same number of blend matrices
	/// the number of matrices is determined by CGeneral3DVertex::NUM_MAX_BLEND_MATRICES_PER_VERTEX
	void ForceWeightMatrixCount();

	void NormalizeVertexBlendWeights();

	void TrimTriangleSetsAndMaterials();

public:

	C3DMeshModelBuilder();

	virtual ~C3DMeshModelBuilder();

	/// \param pModelLoader 3D model loader. Must be in a "loaded" state.
	/// TODO: rename to BuildMeshModelArchive
	void BuildMeshModel( boost::shared_ptr<C3DModelLoader> pModelLoader );

	/// \param [in] borrowed reference
	void BuildMeshModelArchive( boost::shared_ptr<CGeneral3DMesh> pGeneralMesh );

//	void SetTextureFilenameOption( unsigned int option ) { m_TextureFilenameOption = option; }
	void SetTextureFilenameOption( TexturePathnameOption::Option option ) { m_pModelLoader->SetTexturePathnameOption( option ); }

	/// returns a mesh archive object which has been created by BuildMeshModel() call
	C3DMeshModelArchive& GetArchive() { return m_MeshModelArchive; }

	std::vector<CIndexedPolygon>& GetNonTriangulatedPolygonBuffer() { return m_vecNonTriangulatedPolygon; }

};


}


#endif		/*  __3DMESHMODELBUILDER_H__  */
