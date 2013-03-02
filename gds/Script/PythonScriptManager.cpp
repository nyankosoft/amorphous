#include "PythonScriptManager.hpp"
#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"

namespace amorphous
{
	
using namespace std;
using namespace boost;


//=========================================================================
// Global Functions
//=========================================================================


static ScriptManager *g_pScriptManager = NULL;

//static ScriptManager::EventScript *gs_pTargetScript = NULL;


static PyObject *SetScriptUpdateCallback( PyObject *dummy, PyObject *args )
{
    PyObject *result = NULL;
    PyObject *temp;

	if( PyArg_ParseTuple(args, "O:set_callback", &temp) )
	{
		if (!PyCallable_Check(temp))
		{
			// PyCallable_Check does not necessarily return 0 for an invalid script
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			PrintLog( "an invalid event callback" );
//			return NULL;
			Py_INCREF(Py_None);
			result = Py_None;
			return result;
		}

		// Add a reference to new callback
		Py_XINCREF(temp);

//		Py_XDECREF(g_PythonCallback);  // Dispose of previous callback

		// Remember new callback
		g_pScriptManager->AddEventCallback( temp );

		// Boilerplate to return "None"
		Py_INCREF(Py_None);
		result = Py_None;
	}
	return result;
}


static PyObject *SetScriptInitCallback( PyObject *dummy, PyObject *args )
{
    PyObject *result = NULL;
    PyObject *temp;
	try
	{
		if( PyArg_ParseTuple(args, "0:set_callback", &temp) )
		{
			if (!PyCallable_Check(temp))
			{
				// PyCallable_Check does not necessarily return 0 for an invalid script
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				PrintLog( "an invalid init callback" );
//				return NULL;
				Py_INCREF(Py_None);
				result = Py_None;
				return result;
			}

			// Add a reference to new callback
			Py_XINCREF(temp);

//			Py_XDECREF(g_PythonCallback);  // Dispose of previous callback

			// Remember new callback
			g_pScriptManager->AddInitCallback( temp );

			// Boilerplate to return "None"
			Py_INCREF(Py_None);
			result = Py_None;
		}
	}
	catch( std::exception& e )
	{
		g_Log.Print( WL_ERROR, "SetInitCallback() failed - exception: %s", e.what() );
	}

	return result;
}


PyMethodDef g_ScriptBaseMethod[] =
{
    { "SetCallback",		SetScriptUpdateCallback,	METH_VARARGS, "sets an event callback" },
    { "SetInitCallback",	SetScriptInitCallback,		METH_VARARGS, "sets an init callback" },
//	{ "SetInitCallback",	SetInitCallback,		METH_VARARGS, "sets an init callback" },
    {NULL, NULL}
};


PythonScriptManager::PythonScriptManager()
{
	g_pScriptManager = this;

	// make a module for setting callbacks
	AddModule( "ScriptBase", g_ScriptBaseMethod );
//	PyObject *pBaseModule0 = PyImport_AddModule( "ScriptBase" );
//	PyObject *pBaseModule = Py_InitModule( "ScriptBase", g_ScriptBaseMethod );
}


PythonScriptManager::~PythonScriptManager()
{
	g_pScriptManager = NULL;
}


// Called when boost::python is not used. See CStage::InitEventScriptManager()
void PythonScriptManager::AddModule( const string& module_name, PyMethodDef method[] )
{
	PyObject *pModule0 = PyImport_AddModule( module_name.c_str() );
	if( !pModule0 )
	{
		LOG_PRINT_ERROR( " - PyImport_AddModule() failed for module '" + module_name + "'. Error: " + GetExtraErrorInfo() );
	}

	PyObject *pModule  = Py_InitModule( module_name.c_str(), method );
	if( !pModule )
	{
		LOG_PRINT_ERROR( " - Py_InitModule() failed for module '" + module_name + "'. Error: " + GetExtraErrorInfo() );
	}
}


void PythonScriptManager::Update()
{
}


bool PythonScriptManager::LoadScript( const stream_buffer& buffer, PythonEventScript& dest_script )
{
	boost::thread::id thread_id = boost::this_thread::get_id();

	bool res = false;

	PyObject *pMainModule     = PyImport_AddModule( "__main__" );
	PyObject *pMainDictionary = PyModule_GetDict( pMainModule );

	// set script holder object that will be the target for registering callback functions
	m_pTargetScript = &dest_script;

	PyErr_Clear();

	// run  the script and register the callback function(s)
	PyObject* pRunResult = PyRun_String( (const char *)(&buffer.get_buffer()[0]),
											Py_file_input, pMainDictionary, pMainDictionary );

	res = pRunResult ? true : false;

	m_pTargetScript = NULL;

	if( !dest_script.m_pEventCallback )
	{
		dest_script.m_bIsDone = true;
		LOG_PRINT_ERROR( " No callback function has been found in the script or PyRun_String() returned NULL. Error: " + GetExtraErrorInfo() );
//		g_Log.Print( "no callback has been found set for script[%02d]", m_vecEventScript.size() - 1 );
		return false;
	}

//	if( pRunResult == NULL )
	if( res == false )
	{
		return false;
//		PrintLog( "an exception raised during the execution of the script, '" + filename + "'");
		LOG_PRINT_ERROR( " PyRun_String() returned NULL. Error: " + GetExtraErrorInfo() );
	}
	else
	{
		dest_script.m_bIsDone = false;
		return true;
//		PrintLog( "script file, '" + filename + "' loaded");
	}
	return res;
}


} // namespace amorphous
