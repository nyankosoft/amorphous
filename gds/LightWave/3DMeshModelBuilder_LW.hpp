#ifndef  __3DMESHMODELBUILDER_LW_H__
#define  __3DMESHMODELBUILDER_LW_H__


#include <list>
#include <string>
#include <vector>
using namespace std;

#include <boost/shared_ptr.hpp>


#include "Graphics/MeshModel/3DMeshModelBuilder.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"

#include "LightWave/LWO2_Layer.hpp"


namespace MeshModel
{


struct SLayerSet
{
	string strOutputFilename;

	std::vector<CLWO2_Layer *> vecpMeshLayer;

	CLWO2_Layer *pSkelegonLayer;

	SLayerSet() : pSkelegonLayer(NULL) {}

	SLayerSet( const std::vector<CLWO2_Layer *>& mesh_layer )
		: vecpMeshLayer(mesh_layer), pSkelegonLayer(NULL) {}

	SLayerSet(const string& output_filename) : strOutputFilename(output_filename), pSkelegonLayer(NULL) {}
};


class C3DMeshModelBuilder_LW : public C3DModelLoader
{
	/// borrowed reference
	/// this class does not load / release the model data
	/// the model data has to be supplied before building the model
	boost::shared_ptr<CLWO2_Object> m_pSrcObject;


	string m_strTargetLayerName;


	/// store indices from the original skeleton index to the dest bone index
	/// dest bone indices are in the order of the tree traversal during runtime
	/// and point to blend matrices
	vector<int> m_vecDestBoneIndex;
	int m_iNumDestBones;

	/// hold info from which mesh is created
	SLayerSet m_TargetLayerInfo;

	std::vector<std::string> m_vecSurfaceMaterialOption;

	const unsigned int m_DefaultVertexFlags;

private:

	void BuildSkeletonFromSkelegon_r( int iSrcBoneIndex,
		                              vector<CLWO2_Bone>& rvecSrcBone,
									  CLWO2_Layer& rLayer,
//									  const Vector3& vParentOffset,
									  CMMA_Bone& rDestBone );

	/// set transform from model space to bone space
	/// all the bone transforms are not supposed to have rotations
	void BuildBoneTransformsNROT_r(const Vector3& vParentOffset, CMMA_Bone& rDestBone );

	void SetVertexWeights( vector<CGeneral3DVertex>& rDestVertexBuffer, CLWO2_Layer& rLayer );

	void ProcessLayer( CLWO2_Layer& rLayer );

	/// create mesh materials from the surfaces of the LightWave object
	/// - surface texture:    stored in CMMA_Material.vecTexture[0]
	/// - normal map texture: stored in CMMA_Material.vecTexture[1]
	void SetMaterials();

	/// load options written in the comment text box on the surface editor dialog
	void LoadSurfaceCommentOptions();

public:

	C3DMeshModelBuilder_LW();

	C3DMeshModelBuilder_LW( boost::shared_ptr<CLWO2_Object> pSrcObject );

	~C3DMeshModelBuilder_LW();

	virtual const std::string GetInputDirectoryPath() const;

	virtual const std::string GetOutputFilePath() const;

	virtual const std::string& GetSurfaceMaterialOption( int material_index ) const { return m_vecSurfaceMaterialOption[material_index]; }

	void SetTargetLayerName( string& strName ) { m_strTargetLayerName = strName; }

	void LoadMeshModel();

	/// build mesh from the layers that has the same name as m_strTargetLayerName
	bool BuildMeshModel();

	/// build mesh from the target layer info
	bool BuildMeshModel( SLayerSet& rLayerInfo );

	/// create mesh archive object by directly handing a layer.
	/// the pointer to a source LW object must be set in advance as an argument
	/// of the constructor when the instance is created.
	bool BuildMeshFromLayer( CLWO2_Layer& rLayer );

	void BuildSkeletonFromSkelegon( CLWO2_Layer& rLayer );

	vector<int>& GetDestBoneIndexArray() { return m_vecDestBoneIndex; }

	virtual bool LoadFromFile( const std::string& model_filepath, const CGeometryFilter& geometry_filter );

	virtual std::string GetBasePath();
};


}


#endif		/*  __3DMESHMODELBUILDER_LW_H__  */
