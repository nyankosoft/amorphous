#ifndef __PYMODULE_ENTITY_H__
#define __PYMODULE_ENTITY_H__


#include "gds/Script/PythonHeaders.hpp"


namespace amorphous
{


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
extern PyObject* SetTimeOffset( PyObject* self, PyObject* args );


}  /* entity */

}  /* py */

}  /* gsf */

} // namespace amorphous



#endif  /*  __PYMODULE_ENTITY_H__  */
