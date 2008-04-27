#ifndef __PERFORMANCECHECK_H__
#define __PERFORMANCECHECK_H__

struct SPerformanceCheck
{
	int iNumRenderedCopyEntites;
	int iNumDrawPrimCalls;

	float fMouseNewMoveX;
	float fMouseNewMoveY;


	SPerformanceCheck()
	{
		iNumRenderedCopyEntites = 0;
		iNumDrawPrimCalls       = 0;
	}
};

extern SPerformanceCheck g_PerformanceCheck;

#endif   /*  __PERFORMANCECHECK_H__  */