#include "ScriptManager.hpp"
#include "ScriptArchive.hpp"
#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"
#include <boost/python.hpp>


namespace amorphous
{

using namespace std;
using namespace boost;


bool ScriptManager::ms_UseBoostPythonModules = true;


//=========================================================================
// CPythonUserCount
//=========================================================================

singleton<CPythonUserCount> CPythonUserCount::m_obj;



string ScriptManager::GetExtraErrorInfo()
{
	// failed attempt - exception returned by PyErr_Occurred() cannot be retrieved as a string
/*
	PyObject *pException = PyErr_Occurred();
	char *exception_str = NULL;
	if( pException )
	{
		exception_str = PyString_AsString( pException );
	}
*/

	PyObject *pType = NULL, *pValue = NULL, *pTraceback = NULL;
	PyErr_Fetch( &pType, &pValue, &pTraceback );

	char *err_type = NULL, *err_value = NULL, *err_traceback = NULL;

	if( pType )
		err_type = PyString_AsString( pType );

	if( pValue )
		err_value = PyString_AsString( pValue );

	if( pTraceback )
		err_traceback = PyString_AsString( pTraceback );

	string out
		= string("type: ")         + ( err_type      ? err_type      : "" )
		+ string(", value: ")     + ( err_value     ? err_value     : "" )
		+ string(", traceback: ") + ( err_traceback ? err_traceback : "" );

	return out;
}



//===========================================================================
// ScriptManager
//===========================================================================

ScriptManager::ScriptManager()
:
m_pTargetScript(NULL),
m_bLoadFromNonArchivedFiles(false)
{
	PythonUserCount().IncRefCount();
}

ScriptManager::~ScriptManager()
{
	PythonUserCount().DecRefCount();
}


void ScriptManager::AddEventCallback( PyObject* pEventCallback )
{
	if( m_pTargetScript )
		m_pTargetScript->m_pEventCallback = pEventCallback;
}


void ScriptManager::AddInitCallback( PyObject* pEventCallback )
{
	if( m_pTargetScript )
		m_pTargetScript->m_pInitCallback = pEventCallback;
}


bool ScriptManager::LoadScriptArchiveFile( const string& filename )
{
	ScriptArchive script_archive;
	bool res = script_archive.LoadFromFile( filename );
	if( !res )
		return false;

	// script archive contains one or more script files
	// load one by one 
	size_t i, num_scripts = script_archive.m_vecBuffer.size();
	for( i=0; i<num_scripts; i++ )
	{
		string& script_filename = script_archive.m_vecSourceFilename[i];

		m_vecEventScript.push_back( PythonEventScript() );

		if( !LoadScript( script_archive.m_vecBuffer[i].m_Buffer, m_vecEventScript.back() ) )
		{
			// ReleaseScripts();
			LOG_PRINT_ERROR( "invalid script file: [" + to_string((int)i) + "] " + script_filename );
		}
		else
		{
			m_vecEventScript.back().filename = script_filename;
			LOG_PRINT( "loaded a script file: [" + to_string((int)i) + "] " + script_filename );

			// check for the non-archived file
			struct stat script_file_stat;
			if( stat(script_filename.c_str(), &script_file_stat) != 0 )
				continue;

			// non-archived script file was found
			// - record the last modified time of the file
			m_vecEventScript.back().m_LastModifiedTime = script_file_stat.st_mtime;

			// check for file updates later at runtime
			m_bLoadFromNonArchivedFiles = true;
		}
	}

	return true;
}


bool ScriptManager::ExecuteScript( const stream_buffer& buffer )
{
	PythonEventScript es;
	return LoadScript( buffer, es );
}


bool ScriptManager::LoadScriptFromFile( const std::string& filename )
{
/*	stream_buffer script_buffer;
	bool res = script_buffer.LoadTextFile( filename );
	if( !res )
		return false;
	
	LoadScript( script_buffer );

	return true;
*/

	return false;
}


void ScriptManager::InitScripts()
{
	size_t i, num_eventscripts = m_vecEventScript.size();
	for( i=0; i<num_eventscripts; i++ )
	{
		if( !m_vecEventScript[i].m_pInitCallback )
			continue;

		PyObject *pArgList;
		pArgList = Py_BuildValue( "()" );

		PyObject *pResult  = PyEval_CallObject( m_vecEventScript[i].m_pInitCallback, pArgList );

		Py_DECREF( pArgList );
/*
		if( pResult == NULL )
		{
			// an invalid script - discard the object
			PyErr_Clear();	// omitting this causes an error in Py_Finalize()
			m_vecEventScript[i].m_bIsDone = true;
			PrintLog( "invalid return value from init script" );
		}
		else
		{
			int result;
//			PyArg_ParseTuple( pResult, "i", &result );
			PyArg_Parse( pResult, "i", &result );

			if( result == PythonEventScript::EVENT_DONE )
			{
				PythonEventScript& script = m_vecEventScript[i];

//	necessary?	Py_DECREF( m_vecEventScript[i].m_pInitCallback );
				PrintLog( "script[" + to_string(int(i)) + "] '"+ script.name + "' has been initialized" );
			}
			Py_DECREF( pResult );
		}*/
	}
}


void ScriptManager::ReloadUpdatedScriptFiles()
{
	size_t i, num_eventscripts = m_vecEventScript.size();
	struct stat script_file_stat;
	stream_buffer script_buffer;

	for( i=0; i<num_eventscripts; i++ )
	{
		PythonEventScript& script = m_vecEventScript[i];

		if( script.m_LastModifiedTime == 0 )
			continue;	// last modified time is not recorded

		if( stat(script.filename.c_str(), &script_file_stat) != 0 )
			continue;

		if( script.m_LastModifiedTime < script_file_stat.st_mtime )
		{
			// script has been updated - try to reload it

			LOG_PRINT( "Reloading a script: " + script.filename );

			script.m_LastModifiedTime = script_file_stat.st_mtime;

			//ReleaseScript()?
			bool res = script_buffer.LoadTextFile( script.filename );
			if( !res )
				continue;

			if( script.m_pEventCallback )
			{
				Py_DECREF( script.m_pEventCallback ); 
				script.m_pEventCallback = NULL;
			}

			if( script.m_pInitCallback )
			{
				Py_DECREF( script.m_pInitCallback ); 
				script.m_pInitCallback = NULL;
			}

			if( !LoadScript( script_buffer, script ) )
			{
				LOG_PRINT_ERROR( string("invalid script: ") + script.filename );
			}
		}
	}
}


void ScriptManager::Update()
{
	PROFILE_FUNCTION();

	if( m_bLoadFromNonArchivedFiles )
		ReloadUpdatedScriptFiles();

	size_t i, num_eventscripts = m_vecEventScript.size();

	for( i=0; i<num_eventscripts; i++ )
	{
		if( m_vecEventScript[i].m_bIsDone )
			continue;

		PyObject *pArgList;
		pArgList = Py_BuildValue( "()" );

		PyObject *pResult  = PyEval_CallObject( m_vecEventScript[i].m_pEventCallback, pArgList );

		Py_DECREF( pArgList );

		if( pResult == NULL )
		{
			// an invalid script - discard the object

			// save the error info first
			string err_string_ex = GetExtraErrorInfo();

			// omitting this causes an error in Py_Finalize()
			PyErr_Clear();

			// mark the script as 'finished'
			m_vecEventScript[i].m_bIsDone = true;

			// log the error
			string err_string = "invalid return value from event script: " + m_vecEventScript[i].filename;
			if( 0 < err_string_ex.length() )
				err_string += " - " + err_string_ex;

			LOG_PRINT_ERROR( err_string );
		}
		else
		{
			int result;
//			PyArg_ParseTuple( pResult, "i", &result );
			PyArg_Parse( pResult, "i", &result );

			if( result == PythonEventScript::EVENT_DONE )
			{
				PythonEventScript& script = m_vecEventScript[i];

				script.m_bIsDone = true;
//	necessary?	Py_DECREF( m_vecEventScript[i].m_pEventCallback );
				LOG_PRINT( "script[" + to_string(int(i)) + "] '"+ script.filename + "' has been finished." );
			}
			Py_DECREF( pResult );
		}
	}
}


} // namespace amorphous
