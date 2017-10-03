#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"


namespace amorphous
{

//============================ inline implementations ============================

template<class TGeometry>
inline AABTree<TGeometry>::AABTree()
:
m_TreeDepth(0),
m_vWorldPos(Vector3(0,0,0)),
m_MaxDepth(0xFFFF),
m_iNumMaxTrianglesPerCell(8),
m_fMinimumCellVolume(8.0f),
m_RecursionStopCond(COND_OR)
{
}


template<class TGeometry>
inline AABTree<TGeometry>::~AABTree()
{
}


template<class TGeometry>
inline void AABTree<TGeometry>::Release()
{
	m_vecGeometry.resize( 0 );
	m_vecNode.resize( 0 );
}

template<class TGeometry>
inline void AABTree<TGeometry>::SetRecursionStopCondition( const std::string& cond )
{
	if( cond == "||" || cond == "or" )			m_RecursionStopCond = COND_OR;
	else if( cond == "&&" || cond == "and" )	m_RecursionStopCond = COND_AND;
	else					m_RecursionStopCond = COND_AND;
}

template<class TGeometry>
inline bool AABTree<TGeometry>::ShouldStopRecursion( int depth,
										   float sub_space_volume,
										   int num_triangles_in_cell )
{
	if( m_RecursionStopCond == COND_AND )
	{
		return ( m_MaxDepth < depth
			  && sub_space_volume < m_fMinimumCellVolume
			  && num_triangles_in_cell < m_iNumMaxTrianglesPerCell );
	}
	else if( m_RecursionStopCond == COND_OR )
	{
		return ( m_MaxDepth < depth
		      || sub_space_volume < m_fMinimumCellVolume
		      || num_triangles_in_cell < m_iNumMaxTrianglesPerCell );
	}
	else
		return true;
}


template<class TGeometry>
inline void AABTree<TGeometry>::BuildLA_r( int index, int depth )
{
	if( m_TreeDepth <= depth )
		return;

	AABNode& rNode = m_vecNode[index];

	Vector3 vExtents = rNode.aabb.GetExtents();
	if( vExtents[1] < vExtents[0] )
	{
		if( vExtents[2] < vExtents[0] )
			rNode.iAxis = 0;
		else
			rNode.iAxis = 2;
	}
	else
	{
		if( vExtents[2] < vExtents[1] )
			rNode.iAxis = 1;
		else
			rNode.iAxis = 2;
	}

	rNode.fDist = rNode.aabb.GetCenterPosition()[rNode.iAxis];

	AABNode child_node[2];
	child_node[0] = child_node[1] = rNode;

	child_node[0].aabb.vMax[rNode.iAxis] = rNode.fDist;
	child_node[1].aabb.vMin[rNode.iAxis] = rNode.fDist;

	int child_index[2];
	child_index[0] = (int)m_vecNode.size();
	child_index[1] = (int)m_vecNode.size() + 1;

	rNode.child[0] = child_index[0];
	rNode.child[1] = child_index[1];

	m_vecNode.push_back( child_node[0] );
	m_vecNode.push_back( child_node[1] );

	BuildLA_r( child_index[0], depth+1 );
	BuildLA_r( child_index[1], depth+1 );
}


template<class TGeometry>
inline void AABTree<TGeometry>::Build( const AABB3& rBoundingBox, const int depth )
{
	m_vecNode.clear();
	m_vecNode.reserve( (size_t)pow( 2.0, (double)depth ) );

	m_TreeDepth = depth;

	AABNode root_node;
	root_node.aabb = rBoundingBox;

	// set the seed of the tree
	m_vecNode.push_back( root_node );

	BuildLA_r( 0, 1 );

}


template<class TGeometry>
inline void AABTree<TGeometry>::ResetRegisteredGeometries()
{
	const size_t num_nodes = m_vecNode.size();
	for( size_t i=0; i<num_nodes; i++ )
	{
		m_vecNode[i].Clear();
	}
}


template<class TGeometry>
inline void AABTree<TGeometry>::UpdateRegisteredGeometries()
{
	ResetRegisteredGeometries();

	const size_t num_geoms = m_vecGeometry.size();
	for( size_t i=0; i<num_geoms; i++ )
	{
		LinkGeometry( (int)i );
	}
}


template<class TGeometry>
inline void LeafyAABTree<TGeometry>::GetIntersectingAABBs( const AABB3& aabb, std::vector<int>& rvecDestIndex )
{
	size_t i;

	std::vector<int>& nodes_to_check = AABTree<TGeometry>::m_vecNodeToCheck;

	nodes_to_check.resize(0);

	nodes_to_check.push_back(0);

	while( !nodes_to_check.empty() )
	{
		AABNode& rNode =  AABTree<TGeometry>::m_vecNode[ nodes_to_check.back() ];
		nodes_to_check.pop_back();

		if( rNode.IsLeaf() )
		{
			// check intersection with triangles in this cell
			for( i=0; i<rNode.veciGeometryIndex.size(); i++ )
			{
				const int geom_index = rNode.veciGeometryIndex[i];
				const TGeometry& geom = AABTree<TGeometry>::m_vecGeometry[ geom_index ];

				if( m_TestCounter == m_vecTestCounter[ geom_index ] )
					continue;	// already tested

				m_vecTestCounter[ geom_index ] = m_TestCounter;

				if( aabb.IsIntersectingWith( geom.GetAABB() ) )
					rvecDestIndex.push_back( geom_index );
			}
		}
		else
		{
			if( aabb.vMin[rNode.iAxis] <= rNode.fDist )
			{
				// 'aabb' is intersecting with the negative half-space
				nodes_to_check.push_back( rNode.child[1] );
			}

			if( rNode.fDist <= aabb.vMax[rNode.iAxis] )
			{
				// 'aabb' is intersecting with the positive half-space
				nodes_to_check.push_back( rNode.child[0] );
			}
		}
	}

	// increment test counter
	m_TestCounter++;
}


/// for leafy and non-leafy
template<class TGeometry>
inline void AABTree<TGeometry>::GetPossiblyIntersectingAABBs( const Vector3& vPos, std::vector<int>& rvecDestIndex )
{
	int index = 0;

	size_t i, num;

	const Vector3 vTreePos = m_vWorldPos;

	while(1)
	{
		const AABNode& rNode = m_vecNode[index];

		// get the indices to AABBs in this subspace
		num = rNode.veciGeometryIndex.size();
		for( i=0; i<num; i++ )
		{
			rvecDestIndex.push_back( rNode.veciGeometryIndex[i] );
		}

		if( rNode.IsLeaf() )
			return;

		if( vPos[rNode.iAxis] < vTreePos[rNode.iAxis] + rNode.fDist )
		{
			index = rNode.child[0];
		}
		else // i.e. ( vTreePos[rNode.iAxis] + rNode.fDist < vPos[rNode.iAxis] )
		{
			index = rNode.child[1];
		}
	}
}


template<class TGeometry>
inline void AABTree<TGeometry>::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecGeometry;

	ar & m_vecNode;

	ar & m_TreeDepth;

	ar & m_vWorldPos;

	ar & m_MaxDepth;

	ar & m_iNumMaxTrianglesPerCell;
	ar & m_fMinimumCellVolume;
	ar & m_RecursionStopCond;
}


template<class TGeometry>
inline void AABTree<TGeometry>::AddGeometry( const TGeometry& geom )
{
	size_t geom_index = m_vecGeometry.size();
	m_vecGeometry.push_back( geom );

	LinkGeometry( (int)geom_index );
}


inline int GetSplitPlaneAxis( const AABB3& aabb )
{
	Vector3 vLen = aabb.vMax - aabb.vMin;
	if( vLen.x < vLen.y )
	{
		if( vLen.y < vLen.z )
			return 2;//vLen.z;
		else
			return 1;//vLen.y;
	}
	else
	{
		if( vLen.z < vLen.x )
			return 0;//vLen.x;
		else
			return 2;//vLen.z;
	}
}

/**
 post condition: m_vecNode.size() == 1, and all the geometries are stored in the root node.
*/
template<class TGeometry>
inline void AABTree<TGeometry>::InitRootNode()
{
	// release any previous tree nodes
	m_vecNode.resize( 0 );

	// create aabb for the root node
	AABB3 root_aabb;
	root_aabb.Nullify();
	const size_t num_geoms = m_vecGeometry.size();

	LOG_PRINT( fmt_string(" - %d geometries",num_geoms) );

	for(size_t i=0; i<num_geoms; i++)
	{
		root_aabb.MergeAABB( m_vecGeometry[i].GetAABB() );
	}

	LOG_PRINT( "root_aabb: " + to_string(root_aabb) );

	m_vecNode.reserve(2048);

	// set up the root node
	m_vecNode.push_back( AABNode() );

	// set plane for the root node
	m_vecNode[0].aabb = root_aabb;
	int iFirstAxis = GetSplitPlaneAxis( m_vecNode[0].aabb );
	m_vecNode[0].iAxis = iFirstAxis;
	m_vecNode[0].fDist = (root_aabb.vMax[iFirstAxis] + root_aabb.vMin[iFirstAxis]) / 2.0f;

	m_vecNode[0].depth = 0; // record depth

	// node (push all the triangles to the root)
	m_vecNode[0].veciGeometryIndex.reserve( num_geoms );
	for(size_t i=0; i<num_geoms; i++)
	{
		m_vecNode[0].veciGeometryIndex.push_back((int)i);
	}
}


template<class TGeometry>
inline void LeafyAABTree<TGeometry>::Build()
{
	if( AABTree<TGeometry>::m_vecGeometry.size() == 0 )
		return;

	AABTree<TGeometry>::InitRootNode();

	// stack of nodes we need to process
	std::vector<int> veciNodeToProcess;
	veciNodeToProcess.reserve( 256 );
	veciNodeToProcess.push_back(0);	// put the root node

	std::vector<AABNode>& nodes = AABTree<TGeometry>::m_vecNode;

	size_t j;
	while( !veciNodeToProcess.empty() )
	{
		int iNodeIndex = veciNodeToProcess.back();
		veciNodeToProcess.pop_back();

		// split into 2 subspaces if there are more triangles in the current subspace than 'm_iNumMaxTrianglesPerCell'
		// and the volume of the current subspace cell is larger than 'm_fMinimumCellVolume'
//		if( (size_t)m_iNumMaxTrianglesPerCell < m_vecNode[iNodeIndex].veciGeometryIndex.size()
//		 && m_fMinimumCellVolume < m_vecNode[iNodeIndex].aabb.GetVolume() )
		if( !AABTree<TGeometry>::ShouldStopRecursion(
			nodes[iNodeIndex].depth,
			nodes[iNodeIndex].aabb.GetVolume(),
			(int)nodes[iNodeIndex].veciGeometryIndex.size() ) )
		{
			// need to split
			AABNode& current_node = nodes[iNodeIndex];

			int iAxis = nodes[iNodeIndex].iAxis;
			float fMidDist = ( nodes[iNodeIndex].aabb.vMax[iAxis] + nodes[iNodeIndex].aabb.vMin[iAxis] ) / 2.0f;
			for( size_t i=0; i<2; i++ )
			{
				nodes[iNodeIndex].child[i] = (int)nodes.size();
				veciNodeToProcess.push_back( (int)nodes.size() );	// add to the stack

				// add a new node
				nodes.push_back( AABNode() );

				// axis of the plane (will not be used when this turns out to be a leaf node)
//				m_vecNode.back().iAxis = (iAxis + 1) % 3;

				// set aabb which represents subspace of the child nodes
				nodes.back().aabb = nodes[iNodeIndex].aabb;
				if( i==0 )
					nodes.back().aabb.vMin[iAxis] = fMidDist;
				else
					nodes.back().aabb.vMax[iAxis] = fMidDist;

				nodes.back().iAxis = GetSplitPlaneAxis( nodes.back().aabb );

				nodes.back().fDist = ( nodes.back().aabb.vMin[nodes.back().iAxis]
					                     + nodes.back().aabb.vMax[nodes.back().iAxis] ) / 2.0f;

				nodes.back().depth = nodes[iNodeIndex].depth + 1;

				// Move all the triangles of 'm_vecNode[iNodeIndex]' to either of its children.
				// Triangles which cross the plane are handed to both of the children
				for( j=0; j<nodes[iNodeIndex].veciGeometryIndex.size(); j++ )
				{
					TGeometry& geom = AABTree<TGeometry>::m_vecGeometry[ nodes[iNodeIndex].veciGeometryIndex[j] ];
					if( nodes.back().aabb.IsIntersectingWith( geom.GetAABB() ) )
						nodes.back().veciGeometryIndex.push_back( nodes[iNodeIndex].veciGeometryIndex[j] );
				}
			}

//			RecordNode( m_vecNode[iNodeIndex], "split a node" );

			// triangles are stored to child nodes - remove them from the parent node
			nodes[iNodeIndex].veciGeometryIndex.clear();
		}
		else
		{
			const AABNode& current_node = nodes[iNodeIndex];
//			PERIODICAL( 256, RecordLeafNode( current_node ) );
///			RecordNode( current_node, "created a leaf node" );

//			UpdateLeafNodeStatistics( current_node );
		}
	}
}


template<class TGeometry>
inline void LeafyAABTree<TGeometry>::Serialize( IArchive& ar, const unsigned int version )
{
	AABTree<TGeometry>::Serialize( ar, version );

	ar & m_TestCounter;

	if( ar.GetMode() == IArchive::MODE_INPUT )
		m_vecTestCounter.resize( AABTree<TGeometry>::m_vecGeometry.size(), 0 );
}


template<class TGeometry>
inline void CNonLeafyAABTree<TGeometry>::Build()
{
	std::vector<TGeometry>& geometries = AABTree<TGeometry>::m_vecGeometry;

	if( geometries.size() == 0 )
		return;

	AABTree<TGeometry>::InitRootNode();

	std::vector<AABNode>& nodes = AABTree<TGeometry>::m_vecNode;

	// stack of nodes we need to process
	std::vector<int> veciNodeToProcess;
	veciNodeToProcess.reserve( 256 );
	veciNodeToProcess.push_back(0);	// put the root node

	std::vector<int> veciGeometryIndex;
	veciGeometryIndex.reserve( geometries.size() );
	size_t j;
	while( !veciNodeToProcess.empty() )
	{
		int iNodeIndex = veciNodeToProcess.back();
		veciNodeToProcess.pop_back();

		// split into 2 subspaces if there are more triangles in the current subspace than 'm_iNumMaxTrianglesPerCell'
		// and the volume of the current subspace cell is larger than 'm_fMinimumCellVolume'
//		if( (size_t)m_iNumMaxTrianglesPerCell < m_vecNode[iNodeIndex].veciGeometryIndex.size()
//		 && m_fMinimumCellVolume < m_vecNode[iNodeIndex].aabb.GetVolume() )
		if( !AABTree<TGeometry>::ShouldStopRecursion(
			nodes[iNodeIndex].depth,
			nodes[iNodeIndex].aabb.GetVolume(),
			(int)nodes[iNodeIndex].veciGeometryIndex.size() ) )
		{
			// need to split
			AABNode& current_node = nodes[iNodeIndex];

			int iAxis = nodes[iNodeIndex].iAxis;
			float fMidDist = nodes[iNodeIndex].aabb.GetCenterPosition()[iAxis];
			for( size_t i=0; i<2; i++ )
			{
				nodes[iNodeIndex].child[i] = (int)nodes.size();
				veciNodeToProcess.push_back( (int)nodes.size() );	// add to the stack

				// add a new node
				nodes.push_back( AABNode() );
				AABNode& new_node = nodes.back();

				new_node.veciGeometryIndex.reserve( nodes[iNodeIndex].veciGeometryIndex.size() / 2 );

				// set aabb which represents subspace of the child nodes
				new_node.aabb = nodes[iNodeIndex].aabb;
				if( i==0 )
					new_node.aabb.vMin[iAxis] = fMidDist;
				else
					new_node.aabb.vMax[iAxis] = fMidDist;

				new_node.iAxis = GetSplitPlaneAxis( new_node.aabb );

				new_node.fDist = ( new_node.aabb.vMin[new_node.iAxis] + new_node.aabb.vMax[new_node.iAxis] ) / 2.0f;

				new_node.depth = nodes[iNodeIndex].depth + 1;
			}

			// added the 2 child nodes
			// place geometris(such as polygons) to
			// 1. current node when the polygon cross the current split plane
			// 2. child node[0] 
			// 3. child node[1]
			int child_node_index[2];
			child_node_index[0] = (int)nodes.size() - 1;
			child_node_index[1] = (int)nodes.size() - 2;

			// copy all indices in the current node
			veciGeometryIndex = nodes[iNodeIndex].veciGeometryIndex;
			nodes[iNodeIndex].veciGeometryIndex.resize( 0 );

			// Move all the triangles of 'm_vecNode[iNodeIndex]' to either of its children,
			// or move back to itself if it crosses the plane.
			const size_t num_geoms = veciGeometryIndex.size();
			for( j=0; j<num_geoms; j++ )
			{
				TGeometry& geom = geometries[ veciGeometryIndex[j] ];

				const int geom_index = veciGeometryIndex[j];

				if( geom.GetAABB().vMax[iAxis] < fMidDist )
				{
					nodes[child_node_index[0]].veciGeometryIndex.push_back( geom_index );
				}
				else if( fMidDist < geom.GetAABB().vMin[iAxis] )
				{
					nodes[child_node_index[1]].veciGeometryIndex.push_back( geom_index );
				}
				else
					nodes[iNodeIndex].veciGeometryIndex.push_back( geom_index );
			}

//			RecordNode( m_vecNode[iNodeIndex], "split a node" );

		}
/*		else
		{
			const AABNode& current_node = m_vecNode[iNodeIndex];
//			PERIODICAL( 256, RecordLeafNode( current_node ) );
///			RecordNode( current_node, "created a leaf node" );

//			UpdateLeafNodeStatistics( current_node );
		}*/
	}
}


template<class TGeometry>
inline void CNonLeafyAABTree<TGeometry>::GetIntersectingAABBs( const AABB3& aabb, std::vector<int>& rvecDestIndex )
{
	size_t i = 0;
	std::vector<AABNode>& nodes = AABTree<TGeometry>::m_vecNode;

	if( nodes.empty() )
		return;

	std::vector<int>& nodes_to_check = AABTree<TGeometry>::m_vecNodeToCheck;

	nodes_to_check.resize(0);

	nodes_to_check.push_back(0);

	while( !nodes_to_check.empty() )
	{
		AABNode& rNode =  nodes[ nodes_to_check.back() ];
		nodes_to_check.pop_back();

		// check intersection with geometries in this cell
		for( i=0; i<rNode.veciGeometryIndex.size(); i++ )
		{
			const int geom_index = rNode.veciGeometryIndex[i];
			const TGeometry& geom = AABTree<TGeometry>::m_vecGeometry[ geom_index ];

			if( aabb.IsIntersectingWith( geom.GetAABB() ) )
				rvecDestIndex.push_back( geom_index );
		}

		if( !rNode.IsLeaf() )
		{
			if( 0 <= rNode.child[1] && rNode.child[1] < (int)nodes.size()
			 && aabb.IsIntersectingWith( nodes[ rNode.child[0] ].aabb ) )
			{
				// 'aabb' is intersecting with the negative half-space
				nodes_to_check.push_back( rNode.child[1] );
			}

			if( 0 <= rNode.child[0] && rNode.child[0] < (int)nodes.size()
			 && aabb.IsIntersectingWith( nodes[ rNode.child[0] ].aabb ) )
			{
				// 'aabb' is intersecting with the positive half-space
				nodes_to_check.push_back( rNode.child[0] );
			}
		}
	}
}


/// assums the geometry index has been already detached from tree node
template<class TGeometry>
inline void CNonLeafyAABTree<TGeometry>::LinkGeometry( int geom_index )
{
	const Vector3 vTreePos = AABTree<TGeometry>::m_vWorldPos;

	const AABB3& aabb = AABTree<TGeometry>::m_vecGeometry[geom_index].GetAABB();

	int index = 0;

	std::vector<AABNode>& nodes = AABTree<TGeometry>::m_vecNode;

	while(1)
	{
		if( nodes[index].IsLeaf() )
		{
			// reached the leaf - link to this node
			nodes[index].AddIndex( geom_index );
			return;
		}

		int& axis = nodes[index].iAxis;

		if( aabb.vMax[axis] < vTreePos[axis] + nodes[index].fDist )
		{
			// proceed to the front half-space
			index = nodes[index].child[0];
			continue;
		}
		else if( vTreePos[axis] + nodes[index].fDist < aabb.vMin[axis] )
		{
			// proceed to the rear half-space
			index = nodes[index].child[1];
			continue;
		}
		else
		{
			// aabb is crossing the plane of the current node - link to this node
			nodes[index].AddIndex( geom_index );
			return;
		}
	}
}

} // namespace amorphous
