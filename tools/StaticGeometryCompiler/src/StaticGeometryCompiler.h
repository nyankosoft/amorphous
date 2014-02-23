#ifndef __StaticGeometryCompiler_H__
#define __StaticGeometryCompiler_H__


#include <boost/shared_ptr.hpp>
#include "StaticGeometryDesc.hpp"
#include "App/ApplicationBase.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"
#include "Graphics/MeshModel/TerrainMeshGenerator.hpp"
#include "Stage/StaticGeometry.hpp"
#include "Stage/StaticGeometryArchiveFG.hpp"
#include "3DMath/AABTree.hpp"
#include "Physics/Stream.hpp"

using namespace amorphous;


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

	General3DMesh m_CollisionMesh;

	/// stores collision mesh data preprocessed by the physics engine
	physics::CStream m_CollisionMeshStream;

	/// changed to shared_ptr since terrain mesh builder accepts shared_ptr
	boost::shared_ptr<General3DMesh> m_pGraphicsMesh;

	General3DMesh m_GraphicsMeshWithLightmap;

//	CNonLeafyAABTree<CMeshSubset> m_MeshSubsetTree;

	CStaticGeometryArchive m_Archive;

	std::string m_DatabaseRelativeDirPathAtRuntime;

	/// subdivided graphics meshes
	/// - Each mesh meets the limit of vertex and index count of decent graphics cards
	/// - Created through 
	std::vector< boost::shared_ptr<General3DMesh> > m_vecpDestGraphicsMesh;

	std::vector<C3DMeshModelArchive> m_vecDestGraphicsMeshArchive;

	std::map<std::string,int> m_SurfaceNameToSurfaceDescIndex;

	std::map<std::string,CShaderInfo> m_ShaderNameToShaderInfo;

	/// why member variable
	/// -> Need to hold lightmap textures on memory in order to add them later to db
//	boost::shared_ptr<CLightmapBuilder> m_pLightmapBuilder;

protected:

	void AddDestGraphicsMeshInstance();
	
//	virtual void CreateMeshArchive();
	void CreateMeshSubsets_r( AABTree<IndexedPolygon>& src_tree,
							  int src_node_index,
							  General3DMesh& src_mesh,
					          AABTree<CMeshSubset>& dest_tree );

	bool CompileGraphicsGeometry();

	bool CreateCollisionMesh();

	bool CreateLightmaps();

	void SaveToBinaryDatabase( const std::string& db_filename );

	void CopyTreeNodes_r( TerrainMeshTree& src_tree, TerrainMeshNode& node,
					  CNonLeafyAABTree<IndexedPolygon>& dest_tree, int dest_node_index/*, AABNode& dest_node*/ );

	void UpdateSurfaceNameToSurfaceGroupIndexMapping();

	bool SubdivideGraphicsMesh( CTerrainMeshGenerator& mesh_divider );

	void SetShaderParameterGroups();

public:

	CStaticGeometryCompiler();

	virtual ~CStaticGeometryCompiler() {}

	/// returns true on success
	bool CompileFromXMLDescFile( const std::string& xml_filepath );

	const CStaticGeometryDesc& GetDesc() const { return m_Desc; }
};


#endif /* __StaticGeometryCompiler_H__ */
