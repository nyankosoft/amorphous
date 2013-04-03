#include "EmbeddedPythonModule.hpp"
#include "../Support/Log/DefaultLog.hpp"


namespace amorphous
{


PyObject *RegisterEmbeddedPythonModule( const char *module_name, const char *source )
{
	std::string filename = std::string("embedded:") + module_name;
	PyObject *pCompiledCodeObject = Py_CompileString( source, filename.c_str(), Py_file_input );

	if( !pCompiledCodeObject )
		LOG_PRINT_ERROR( "Py_CompileString() returned NULL." );

	// Copy the module name because PyImport_ExecCodeModule does not accept const char *
	const int max_module_name_length = 256;
	char module_name_copy[max_module_name_length];
	memset( module_name_copy, 0, sizeof(module_name_copy) );
	strcpy( module_name_copy, module_name );

	PyObject *pModule = PyImport_ExecCodeModule( module_name_copy, pCompiledCodeObject );

	if( !pModule )
		LOG_PRINT_ERROR( "PyImport_ExecCodeModule() returned NULL." );

	return pModule;
}


PyObject *RegisterEmbeddedPythonModule( const CEmbeddedPythonModule& module )
{
	return RegisterEmbeddedPythonModule( module.GetModuleName(), module.GetSource() );
}


} // namespace amorphous
