#ifndef  __3DMESHMODELBUILDER_LW_H__
#define  __3DMESHMODELBUILDER_LW_H__


#include <list>
#include "fwd.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "amorphous/Graphics/MeshModel/General3DMesh.hpp"


#ifdef _DEBUG
#pragma comment ( lib, "LW_FrameworkEx_d.lib" )
#else
#pragma comment ( lib, "LW_FrameworkEx.lib" )
#endif


namespace amorphous
{


struct SLayerSet
{
	std::string strOutputFilename;

	int GroupNumber;

	std::vector<LWO2_Layer *> vecpMeshLayer;

	/// A skeleton is created from the skelegons in this layer and saved
	/// to the same file with the mesh data extracted from vecpMeshLayer.
	/// A skeleton of a mesh must be represented as a single skelegon tree
	/// in a single layer. 
	LWO2_Layer *pSkelegonLayer;

public:

	SLayerSet()
		:
	GroupNumber(-1),
	pSkelegonLayer(NULL)
	{}

	SLayerSet( const std::vector<LWO2_Layer *>& mesh_layer )
		:
	GroupNumber(-1),
	vecpMeshLayer(mesh_layer),
	pSkelegonLayer(NULL)
	{}

	SLayerSet(const std::string& output_filename)
		:
	strOutputFilename(output_filename),
	GroupNumber(-1),
	pSkelegonLayer(NULL)
	{}
};


class C3DMeshModelBuilder_LW : public C3DModelLoader
{
	/// borrowed reference
	/// this class does not load / release the model data
	/// the model data has to be supplied before building the model
	boost::shared_ptr<LWO2_Object> m_pSrcObject;


	std::string m_strTargetLayerName;


	/// store indices from the original skeleton index to the dest bone index
	/// dest bone indices are in the order of the tree traversal during runtime
	/// and point to blend matrices
	std::vector<int> m_vecDestBoneIndex;
	int m_iNumDestBones;

	/// hold info from which mesh is created
	SLayerSet m_TargetLayerInfo;

	std::vector<std::string> m_vecSurfaceMaterialOption;

	const unsigned int m_DefaultVertexFlags;

	bool m_UseBoneStartAsBoneLocalOrigin;

	std::vector<int> m_PolygonGroupIndices;

private:

	void BuildSkeletonFromSkelegon_r( int iSrcBoneIndex,
		                              const std::vector<LWO2_Bone>& rvecSrcBone,
									  const LWO2_Layer& rLayer,
//									  const Vector3& vParentOffset,
									  CMMA_Bone& rDestBone );

	/// set transform from model space to bone space
	/// all the bone transforms are not supposed to have rotations
	void BuildBoneTransformsNROT_r(const Vector3& vParentOffset, CMMA_Bone& rDestBone );

	void SetVertexWeights( std::vector<General3DVertex>& rDestVertexBuffer, LWO2_Layer& rLayer );

	void ProcessLayer( LWO2_Layer& rLayer, const GeometryFilter& filter = GeometryFilter() );

	/// create mesh materials from the surfaces of the LightWave object
	/// - surface texture:    stored in CMMA_Material.vecTexture[0]
	/// - normal map texture: stored in CMMA_Material.vecTexture[1]
	void SetMaterials();

	/// load options written in the comment text box on the surface editor dialog
	void LoadSurfaceCommentOptions();

	void BreakPolygonsIntoSubsetsByPolygonGroups();

public:

	C3DMeshModelBuilder_LW();

	C3DMeshModelBuilder_LW( boost::shared_ptr<LWO2_Object> pSrcObject );

	~C3DMeshModelBuilder_LW();

	virtual const std::string GetInputDirectoryPath() const;

	virtual const std::string GetOutputFilePath() const;

	virtual const std::string& GetSurfaceMaterialOption( int material_index ) const { return m_vecSurfaceMaterialOption[material_index]; }

	void SetTargetLayerName( std::string& strName ) { m_strTargetLayerName = strName; }

	void LoadMeshModel();

	/// build mesh from the layers that has the same name as m_strTargetLayerName
	bool BuildMeshModel();

	/// build mesh from the target layer info
	bool BuildMeshModel( SLayerSet& rLayerInfo );

	/// create mesh archive object by directly handing a layer.
	/// the pointer to a source LW object must be set in advance as an argument
	/// of the constructor when the instance is created.
	bool BuildMeshFromLayer( LWO2_Layer& rLayer );

	void BuildSkeletonFromSkelegon( LWO2_Layer& rLayer );

	std::vector<int>& GetDestBoneIndexArray() { return m_vecDestBoneIndex; }

	bool LoadFromLWO2Object( boost::shared_ptr<LWO2_Object> pObject, const GeometryFilter& geometry_filter );

	virtual bool LoadFromFile( const std::string& model_filepath, const GeometryFilter& geometry_filter );

	virtual std::string GetBasePath();
};


/// \param words [in] layer name separated by spaces or tabs
int GetGroupNumber( const std::vector<std::string>& words );


} // amorphous



#endif		/*  __3DMESHMODELBUILDER_LW_H__  */
