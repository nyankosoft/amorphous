#ifndef  __TerrainMeshGenerator_H__
#define  __TerrainMeshGenerator_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "../fwd.hpp"
#include "../IndexedPolygon.hpp"
#include "3DMath/AABB3.hpp"


namespace amorphous
{


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

public:

	TerrainMeshNode()
		:
	m_Axis(-1)
	{
	}

	bool IsLeaf() const { return ( m_child.size() == 0 ); }

	inline int GetNumNodes_r();
};


inline int TerrainMeshNode::GetNumNodes_r()
{
	int num_child_nodes = 0;
	for( size_t i=0; i<m_child.size(); i++ )
		num_child_nodes += m_child[i].GetNumNodes_r();

	return num_child_nodes + 1;
}



/**
 * creates a binary tree that stores polygons in its root nodes
 *
 */
class TerrainMeshTree
{
	TerrainMeshNode m_RootNode;

	int m_TargetDepth;

	int m_TargetMaterialIndex;

	/// stores the mesh archives created from the triangles
	/// in tree nodes
//	std::vector<C3DMeshModelArchive> m_vecMeshArchive;

	boost::shared_ptr<General3DMesh> m_pSrcMesh;

	boost::shared_ptr<General3DMesh> m_pDestMesh;

	int m_NumMaxTrianglesPerMesh;

	int m_NumMaxVerticesPerMesh;

	int m_NumMaxBatchNodes;

	double m_TexCoordShiftU;
	double m_TexCoordShiftV;

	std::string m_OutputTextureImageFormat;

	/// directory to put image files of split textures
	std::string m_TextureOutputDirectory;

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

public:

	TerrainMeshTree();

	/// Build mesh tree.
	/// Remember that the content of the polygon buffer may be changed
	/// since polygons that cross subdivision planes will be split
	/// and new polygons will be added to the buffer.
	bool Build( boost::shared_ptr<General3DMesh> pSrcMesh, int target_depth );

	void ScaleTexCoords();

	void SetTextureIndices( int num_tex_edge_splits ) { SetTextureIndices_r(m_RootNode,num_tex_edge_splits); }

	/// create mesh archives from polygons stored in the leaf nodes
	void MakeMesh();

	void SetTargetMaterialIndex( int mat_index ) { m_TargetMaterialIndex = mat_index; }

	int GetTargetMaterialIndex() const { return m_TargetMaterialIndex; }

	void SetTerrainTexCoordShiftU( double shift_u ) { m_TexCoordShiftU = shift_u; }
	void SetTerrainTexCoordShiftV( double shift_v ) { m_TexCoordShiftV = shift_v; }

	TerrainMeshNode& GetRootNode() { return m_RootNode; }

	int GetNumNodes() { return m_RootNode.GetNumNodes_r(); }

	boost::shared_ptr<General3DMesh> GetDestMesh() { return m_pDestMesh; }

	// extension of the output texture image file
	void SetOutputTextureImageFormat( const std::string& image_ext );

	std::string CreateSubdividedTextureFilepath( const std::string& src_tex_filename, int index );

	/// directory path where texture image files are saved
	void SetTextureOutputDirectory( const std::string& tex_output_dir );
};




class CTerrainMeshGenerator
{
	boost::shared_ptr<General3DMesh> m_pSrcMesh;

	TerrainMeshTree m_MeshTree;

	/// width & height of the split textures
	int m_TextureWidth;

	int m_NumTexEdgeSplits;

	std::string m_OutputTextureRelativePath;

private:

	/// subdivide textures
	bool SplitTexture( const std::string& src_tex_filename );

	void CopyVerticesAndTriangles( C3DMeshModelArchive& src_mesh );

	void CreateMeshTree();

	void ScaleTextureCoordinates();

public:

	CTerrainMeshGenerator();

	~CTerrainMeshGenerator();

	bool BuildTerrainMesh( boost::shared_ptr<General3DMesh> pSrcMesh );

//	std::vector<C3DMeshModelArchive>& GetMeshArchive() { return m_MeshTree.GetMeshArchive(); }
//	size_t GetNumMeshArchives() { return m_MeshTree.GetMeshArchive().size(); }
//	const C3DMeshModelArchive& GetMeshArchive( int index ) { return m_MeshTree.GetMeshArchive()[index]; }
//	void GetMeshArchive( std::vector<C3DMeshModelArchive>& dest_mesh ) { dest_mesh = m_MeshTree.GetMeshArchive(); }

	/// directory path used for texture filename of the mesh archives
	void SetOutputTextureRelativePath( const std::string& pathname ) { m_OutputTextureRelativePath = pathname; }

	void SetOutputTextureFormat( const std::string& image_ext );

	void SetTextureOutputDirectory( const std::string& tex_output_dir ) { m_MeshTree.SetTextureOutputDirectory( tex_output_dir ); }

	TerrainMeshTree& GetMeshTree() { return m_MeshTree; }

	boost::shared_ptr<General3DMesh> GetDestMesh() { return m_MeshTree.GetDestMesh(); }

	bool SaveToFiles();

	void SetTargetMaterialIndex( int mat_index ) { m_MeshTree.SetTargetMaterialIndex( mat_index ); }

	/// sets the size(width & height) of the each split texture
	void SetSplitTextureWidth( int width ) { m_TextureWidth = width; }

	void SetTerrainTexCoordShiftU( double shift_u ) { m_MeshTree.SetTerrainTexCoordShiftU( shift_u ); }
	void SetTerrainTexCoordShiftV( double shift_v ) { m_MeshTree.SetTerrainTexCoordShiftV( shift_v ); }
};


} // namespace amorphous



#endif		/*  __TerrainMeshGenerator_H__  */
