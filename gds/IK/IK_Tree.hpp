#ifndef __CLASS_IK_TREE__
#define __CLASS_IK_TREE__


#include "Math64/LinearR3.h"
#include "IK_Node.hpp"


class CIK_Tree {

public:
	CIK_Tree();

	int GetNumNode() const { return nNode; }
	int GetNumEffector() const { return nEffector; }
	int GetNumJoint() const { return nJoint; }
	void InsertRoot(CIK_Node*);
	void InsertLeftChild(CIK_Node* parent, CIK_Node* child);
	void InsertRightSibling(CIK_Node* parent, CIK_Node* child);

	// Accessors based on node numbers
	CIK_Node* GetJoint(int);
	CIK_Node* GetEffector(int);
	const VectorR3& GetEffectorPosition(int);

	// Accessors for tree traversal
	CIK_Node* GetRoot() const { return root; }
	CIK_Node* GetSuccessor ( const CIK_Node* ) const;
	CIK_Node* GetParent( const CIK_Node* node ) const { return node->realparent; }

	void Compute();
	void Draw();
	void Print();
	void Init();
	void UnFreeze();

private:
	CIK_Node* root;
	int nNode;			// nNode = nEffector + nJoint
	int nEffector;
	int nJoint;
	void SetSeqNum(CIK_Node*);
	CIK_Node* SearchJoint(CIK_Node*, int);
	CIK_Node* SearchEffector(CIK_Node*, int);
	void ComputeTree(CIK_Node*);
	void DrawTree(CIK_Node*);
	void PrintTree(CIK_Node*);
	void InitTree(CIK_Node*);
	void UnFreezeTree(CIK_Node*);
};

inline CIK_Node* CIK_Tree::GetSuccessor ( const CIK_Node* node ) const
{
	if ( node->left ) {
		return node->left;
	}
	while ( true ) {
		if ( node->right ) {
			return ( node->right );
		}
		node = node->realparent;
		if ( !node ) {
			return 0;		// Back to root, finished traversal
		} 
	}
}

#endif  /*  __CLASS_IK_TREE__   */
