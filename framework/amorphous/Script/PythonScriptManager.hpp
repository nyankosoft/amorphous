#ifndef __PythonScriptManager_HPP__
#define __PythonScriptManager_HPP__


#include "ScriptManager.hpp"


namespace amorphous
{


class PythonScriptManager : public ScriptManager
{
	bool LoadScript( const stream_buffer& buffer, PythonEventScript& dest_script );

public:

	PythonScriptManager();

	~PythonScriptManager();

	void AddModule( const std::string& module_name, PyMethodDef method[] );

	void Update();
};


} // namespace amorphous


#endif /* __PythonScriptManager_HPP__ */
