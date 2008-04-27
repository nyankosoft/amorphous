#ifndef  __3DMath_AABTree_H__
#define  __3DMath_AABTree_H__


#include <vector>

#include "3DMath/Vector3.h"
#include "3DMath/AABB3.h"

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;


//===============================================================================
// CAABTree
//   - creates a mesh object that holds geometries specified as template class
//   - the template class TGeometry needs to implement the following functions
//   - TGeometry must also be serializable. i.e.) must be derived from IArchiveObjectBase
//===============================================================================

/*
class TGeometry : public IArchiveObjectBase
{
public:

	/// returns the axis-aligned bounding box in world space
	const AABB3& GetAABB() const;

	/// return if the ray hits the polygon
	bool ClipTrace( SRay& ray ) const;

	/// returns true if the polygon should be checked for collision
	bool IsCollidable() const;

}
*/

/**
 STMBinNode - binary tree node for triangle mesh
 - each non-leaf node holds an axis-aligned plane, which is represented by 'iAxis' & 'fDist'.
 - 'iAxis' represents the plane normal along the corresponding axis
*/
class CAABNode : public IArchiveObjectBase
{
public:

    /// axis of aa plane - 0:x, 1:y, 2:z
	int iAxis;

	/// distance of plane
	float fDist;

	/// aabb to hold subspace represented by this node
	AABB3 aabb;

	/// indices to child nodes
	int child[2];

	int iCellIndex;

	/// indices to geometries which belong to this node.
	/// - In a leafy binary tree,
	///   - Only the leaf nodes hold polygons, so this will be empty for non-leaf nodes
	///   - Geometries which cross subspaces belong to the both of the leaf nodes
	/// - In a non-leafy binary tree,
	///   - Geometries that cross a subspace belong to the internal nodes
	std::vector<int> veciGeometryIndex;

	int depth; ///< zero-origin depth

	static int ms_DefaultReserveSize;

	enum NodeType
	{
		LEAF = -1,
	};

public:

	CAABNode() : iCellIndex(-1), iAxis(0), fDist(0), depth(0) { child[0] = child[1] = LEAF; }

	inline bool IsLeaf() const { return ( (child[0] == LEAF) && (child[1] == LEAF) ); }

	inline void AddIndex( int index ) { veciGeometryIndex.push_back( index ); }

	inline void Clear() { veciGeometryIndex.resize(0); }

	inline void Serialize( IArchive& ar, const unsigned int version );
};


inline void CAABNode::Serialize( IArchive& ar, const unsigned int version )
{
	ar & iAxis;
	ar & fDist;
	ar & aabb;
	ar & child[0] & child[1];
	ar & iCellIndex;
	ar & veciGeometryIndex;
	ar & depth;
}


//===============================================================================
// CAABTree
//===============================================================================

template<class TGeometry>
class CAABTree : public IArchiveObjectBase
{
protected:

	std::vector<TGeometry> m_vecGeometry;

	std::vector<CAABNode> m_vecNode;

	int m_TreeDepth;

	/// world position of the tree
	Vector3 m_vWorldPos;

	/// controls recursion
	/// - RecursionStopCond::COND_AND or RecursionStopCond::COND_OR
	int m_RecursionStopCond;

	/// used to control recursion depth
	int m_MaxDepth;

	/// used to control recursion depth
	int m_iNumMaxTrianglesPerCell;

	/// used to control recursion depth
	float m_fMinimumCellVolume;

	/// used at runtime as a stack of node indices to check for collisions
	std::vector<int> m_vecNodeToCheck;

	enum RecursionStopCond
	{
		COND_AND, ///< i.e. loose recursion stopper
		COND_OR,  ///< i.e. strict recursion stopper
	};

private:

	inline void BuildLA_r( int index, int depth );

public:

	inline CAABTree();

	inline virtual ~CAABTree();

	inline void Release();

	std::vector<TGeometry>& GetGeometryBuffer() { return m_vecGeometry; }

	/// creates an empty tree
	/// - Declare as virtual and have derived class call this
	///   - Avoid name conflicts with Build( const std::vector<TGeometry>& vecGeometry ) below.
	inline virtual void Build( const AABB3& rBoundingBox, const int depth );

	void Build( const std::vector<TGeometry>& vecGeometry )
	{
		m_vecGeometry = vecGeometry;

		Build();
	}

	// creates the tree from 'm_vecGeometry'
	virtual void Build() = 0;

	/// set tree position in world space
	void SetWorldPosition( const Vector3& rvWorldPos ) { m_vWorldPos = rvWorldPos; }

	const CAABNode& GetNode( int index ) const { return m_vecNode[index]; }

	int GetNumNodes() const { return (int)m_vecNode.size(); }

	void SetMaxDepth( int max_depth ) { m_MaxDepth = max_depth; }

	void SetMinimumCellVolume( float volume ) { m_fMinimumCellVolume = volume; }

	void SetNumMaxTrianglesPerCell( int num_max_triangles ) { m_iNumMaxTrianglesPerCell = num_max_triangles; }

	inline void SetRecursionStopCondition( const string& cond );

	inline bool ShouldStopRecursion( int depth, float sub_space_volume, int num_triangles_in_cell );

//	inline int AddAABB( const AABB3& aabb );

	/// register a geometry with the specified index
//	inline void AddGeometry( const TGeometry& geom, int index )

	/// get a list of geometry indices which intersect with 'raabb'
	/// - Checks overlaps between abbb and geometries
	/// \param [in] aabb
	/// \param [out] rvecDestIndex dest buffer that holds indices of geometries
	///              intersecting with aabb
	inline void GetIntersectingGeometries( const AABB3& aabb, std::vector<int>& rvecDestIndex );

	/// get a list of geometry indices whose aabb intersect with 'aabb'
	/// - Checks overlaps between abbb and geometry aabbs
	/// - Does not mean geometry itself intersects with the aabb
	/// - Calls TGeometry::GetAABB() inside
	/// \param [in] aabb
	/// \param [out] dest buffer to store indices
	virtual void GetIntersectingAABBs( const AABB3& aabb, std::vector<int>& rvecDestIndex ) = 0;

	/// get AABBs which possibly include the specified position 'vPos'
	inline void GetPossiblyIntersectingAABBs( const Vector3& vPos, std::vector<int>& rvecDestIndex );

	inline void AddGeometry( const TGeometry& geom );

	/// clear geometry indices from nodes
	/// - Does not remove the geometry instances. Only the indices in the nodes
	inline void ResetRegisteredGeometries();

	/// - Call ResetRegisteredGeometries()
	/// - Link geoms to the tree nodes
	inline void UpdateRegisteredGeometries();

	virtual void LinkGeometry( int geom_index ) = 0;

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


/**
 - leafy binary tree
 - Not thread safe
   - See m_TestCounter and how it is used
*/
template<class TGeometry>
class CLeafyAABTree : public CAABTree<TGeometry>
{
	int m_TestCounter;

	// test counters for each geometry
	std::vector<int> m_vecTestCounter;

public:

	CLeafyAABTree() : m_TestCounter(0) {}

	virtual ~CLeafyAABTree() {}

	void Build( const AABB3& rBoundingBox, const int depth ) { CAABTree::Build( rBoundingBox, depth ); }

	/// \param [in] vecGeometry copied and stored
	inline void Build();

	inline virtual void GetIntersectingAABBs( const AABB3& aabb, std::vector<int>& rvecDestIndex );

	inline virtual void LinkGeometry( int geom_index ) {}

	inline void Serialize( IArchive& ar, const unsigned int version );
};


template<class TGeometry>
class CNonLeafyAABTree : public CAABTree<TGeometry>
{
public:

	CNonLeafyAABTree() {}

	virtual ~CNonLeafyAABTree() {}

	/// update the link of the geometry[index]
	inline void UpdateGeometry( int index );

	void Build( const AABB3& rBoundingBox, const int depth ) { CAABTree::Build( rBoundingBox, depth ); }

	/// \param [in] vecGeometry copied and stored
	inline void Build()
	{
		assert( !"Not implemented\n" );
	}

	inline virtual void GetIntersectingAABBs( const AABB3& aabb, std::vector<int>& rvecDestIndex )
	{
		assert( !"Not implemented\n" );
	}

	inline virtual void LinkGeometry( int geom_index );
};


#include "AABTree.inl"


#endif		/*  __3DMath_AABTree_H__  */
