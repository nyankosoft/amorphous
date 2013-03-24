#ifndef __PYMODULE_SOUND_H__
#define __PYMODULE_SOUND_H__


#include "../Script/fwd.hpp"
#include "../Script/PythonHeaders.hpp"


namespace amorphous
{


void RegisterPythonModule_Sound( PythonScriptManager& mgr );

extern void ReleaseAllScriptSounds();


} // namespace amorphous



#endif  /*  __PYMODULE_SOUND_H__  */