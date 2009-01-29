#ifndef __PYMODULE_TASK_H__
#define __PYMODULE_TASK_H__


#include <string>

#include <Python.h>

class CStage;


namespace gsf
{

namespace py
{

namespace task
{


extern PyMethodDef g_PyModuleTaskMethod[];

//extern void SetStageForEntityScriptCallback( CStage* pStage );


}  /* task */

}  /* py */

}  /* gsf */


#endif  /*  __PYMODULE_CAMERA_H__  */
