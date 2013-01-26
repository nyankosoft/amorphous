#ifndef  __3DMESHMODELBUILDER_H__
#define  __3DMESHMODELBUILDER_H__


#include "../IndexedPolygon.hpp"
#include "../TextureCoord.hpp"
#include "../FloatRGBAColor.hpp"
#include "../General3DVertex.hpp"
#include "3DMeshModelArchive.hpp"
#include "General3DMesh.hpp"


namespace amorphous
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
	std::vector<int> vecPolygonIndex;
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

	boost::shared_ptr<General3DMesh> m_pMesh;

	/// used to specify the way of exporting the texture filename
	TexturePathnameOption::Option m_TextureFilePathOption;

	/// hold a user-defined fixed path for texture filenames
	std::string m_strTexPath;

//	std::vector<CPolygonGroup> m_vecPolygonGroup;

	std::vector<int> m_vecPolygonGroupIndex;

	std::vector<std::string> m_vecPolygonGroupName;

public:

	C3DModelLoader();

	virtual ~C3DModelLoader() {}

	int GetNumMaterials() const { return m_pMesh->GetNumMaterials(); }

	const std::vector<General3DVertex>& GetVertexBuffer() const { return (*m_pMesh->GetVertexBuffer().get()); }

	std::vector<CMMA_Material>& GetMaterialBuffer() { return m_pMesh->GetMaterialBuffer(); }

	std::vector<IndexedPolygon>& GetPolygonBuffer() { return m_pMesh->GetPolygonBuffer(); }

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
	void GetGeneral3DMesh( General3DMesh& dest ) { dest = *m_pMesh; }

	General3DMesh& GetGeneral3DMesh() { return *m_pMesh; }

	boost::shared_ptr<General3DMesh> GetGeneral3DMeshSharedPtr() { return m_pMesh; }

	void SetVertexFormatFlags( unsigned int flags ) { m_pMesh->m_VertexFormatFlag = flags; }

	void RaiseVertexFormatFlags( unsigned int flags ) { m_pMesh->m_VertexFormatFlag |= flags; }

	/// Returns true on success
	/// If geometry_filter is not provided, or no include/exclude filters were in geometry_filter,
	/// all the geometry in the model file is processed.
	virtual bool LoadFromFile( const std::string& model_filepath, const GeometryFilter& geometry_filter = GeometryFilter() ) = 0;

	virtual std::string GetBasePath() { return std::string(); }
};



/**
  Converts a general 3d mesh into a 3d mesh archive

 [in] A General 3D Mesh loaded by C3DModelLoader which has
   - non-triangulated polygons
   - verties represented as an array of General3DVertex
 [out] a mesh archive
   - 

 common routines to create mesh archives.
 NOTE: vertex buffer in IndexedPolygon is not used. Vertices are
 stored and managed separately by the mesh model builder
 */
class C3DMeshModelBuilder
{
public:

	enum MeshFlags
	{
		MESH_SHADOWVOLUME = (1 << 0),
//		MESH_ = (1 << 1),
//		MESH_ = (1 << 2),
	};

	enum BuildOptionFlags
	{
		BOF_OUTPUT_AS_TEXTFILE                    = (1 << 0), ///< Output the built mesh as a readable text file. Used to examine the content of the created mesh.
		BOF_SAVE_TEXTURES_AS_IMAGE_ARCHIVES       = (1 << 1),
		BOF_CHANGE_TEXTURE_FILE_EXTENSIONS_TO_IA  = (1 << 2), ///< All the output texture filepaths will be changed to ".ia"
//		BOF_ANOTHER_FLAG                          = (1 << 3),
//		BOF_YET_ANOTHER_FLAG                      = (1 << 4),
	};

protected:

	boost::shared_ptr<C3DModelLoader> m_pModelLoader;

	boost::shared_ptr<General3DMesh> m_pMesh;

	/// file name of the original model data
	std::string m_strSrcFilename;

	/// file name of the ouput mesh model built from the input model data
	std::string m_strDestFilename;

	/// holds the mesh models
	/// these models are serialized and save into files
	C3DMeshModelArchive m_MeshModelArchive;

	std::vector<IndexedPolygon> m_vecNonTriangulatedPolygon;

	std::vector<IndexedPolygon> m_vecTriangulatedPolygon;

	/// input texture filepath -> output texture filepath
//	std::map<std::string,std::string> m_DetectedTextureFilepaths;

	// m_OrigTextureFilepaths[i][j] filepath of j-th texture of i-th material
	std::vector< std::vector<std::string> > m_OrigTextureFilepaths;

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
	/// the number of matrices is determined by General3DVertex::NUM_MAX_BLEND_MATRICES_PER_VERTEX
	void ForceWeightMatrixCount();

	void NormalizeVertexBlendWeights();

	void TrimTriangleSetsAndMaterials();

	void SaveOrigTextureFilepaths();

public:

	C3DMeshModelBuilder();

	virtual ~C3DMeshModelBuilder();

	/// \param pModelLoader 3D model loader. Must be in a "loaded" state.
	/// TODO: rename to BuildMeshModelArchive
	void BuildMeshModel( boost::shared_ptr<C3DModelLoader> pModelLoader, U32 build_option_flags = 0 );

	/// \param [in] borrowed reference
	void BuildMeshModelArchive( boost::shared_ptr<General3DMesh> pGeneralMesh, U32 build_option_flags = 0 );

//	void SetTextureFilenameOption( unsigned int option ) { m_TextureFilenameOption = option; }
	void SetTextureFilenameOption( TexturePathnameOption::Option option ) { m_pModelLoader->SetTexturePathnameOption( option ); }

	/// returns a mesh archive object which has been created by BuildMeshModel() call
	C3DMeshModelArchive& GetArchive() { return m_MeshModelArchive; }

	Result::Name GetArchive( C3DMeshModelArchive& dest );

	std::vector<IndexedPolygon>& GetNonTriangulatedPolygonBuffer() { return m_vecNonTriangulatedPolygon; }

//	const std::map<std::string,std::string> GetDetectedTextureFilepaths() const { return m_DetectedTextureFilepaths; }
	const std::vector< std::vector<std::string> >& GetOriginalTextureFilepaths() const { return m_OrigTextureFilepaths; }
};


} // namespace amorphous


#endif		/*  __3DMESHMODELBUILDER_H__  */
