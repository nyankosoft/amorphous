#include "BoostPythonScriptManager.hpp"
#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"
#include <boost/python.hpp>


namespace amorphous
{
	
using namespace std;
using namespace boost;


BoostPythonScriptManager::BoostPythonScriptManager()
{
	// add script_base module
}


BoostPythonScriptManager::~BoostPythonScriptManager()
{
}


bool BoostPythonScriptManager::LoadScript( const stream_buffer& buffer, PythonEventScript& dest_script )
{
	boost::thread::id thread_id = boost::this_thread::get_id();

	bool res = false;

	// Retrieve the main module
	python::object main = python::import("__main__");

	// Retrieve the main module's namespace
	python::object global(main.attr("__dict__"));

	// set script holder object that will be the target for registering callback functions
	m_pTargetScript = &dest_script;

	PyErr_Clear();

	// run  the script and register the callback function(s)
//		python::object result = python::exec( &(buffer.get_buffer()[0]), global, global );

	try
	{
		python::object result = python::exec( &(buffer.get_buffer()[0]), global, global );
	}
	catch( std::exception& e )
	{
		LOG_PRINTF_WARNING(( "std::exception: %s", e.what() ));
	}
	catch( boost::exception& e )
	{
		// handle exception
		LOG_PRINT_ERROR( "boost::exception" );
	}
	catch( boost::python::error_already_set& e )
	{
		// handle exception
		LOG_PRINT_ERROR( "python::exec() threw an exception 'error_already_set'. Error: " + GetExtraErrorInfo() );
	}

	res = true;//result

	m_pTargetScript = NULL;

	return res;
}


} // namespace amorphous
