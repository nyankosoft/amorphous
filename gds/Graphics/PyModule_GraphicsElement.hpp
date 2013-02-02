#ifndef __PYMODULE_GRAPHICSELEMENT_H__
#define __PYMODULE_GRAPHICSELEMENT_H__


#include "../Script/PythonHeaders.hpp"


namespace amorphous
{


extern PyMethodDef g_PyModuleGraphicsElementMethod[];
extern PyMethodDef g_PyModuleAnimatedGraphicsMethod[];


class GraphicsElementManager;
class GraphicsElementAnimationManager;

void SetGraphicsElementManagerForScript( GraphicsElementManager *pManager );
void RemoveGraphicsElementManagerForScript();

void SetAnimatedGraphicsManagerForScript( GraphicsElementAnimationManager *pManager );
void RemoveAnimatedGraphicsManagerForScript();

} // namespace amorphous



#endif  /*  __PYMODULE_GRAPHICSELEMENT_H__  */
