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

	CGeneral3DMesh m_GraphicsMesh;

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

protected:

	void AddDestGraphicsMeshInstance();
	
//	virtual void CreateMeshArchive();
	void CreateMeshSubsets_r( CAABTree<CIndexedPolygon>& src_tree,
							  int src_node_index,
							  CGeneral3DMesh& src_mesh,
					          CAABTree<CMeshSubset>& dest_tree );

	bool CompileGraphicsGeometry();

	bool CreateCollisionMesh();

	void SaveToBinaryDatabase( const std::string& db_filename );

public:

	CStaticGeometryCompiler();

	virtual ~CStaticGeometryCompiler() {}

	/// returns true on success
	bool CompileFromXMLDescFile( const std::string& xml_filepath );

	const CStaticGeometryDesc& GetDesc() const { return m_Desc; }
};


#endif /* __StaticGeometryCompiler_H__ */
