
#ifndef  __IK_CONTROLLER_H__
#define  __IK_CONTROLLER_H__

#include "Math64/LinearR3.h"

#include "IK_Jacobian.h"

#include <vector>
using namespace std;

//class CIK_Jacobian;
class CIK_Tree;


class CIK_Controller
{
	CIK_Tree *m_pTree;

	CIK_Jacobian *m_pJacobian;

	vector<VectorR3> m_vecvTargetPosition;

	bool m_bUseJacobianTargets;


public:
	CIK_Controller();
	~CIK_Controller();

	bool Init();
	void Release();

	void SetTargetPosition( int i, VectorR3 v );
	VectorR3 GetTargetPosition( int i );

	void SetJacobianUpdateMethod( JacobUpdateMode mode );
	JacobUpdateMode GetJacobianUpdateMethod();

	void DoUpdateStep();

};


#endif		/*  __IK_CONTROLLER_H__  */