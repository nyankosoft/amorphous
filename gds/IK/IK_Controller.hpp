#ifndef  __IK_CONTROLLER_H__
#define  __IK_CONTROLLER_H__


#include "3DMath/Vector3.hpp"
#include "IK_Jacobian.hpp"

#include <vector>

//class CIK_Jacobian;
class CIK_Tree;


class CIK_Controller
{
	CIK_Tree *m_pTree;

	CIK_Jacobian *m_pJacobian;

	std::vector<dVector3> m_vecvTargetPosition;

	bool m_bUseJacobianTargets;


public:
	CIK_Controller();
	~CIK_Controller();

	bool Init();
	void Release();

	void SetTargetPosition( int i, dVector3 v );
	dVector3 GetTargetPosition( int i );

	void SetJacobianUpdateMethod( JacobUpdateMode mode );
	JacobUpdateMode GetJacobianUpdateMethod();

	void DoUpdateStep();

};


#endif		/*  __IK_CONTROLLER_H__  */