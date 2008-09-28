#ifndef __StaticGeometryCompiler_H__
#define __StaticGeometryCompiler_H__


#include <boost/shared_ptr.hpp>
#include "StaticGeometryDesc.h"
#include "3DCommon/MeshModel/3DMeshModelArchive.h"
#include "3DCommon/MeshModel/General3DMesh.h"
#include "Stage/StaticGeometry.h"
#include "Stage/StaticGeometryArchiveFG.h"
#include "3DMath/AABTree.h"
using namespace MeshModel;


class TerrainMeshNode;
class TerrainMeshTree;
class CTerrainMeshGenerator;


class CShaderInfo
{
public:
//	std::string ShaderName;
	int ShaderIndex;
	std::map<std::string,int> TechniqueToIndex;
	int TechniqueIndexOffset;

public:

	CShaderInfo(int shader_index = 0) : ShaderIndex(shader_index), TechniqueIndexOffset(0) {}
};


class CStaticGeometryCompiler
{
	CStaticGeometryDesc m_Desc;

	CGeneral3DMesh m_CollisionMesh;

	/// changed to shared_ptr since terrain mesh builder accepts shared_ptr
	boost::shared_ptr<CGeneral3DMesh> m_pGraphicsMesh;

	CGeneral3DMesh m_GraphicsMeshWithLightmap;

//	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

	CStaticGeometryArchive m_Archive;

	std::string m_DatabaseRelativeDirPathAtRuntime;

	/// subdivided graphics meshes
	/// - Each mesh meets the limit of vertex and index count of decent graphics cards
	/// - Created through 
	std::vector<boost::shared_ptr<CGeneral3DMesh>> m_vecpDestGraphicsMesh;

	std::vector<C3DMeshModelArchive> m_vecDestGraphicsMeshArchive;

	std::map<std::string,int> m_SurfaceNameToSurfaceDescIndex;

	std::map<std::string,CShaderInfo> m_ShaderNameToShaderInfo;

	/// why member variable
	/// -> Need to hold lightmap textures on memory in order to add them later to db
//	boost::shared_ptr<CLightmapBuilder> m_pLightmapBuilder;

protected:

	void AddDestGraphicsMeshInstance();
	
//	virtual void CreateMeshArchive();
	void CreateMeshSubsets_r( CAABTree<CIndexedPolygon>& src_tree,
							  int src_node_index,
							  CGeneral3DMesh& src_mesh,
					          CAABTree<CMeshSubset>& dest_tree );

	bool CompileGraphicsGeometry();

	bool CreateCollisionMesh();

	bool CreateLightmaps();

	void SaveToBinaryDatabase( const std::string& db_filename );

	void CopyTreeNodes_r( TerrainMeshTree& src_tree, TerrainMeshNode& node,
					  CNonLeafyAABTree<CIndexedPolygon>& dest_tree, int dest_node_index/*, CAABNode& dest_node*/ );

	void UpdateSurfaceNameToSurfaceGroupIndexMapping();

	bool SubdivideGraphicsMesh( CTerrainMeshGenerator& mesh_divider );


public:

	CStaticGeometryCompiler();

	virtual ~CStaticGeometryCompiler() {}

	/// returns true on success
	bool CompileFromXMLDescFile( const std::string& xml_filepath );

	const CStaticGeometryDesc& GetDesc() const { return m_Desc; }
};


#endif /* __StaticGeometryCompiler_H__ */
