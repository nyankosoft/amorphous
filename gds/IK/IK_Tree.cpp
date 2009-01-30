
#include <iostream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/

#include "Math64/LinearR3.h"
#include "IK_Tree.hpp"
#include "IK_Node.hpp"


CIK_Tree::CIK_Tree()
{
	root = 0;
	nNode = nEffector = nJoint = 0;
}


void CIK_Tree::SetSeqNum( CIK_Node* node )
{
	switch (node->purpose)
	{
	case CIK_Node::JOINT:
		node->seqNumJoint = nJoint++;
		node->seqNumEffector = -1;
		break;
	case CIK_Node::EFFECTOR:
		node->seqNumJoint = -1;
		node->seqNumEffector = nEffector++;
		break;
	}
}


void CIK_Tree::InsertRoot(CIK_Node* root)
{
	assert( nNode==0 );
	nNode++;
	CIK_Tree::root = root;
	root->r = root->attach;
	assert( !(root->left || root->right) );
	SetSeqNum(root);
}


void CIK_Tree::InsertLeftChild(CIK_Node* parent, CIK_Node* child)
{
	assert(parent);
	nNode++;
	parent->left = child;
	child->realparent = parent;
	child->r = child->attach - child->realparent->attach;
	assert( !(child->left || child->right) );
	SetSeqNum(child);
}


void CIK_Tree::InsertRightSibling(CIK_Node* parent, CIK_Node* child)
{
	assert(parent);
	nNode++;
	parent->right = child;
	child->realparent = parent->realparent;
	child->r = child->attach - child->realparent->attach;
	assert( !(child->left || child->right) );
	SetSeqNum(child);
}


// Search recursively below "node" for the node with index value.
CIK_Node* CIK_Tree::SearchJoint(CIK_Node* node, int index)
{
	CIK_Node* ret;
	if (node != 0) {
		if (node->seqNumJoint == index) {
			return node;
		} else {
			if (ret = SearchJoint(node->left, index)) {
				return ret;
			}
			if (ret = SearchJoint(node->right, index)) {
				return ret;
			}
			return NULL;
		}
	} 
	else {
		return NULL;
	}
}


// Get the joint with the index value
CIK_Node* CIK_Tree::GetJoint(int index)
{
	return SearchJoint(root, index);
}


// Search recursively below node for the end effector with the index value
CIK_Node* CIK_Tree::SearchEffector(CIK_Node* node, int index)
{
	CIK_Node* ret;
	if (node != 0) {
		if (node->seqNumEffector == index) {
			return node;
		} else {
			if (ret = SearchEffector(node->left, index)) {
				return ret;
			}
			if (ret = SearchEffector(node->right, index)) {
				return ret;
			}
			return NULL;
		}
	} else {
		return NULL;
	}
}


// Get the end effector for the index value
CIK_Node* CIK_Tree::GetEffector(int index)
{
	return SearchEffector(root, index);
}


// Returns the global position of the effector.
const VectorR3& CIK_Tree::GetEffectorPosition(int index)
{
	CIK_Node* effector = GetEffector(index);
	assert(effector);
	return (effector->s);  
}


void CIK_Tree::ComputeTree(CIK_Node* node)
{
	if (node != 0) {
		node->ComputeS();
		node->ComputeW();
		ComputeTree(node->left);
		ComputeTree(node->right);
	}
}


void CIK_Tree::Compute(void)
{ 
	ComputeTree(root); 
}


void CIK_Tree::DrawTree(CIK_Node* node)
{
/**	if (node != 0) {
		glPushMatrix();
		node->DrawNode( node==root );	// Recursively draw node and update ModelView matrix
		if (node->left) {
			DrawTree(node->left);		// Draw tree of children recursively
		}
		glPopMatrix();
		if (node->right) {
			DrawTree(node->right);		// Draw right siblings recursively
		}
	}**/
}


void CIK_Tree::Draw(void) 
{
	DrawTree(root);
}


void CIK_Tree::PrintTree(CIK_Node* node)
{
	if (node != 0) {
		node->PrintNode();
		PrintTree(node->left);
		PrintTree(node->right);
	}
}


void CIK_Tree::Print(void) 
{ 
///	PrintTree(root);  
///	cout << "\n";
}


// Recursively initialize tree below the node
void CIK_Tree::InitTree(CIK_Node* node)
{
	if (node != 0) {
		node->InitNode();
		InitTree(node->left);
		InitTree(node->right);
	}
}

// Initialize all nodes in the tree
void CIK_Tree::Init(void)
{
	InitTree(root);
}

void CIK_Tree::UnFreezeTree(CIK_Node* node)
{
	if (node != 0) {
		node->UnFreeze();
		UnFreezeTree(node->left);
		UnFreezeTree(node->right);
	}
}

void CIK_Tree::UnFreeze(void)
{
	UnFreezeTree(root);
}