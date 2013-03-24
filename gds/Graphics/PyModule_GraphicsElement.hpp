#ifndef __PYMODULE_GRAPHICSELEMENT_H__
#define __PYMODULE_GRAPHICSELEMENT_H__


#include "fwd.hpp"
#include "../Script/fwd.hpp"
#include "../Script/PythonHeaders.hpp"


namespace amorphous
{

//void RegisterPythonModule_Graphics( PythonScriptManager& mgr );
//void RegisterPythonModule_Animation( PythonScriptManager& mgr );

void AddPythonModule_Graphics();
void AddPythonModule_Animation();

void SetGraphicsElementManagerForScript( GraphicsElementManager *pManager );
void RemoveGraphicsElementManagerForScript();

void SetAnimatedGraphicsManagerForScript( GraphicsElementAnimationManager *pManager );
void RemoveAnimatedGraphicsManagerForScript();

} // namespace amorphous



#endif  /*  __PYMODULE_GRAPHICSELEMENT_H__  */
