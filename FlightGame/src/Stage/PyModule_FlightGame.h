#ifndef __PyModule_FlightGame_H__
#define __PyModule_FlightGame_H__


#include <string>
#include <Python.h>
#include "Stage/fwd.h"


extern PyMethodDef g_PyModuleFlightGameMethod[];


#endif  /*  __PyModule_FlightGame_H__  */





/*
class CStageScriptModuleHodler
{
	class ScriptEntry()
	{
	public:

		std::string m_ModuleName;
		PyMethodDef *m_pPyMethodDef;

	public:

		ScriptEntry( const std::string& module_name, PyMethodDef method[] )
			:
		m_ModuleName(module_name),
		m_pPyMethodDef(method)
		{}

	};
};
*/


/*
class CPyModule
{
	std::vector<PyMethodDef> m_vecPyModuleFlightGameMethod;

protected:

	void AddPyMethod(  );

	void SetPyModuleName(  );

public:

	void RegisterModule();

	CPyModule() {}
	virtual ~CPyModule() {}
};


class CPyModuleFlightGame : public CPyModule
{
	CStageWeakPtr m_pStage;

public:

	CPyModuleFlightGame() {}

	CPyModuleFlightGame( CStageWeakPtr pStage ) : m_pStage(pStage) {}

	virtual ~CPyModuleFlightGame() {}
}*/
