#ifndef __BoostPythonScriptManager_HPP__
#define __BoostPythonScriptManager_HPP__


#include "ScriptManager.hpp"


namespace amorphous
{


class BoostPythonScriptManager : public ScriptManager
{

	bool LoadScript( const stream_buffer& buffer, PythonEventScript& dest_script );

public:

	BoostPythonScriptManager();

	~BoostPythonScriptManager();
};


} // namespace amorphous


#endif /* __BoostPythonScriptManager_HPP__ */
