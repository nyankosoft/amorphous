#ifndef __CLASS_IK_NODE_H__
#define __CLASS_IK_NODE_H__

#include "Math64/LinearR3.h"


class VectorR3;

class CIK_Node
{
	friend class CIK_Tree;

public:

	enum Purpose {JOINT, EFFECTOR};

	CIK_Node(const VectorR3&, const VectorR3&, double, Purpose, double minTheta = -_PI, double maxTheta = _PI, double restAngle = 0.);

	void DrawNode(bool);
	void PrintNode();
	void InitNode();

	const VectorR3& GetAttach() const { return attach; }

	double GetTheta() const { return theta; }
	double AddToTheta( double delta ) { theta += delta; return theta; }

	const VectorR3& GetS() const { return s; }
	const VectorR3& GetW() const { return w; }

	double GetMinTheta() const { return minTheta; }
	double GetMaxTheta() const { return maxTheta; } 
	double GetRestAngle() const { return restAngle; } ;
	void SetTheta(double newTheta) { theta = newTheta; }
	void ComputeS(void);
	void ComputeW(void);

	bool IsEffector() const { return purpose == EFFECTOR; } 
	bool IsJoint() const { return purpose == JOINT; }
	int GetEffectorNum() const { return seqNumEffector; }
	int GetJointNum() const { return seqNumJoint; }

	bool IsFrozen() const { return freezed; }
	void Freeze() { freezed = true; }
	void UnFreeze() { freezed = false; }

private:
	bool freezed;			// Is this node frozen?
	int seqNumJoint;		// sequence number if this node is a joint
	int seqNumEffector;		// sequence number if this node is an effector
	double size;			// size
	Purpose purpose;		// joint / effector / both
	VectorR3 attach;		// attachment point
	VectorR3 r;				// relative position vector
	VectorR3 v;				// rotation axis
	double theta;			// joint angle (radian)
	double minTheta;		// lower limit of joint angle
	double maxTheta;		// upper limit of joint angle
	double restAngle;		// rest position angle
	VectorR3 s;				// GLobal Position
	VectorR3 w;				// Global rotation axis
	CIK_Node* left;				// left child
	CIK_Node* right;			// right sibling
	CIK_Node* realparent;		// pointer to real parent

	void DrawBox() const;
};


#endif  /*  __CLASS_IK_NODE_H__  */
