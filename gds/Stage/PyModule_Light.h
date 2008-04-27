#ifndef __PyModule_Light_H__
#define __PyModule_Light_H__


#include <string>
#include <Python.h>
#include "Stage/fwd.h"


namespace gsf
{

namespace py
{

namespace light
{


extern PyMethodDef g_PyModuleLightMethod[];

//extern void SetStageForEntityScriptCallback( CStage* pStage );
extern void SetEntityForLight( CCopyEntity* pEntity );

extern PyObject* SetColor( PyObject* self, PyObject* args );
extern PyObject* SetColorU32( PyObject* self, PyObject* args );
extern PyObject* SetPosition( PyObject* self, PyObject* args );
extern PyObject* SetTargetEntity( PyObject* self, PyObject* args );


}  /* light */

}  /* py */

}  /* gsf */


#endif  /*  __PyModule_Light_H__  */
