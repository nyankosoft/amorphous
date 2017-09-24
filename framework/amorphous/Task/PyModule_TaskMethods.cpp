#include "PyModule_TaskMethods.hpp"

#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Task/GameTask.hpp"
#include "amorphous/Task/GameTaskManager.hpp"
#include "amorphous/Script/PythonScriptManager.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


inline static bool IsTaskAvailable()
{
	if( g_pGameAppBase
	 && g_pGameAppBase->GetTaskManager()
	 && g_pGameAppBase->GetTaskManager()->GetCurrentTask() )
		return true;
	else
		return false;
}

#define RETURN_PYNONE_IF_NO_TASK()	if( !IsTaskAvailable() )	{	Py_INCREF( Py_None );	return Py_None;	}


PyObject* RequestTaskTransition( PyObject* self, PyObject* args )
{
	char *task_name;
	float fade_out_time_in_sec = -1.0f;	// time spent on fade out effect for the current task (default value is used when a negative value is specified)
	float fade_in_time_in_sec = -1.0f;	// time spent on fade in effect for the next task (default value is used when a negative value is specified)
	float delay_in_sec = 0;

	int priority = 0;

	int result = PyArg_ParseTuple( args, "s|fff", &task_name, &delay_in_sec, &fade_out_time_in_sec, &fade_in_time_in_sec );

	RETURN_PYNONE_IF_NO_TASK()

	g_pGameAppBase->GetTaskManager()->GetCurrentTask()->RequestTaskTransition( task_name, priority, delay_in_sec, fade_out_time_in_sec, fade_in_time_in_sec );

    Py_INCREF( Py_None );
	return Py_None;
}


static PyMethodDef sg_PyModuleTaskMethod[] =
{
	{ "RequestTaskTransition",		RequestTaskTransition,		METH_VARARGS, "" },
//	{ "RequestTransitionToNextTask",RequestTransitionToNextTask,METH_VARARGS, "" },
	{NULL, NULL}
};


void RegisterPythonModule_Task( PythonScriptManager& mgr )
{
	mgr.AddModule( "Task", sg_PyModuleTaskMethod );
}


} // namespace amorphous
