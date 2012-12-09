#ifndef __EmbeddedPythonModule_HPP__
#define __EmbeddedPythonModule_HPP__


// Include the boost python header even if you are not using it
// for the application to run.
// i.e. Do not include Python.h directly if you link with the boost python library
// whether you are using boost python or not.
#include <boost/python.hpp>


class CEmbeddedPythonModule
{
	const char *m_pModuleName;
	const char *m_pSource;

public:

	CEmbeddedPythonModule( const char *pModuleName, const char *pSource )
	:
	m_pModuleName(pModuleName),
	m_pSource(pSource)
	{}

	~CEmbeddedPythonModule(){}

	const char *GetModuleName() const { return m_pModuleName; }
	const char *GetSource()     const { return m_pSource; }
};


//PyObject *RegisterEmbeddedPythonModule( const char *module_name, const char *source );
PyObject *RegisterEmbeddedPythonModule( const CEmbeddedPythonModule& module );


#endif /* __EmbeddedPythonModule_HPP__ */
