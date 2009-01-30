
#include "IK_Controller.hpp"

#include "IK_Tree.hpp"
#include "IK_Jacobian.hpp"



CIK_Controller::CIK_Controller()
{
	m_pTree = NULL;
	m_pJacobian = NULL;
}


CIK_Controller::~CIK_Controller()
{
	Release();
}


bool CIK_Controller::Init( tree )
{
	Release();

	m_pJacobian = new CIK_Jacobian( tree );

	return true;
}


void CIK_Controller::Release()
{
	SafeDelete( m_pJacobian );
	SafeDelete( m_pTree );
}



// Make slowdown factor larger to make the simulation take larger, less frequent steps
// Make the constant factor in Tstep larger to make time pass more quickly
//const int SlowdownFactor = 40;
const int SlowdownFactor = 10;		// Make higher to take larger steps less frequently
const int SleepsPerStep=SlowdownFactor;
int SleepCounter=0;
const double Tstep = 0.0005*(double)SlowdownFactor;		// Time step
double T = -Tstep;				// Current time



void CIK_Controller::DoUpdateStep()
{
/*	if ( SleepCounter==0 )
	{
		T += Tstep;
//		UpdateTargets( T );
	} 
*/
	Jacobian *jacob = m_pJacobian;

	if ( UseJacobianTargets )
	{
		jacob->SetJtargetActive();
	}
	else
	{
		jacob->SetJendActive();
	}
	
	jacob->ComputeJacobian();						// Set up Jacobian and deltaS vectors

	// Calculate the change in theta values 
	jacob->CalcDeltaThetas();

/*	switch (WhichMethod)
	{
		case JACOB_TRANS:
			jacob->CalcDeltaThetasTranspose();		// Jacobian transpose method
			break;
		case DLS:
			jacob->CalcDeltaThetasDLS();			// Damped least squares method
			break;
		case PURE_PSEUDO:
			jacob->CalcDeltaThetasPseudoinverse();	// Pure pseudoinverse method
			break;
		case SDLS:
			jacob->CalcDeltaThetasSDLS();			// Selectively damped least squares method
			break;
		default:
			jacob->ZeroDeltaThetas();
			break;
	}*/

	if ( SleepCounter==0 )
	{
		jacob->UpdateThetas();							// Apply the change in the theta values
		jacob->UpdatedSClampValue();
		SleepCounter = SleepsPerStep;
	}
	else
	{ 
		SleepCounter--;
	}

}