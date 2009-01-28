
#include "PyModule_Task.h"

#include "App/ApplicationBase.h"
#include "Task/GameTask.h"
#include "Task/GameTaskManager.h"

#include "Support/Macro.h"
#include "Support/Log/DefaultLog.h"

#include "../base.h"

using namespace std;


inline static bool IsTaskAvailable()
{
	if( g_pAppBase
	 && g_pAppBase->GetTaskManager()
	 && g_pAppBase->GetTaskManager()->GetCurrentTask() )
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

	int result = PyArg_ParseTuple( args, "s|fff", &task_name, &delay_in_sec, &fade_out_time_in_sec, &fade_in_time_in_sec );

	RETURN_PYNONE_IF_NO_TASK()

	g_pAppBase->GetTaskManager()->GetCurrentTask()->RequestTaskTransition( task_name, delay_in_sec );

    Py_INCREF( Py_None );
	return Py_None;
}


PyMethodDef gsf::py::task::g_PyModuleTaskMethod[] =
{
	{ "RequestTaskTransition",		RequestTaskTransition,		METH_VARARGS, "" },
//	{ "RequestTransitionToNextTask",RequestTransitionToNextTask,METH_VARARGS, "" },
	{NULL, NULL}
};
