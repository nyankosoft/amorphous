#ifndef __OBBTree_HPP__
#define __OBBTree_HPP__

#include <d3dx9.h>
#pragma warning(disable:4786)
#include <vector>
#include "3DMath/Vector3.hpp"

#define for if( 0 );else for

struct FACE{
	Vector3 Vertex[3];
	Vector3 Center;
};

typedef std::vector < FACE > FACES;

struct OBBDATA{
	D3DXMATRIX Matrix;
	Vector3 Axis[3];
	float Length[3];
	Vector3 Center;
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
	bool Create( LPD3DXMESH pMesh, int Level );
	bool Create( const std::vector<Vector3>& vertices, const std::vector<unsigned int>& triangle_indices, int Level );
	void Create( FACES &Face, int Level, OBBNODE *pOBBNode );
	bool CheckCollision( COBBTree &OBBTreeA, D3DXMATRIX &TransMatA,
						 COBBTree &OBBTreeB, D3DXMATRIX &TransMatB );
	void GetLeafOBBs( std::vector<OBBDATA>& obbs );
	void DumpToTextFile( const std::string& filepath );
	const OBBNODE *GetTopNode() const { return pOBBTopNode; }
private:
	void CreateOBB( std::vector < FACE > &Face, OBBDATA &OBBData );
	void Rotate( float a[3][3], float s, float tau, int i, int j, int k, int l );
	bool Jacobi( float a[3][3], float v[3][3], float d[3] );
	void ReleaseNodes( OBBNODE *pOBBNode );
	bool IsCollision( OBBDATA &BoxA, OBBDATA &BoxB );
	bool CheckCollision( OBBNODE *pNodeA, OBBNODE *pNodeB, D3DXMATRIX &TransMat, D3DXMATRIX &RotMat );
	void GetLeafOBBs( const OBBNODE *pNode,std ::vector<OBBDATA>& obbs );
	void DumpToTextFile( FILE *fp, OBBNODE *pNode, int depth );

public:
	OBBNODE *pOBBTopNode;
};

#endif // __OBBTree_HPP__
