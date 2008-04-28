
#ifndef  __APPLICATIONBASEFG_H__
#define  __APPLICATIONBASEFG_H__


#include "App/ApplicationBase.h"


class CApplicationBaseFG : public CApplicationBase
{
private:

	void Execute();

public:

	CApplicationBaseFG();
	virtual ~CApplicationBaseFG();

	virtual bool Init();

	/// returns true if succeeds
	virtual bool InitTaskManager();

	void Release();
};


#endif		/*  __APPLICATIONBASEFG_H__  */
