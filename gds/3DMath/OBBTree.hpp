#ifndef __OBBTree_HPP__
#define __OBBTree_HPP__

#pragma warning(disable:4786)
#include <vector>
#include "Matrix34.hpp"

#define for if( 0 );else for

struct FACE{
	Vector3 Vertex[3];
	Vector3 Center;
};

typedef std::vector < FACE > FACES;

struct OBBDATA{
	Matrix34 Matrix;
	Vector3 Axis[3];
	float Length[3];
	Vector3 Center;

	OBBDATA()
		:
	Matrix( Matrix34Identity() ),
	Center( Vector3(0,0,0) )
	{
		for( int i=0; i<3; i++ )
		{
			Axis[i] = Vector3(0,0,0);
			Length[i] = 0;
		}
	}
};

struct OBBNODE{
	OBBNODE *pOBBNodeL, *pOBBNodeR;
	OBBDATA OBBData;
	OBBNODE(){ pOBBNodeL = NULL; pOBBNodeR = NULL; }
};

class COBBTree
{
public:
	COBBTree();
	~COBBTree();
//	bool Create( LPD3DXMESH pMesh, int Level );
	bool Create( const std::vector<Vector3>& vertices, const std::vector<unsigned int>& triangle_indices, int Level );
	void Create( FACES &Face, int Level, OBBNODE *pOBBNode );
	bool CheckCollision( COBBTree &OBBTreeA, Matrix34 &TransMatA,
						 COBBTree &OBBTreeB, Matrix34 &TransMatB );
	void GetLeafOBBs( std::vector<OBBDATA>& obbs );
	void DumpToTextFile( const std::string& filepath );
	const OBBNODE *GetTopNode() const { return pOBBTopNode; }
private:
	void CreateOBB( std::vector < FACE > &Face, OBBDATA &OBBData );
	void Rotate( float a[3][3], float s, float tau, int i, int j, int k, int l );
	bool Jacobi( float a[3][3], float v[3][3], float d[3] );
	void ReleaseNodes( OBBNODE *pOBBNode );
	bool IsCollision( OBBDATA &BoxA, OBBDATA &BoxB );
	bool CheckCollision( OBBNODE *pNodeA, OBBNODE *pNodeB, Matrix34 &TransMat, Matrix33 &RotMat );
	void GetLeafOBBs( const OBBNODE *pNode,std ::vector<OBBDATA>& obbs );
	void DumpToTextFile( FILE *fp, OBBNODE *pNode, int depth );

public:
	OBBNODE *pOBBTopNode;
};

#endif // __OBBTree_HPP__
