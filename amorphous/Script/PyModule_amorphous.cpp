#include "amorphous/Script/PyModule_amorphous.hpp"
#include <boost/python.hpp>

#include "amorphous/base.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace boost;


BOOST_PYTHON_MODULE(amorphous)
{
	using namespace boost::python;
	namespace py = boost::python;

	enum_< Result::Name >("Result")
		.value("SUCCESS",             Result::SUCCESS)
		.value("OUT_OF_MEMORY",       Result::OUT_OF_MEMORY)
		.value("INVALID_ARGS",        Result::INVALID_ARGS)
		.value("RESOURCE_NOT_FOUND",  Result::RESOURCE_NOT_FOUND)
		.value("RESOURCE_IN_USE",     Result::RESOURCE_IN_USE)
		.value("UNKNOWN_ERROR",       Result::UNKNOWN_ERROR)
	;
}


void RegisterPythonModule_amorphous()
{
	// Register the module with the interpreter
	if(PyImport_AppendInittab("amorphous", initamorphous) == -1)
	{
		const char *msg = "Failed to add 'amorphous' to the interpreter's builtin modules";
		LOG_PRINT_ERROR( msg );
		throw std::runtime_error( msg );
	}
}


} // namespace amorphous
