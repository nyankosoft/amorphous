#ifndef  __TerrainMeshGenerator_H__
#define  __TerrainMeshGenerator_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "../fwd.h"
#include "../IndexedPolygon.h"
#include "3DMath/aabb3.h"
#include "3DMeshModelArchive.h"
using namespace MeshModel;


/**
 * Leaf nodes are used to store polygons contained in their subspace
 *
 */
class TerrainMeshNode
{
public:

	AABB3 m_AABB;

	std::vector<TerrainMeshNode> m_child;

	int m_Axis;

	/// holds indices to the polygons currently linked to this node
	std::vector<int> m_vecPolygonIndex;

	int m_TextureIndex;

	bool IsLeaf() const { return ( m_child.size() == 0 ); }

public:

	TerrainMeshNode()
		:
	m_Axis(-1)
	{
	}
};

/**
 * creates a binary tree that stores polygons in its root nodes
 *
 */
class TerrainMeshTree
{
	TerrainMeshNode m_RootNode;

	int m_TargetDepth;

//	boost::shared_ptr<std::vector<CGeneral3DVertex>> m_pVertexBuffer;

//	std::vector<CIndexedPolygon>* m_pvecPolygonBuffer;

	/// stores the mesh archives created from the triangles
	/// in tree nodes
//	std::vector<C3DMeshModelArchive> m_vecMeshArchive;

	boost::shared_ptr<CGeneral3DMesh> m_pSrcMesh;

	boost::shared_ptr<CGeneral3DMesh> m_pDestMesh;

	int m_NumMaxTrianglesPerMesh;

	int m_NumMaxVerticesPerMesh;

	int m_NumMaxBatchNodes;

	std::string m_BaseTextureFilename;

	double m_TexCoordShiftU;
	double m_TexCoordShiftV;

	std::string m_OutputTextureImageFormat;

private:

	void MakeMeshNodes_r( TerrainMeshNode& node, int axis, int depth );

    void ScaleTexCoords_r( TerrainMeshNode& node );

	void MakeMesh_r( TerrainMeshNode& node,
					 int& num_current_nodes,
					 int& num_current_vertices,
					 int& num_current_triangles );

	/// If the node is leaf, add the polygons stored in the node to m_pDestMesh.
	/// If the node is not leaf, recurse down to child nodes.
	/// One surface material per node
    void AddToDestMesh( TerrainMeshNode& node );

	void SetTextureIndices_r( TerrainMeshNode& node, int num_tex_edge_splits );

//	void Triangulate_r( TerrainMeshNode& node,
//		                std::vector<CIndexedPolygon>& dest_triangle_buffer);

public:

	TerrainMeshTree();

	bool Build( boost::shared_ptr<CGeneral3DMesh> pSrcMesh, int target_depth );

	/// build mesh tree
	/// remember that the content of the polygon buffer may be changed
	/// since polygons that cross subdivision planes will be split
	/// and new polygons will be added to the buffer
//	bool Build( boost::shared_ptr<std::vector<CGeneral3DVertex>> pVertexBuffer,
//		        std::vector<CIndexedPolygon>& vecPolygonBuffer,
//				int target_depth );

	void ScaleTexCoords();

	void SetTextureIndices( int num_tex_edge_splits ) { SetTextureIndices_r(m_RootNode,num_tex_edge_splits); }

	/// create mesh archives from polygons stored in the leaf nodes
	void MakeMesh();

	void SetBaseTextureFilename( const std::string& filename ) { m_BaseTextureFilename = filename; }

//	std::vector<C3DMeshModelArchive>& GetMeshArchive() { return m_vecMeshArchive; }

	void SetTerrainTexCoordShiftU( double shift_u ) { m_TexCoordShiftU = shift_u; }
	void SetTerrainTexCoordShiftV( double shift_v ) { m_TexCoordShiftV = shift_v; }

	TerrainMeshNode& GetRootNode() { return m_RootNode; }

	boost::shared_ptr<CGeneral3DMesh> GetDestMesh() { return m_pDestMesh; }

	// extension of the output texture image file
	void SetOutputTextureImageFormat( const std::string& tex_image_format ) { m_OutputTextureImageFormat = tex_image_format; }
};




class CTerrainMeshGenerator
{
	boost::shared_ptr<CGeneral3DMesh> m_pSrcMesh;

//	boost::shared_ptr<std::vector<CGeneral3DVertex>> m_pVertexBuffer;

	// stores triangled copied from the source mesh
	// m_MaterialIndex is not used
//	std::vector<CIndexedPolygon> m_vecPolygonBuffer;

	TerrainMeshTree m_MeshTree;

	/// width & height of the split textures
	int m_TextureWidth;

	int m_NumTexEdgeSplits;

	/// directory to put image files of split textures
	std::string m_TextureOutputDirectory;

	std::string m_OutputTextureImageFormat;

	std::string m_OutputTextureRelativePath;

	/// save output texture filenames so that static geometry compiler
	/// can retrieve them and add them to a binary database
	/// - 18:48 2008-01-13 changed: static geometry compiler retrieves texture files
	///                    from CStaticGeometryCompilerFG::m_vecMeshArchive
//	std::vector<std::string> m_vecOutputTextureFilename;

private:

	/// subdivide textures
	bool SplitTexture( const string& src_tex_filename );

    void CopyVerticesAndTriangles( C3DMeshModelArchive& src_mesh );

	void CreateMeshTree();

	void ScaleTextureCoordinates();

public:

	CTerrainMeshGenerator();

	~CTerrainMeshGenerator();

	bool BuildTerrainMesh( boost::shared_ptr<CGeneral3DMesh> pSrcMesh );

//	std::vector<C3DMeshModelArchive>& GetMeshArchive() { return m_MeshTree.GetMeshArchive(); }

//	size_t GetNumMeshArchives() { return m_MeshTree.GetMeshArchive().size(); }

//	const C3DMeshModelArchive& GetMeshArchive( int index ) { return m_MeshTree.GetMeshArchive()[index]; }

//	void GetMeshArchive( std::vector<C3DMeshModelArchive>& dest_mesh ) { dest_mesh = m_MeshTree.GetMeshArchive(); }

	/// directory path where texture image files are saved
	void SetTextureOutputDirectory( const std::string& tex_output_dir );

	/// directory path used for texture filename of the mesh archives
	void SetOutputTextureRelativePath( const std::string& pathname ) { m_OutputTextureRelativePath = pathname; }

	void SetOutputTextureFormat( const std::string& image_ext );

//	int GetNumOutputTextures() { return m_vecOutputTextureFilename.size(); }

//	const std::string& GetNumOutputTextureFilename( int index ) const { return m_vecOutputTextureFilename[index]; }

	TerrainMeshTree& GetMeshTree() { return m_MeshTree; }

	boost::shared_ptr<CGeneral3DMesh> GetDestMesh() { return m_MeshTree.GetDestMesh(); }

	bool SaveToFiles();

	/// sets the size(width & height) of the each split texture
	void SetSplitTextureWidth( int width ) { m_TextureWidth = width; }

	void SetTerrainTexCoordShiftU( double shift_u ) { m_MeshTree.SetTerrainTexCoordShiftU( shift_u ); }
	void SetTerrainTexCoordShiftV( double shift_v ) { m_MeshTree.SetTerrainTexCoordShiftV( shift_v ); }
};



#endif		/*  __TerrainMeshGenerator_H__  */