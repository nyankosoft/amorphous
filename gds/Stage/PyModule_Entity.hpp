#ifndef __PYMODULE_ENTITY_H__
#define __PYMODULE_ENTITY_H__


#include <Python.h>


class CStage;


namespace gsf
{

namespace py
{

namespace entity
{


extern PyMethodDef g_PyModuleEntityMethod[];

extern void SetStageForEntityScriptCallback( CStage* pStage );

extern PyObject* SetTarget( PyObject* self, PyObject* args );
extern PyObject* SetPosition( PyObject* self, PyObject* args );


}  /* entity */

}  /* py */

}  /* gsf */


#endif  /*  __PYMODULE_ENTITY_H__  */
