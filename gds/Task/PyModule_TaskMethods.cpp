#include "PyModule_TaskMethods.hpp"

#include "App/GameApplicationBase.hpp"
#include "Task/GameTask.hpp"
#include "Task/GameTaskManager.hpp"

#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"

#include "../base.hpp"

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


using namespace gsf::py::task;


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


PyMethodDef gsf::py::task::g_PyModuleTaskMethod[] =
{
	{ "RequestTaskTransition",		RequestTaskTransition,		METH_VARARGS, "" },
//	{ "RequestTransitionToNextTask",RequestTransitionToNextTask,METH_VARARGS, "" },
	{NULL, NULL}
};
