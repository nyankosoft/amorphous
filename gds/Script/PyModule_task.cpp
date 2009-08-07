#include "PyModule_task.hpp"
#include "App/ApplicationBase.hpp"
#include "Task/GameTask.hpp"
#include "Task/GameTaskManager.hpp"
#include "Support/Macro.h"
#include "Support/Log/DefaultLog.hpp"
#include <boost/python.hpp>

#include "../base.hpp"

using namespace std;
using namespace boost;


inline static bool IsTaskAvailable()
{
	if( g_pAppBase
	 && g_pAppBase->GetTaskManager()
	 && g_pAppBase->GetTaskManager()->GetCurrentTask() )
		return true;
	else
		return false;
}


//using namespace gsf::py::task;


/// time spent on fade out effect for the current task (default value is used when a negative value is specified)
/// time spent on fade in effect for the next task (default value is used when a negative value is specified)
bool RequestTaskTransition( const std::string &task_name, int priority = 0, float delay_in_sec = 0.0f, float fadeout_time_in_sec = -1.0f, float fadein_time_in_sec = -1.0f )
{
	if( !IsTaskAvailable )
		return false;

	g_pAppBase->GetTaskManager()->GetCurrentTask()->RequestTaskTransition(
		task_name,
		priority,
		delay_in_sec,
		fadeout_time_in_sec,
		fadein_time_in_sec );

	return true;
}



BOOST_PYTHON_MODULE(task)
{
	using namespace boost::python;
	namespace py = boost::python;

	def( "RequestTaskTransition", RequestTaskTransition, ( py::arg("task_name"), py::arg("priority")=0, py::arg("delay")=0.0f, py::arg("fadein_time")=-1.0f, py::arg("fadeout_time")=-1.0f ) );
}


void RegisterPythonModule_task()
{
	// Register the module with the interpreter
	if (PyImport_AppendInittab("task", inittask) == -1)
	{
		const char *msg = "Failed to add 'task' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}
