#ifndef __PYMODULE_ENTITY_H__
#define __PYMODULE_ENTITY_H__


#include "fwd.hpp"
#include "../Script/fwd.hpp"
#include "../Script/PythonHeaders.hpp"


namespace amorphous
{

namespace py
{

namespace entity
{


void RegisterPythonModule_Entity( PythonScriptManager& mgr );

extern void SetStageForEntityScriptCallback( CStage* pStage );

extern PyObject* SetTarget( PyObject* self, PyObject* args );
extern PyObject* SetPosition( PyObject* self, PyObject* args );
extern PyObject* SetTimeOffset( PyObject* self, PyObject* args );


}  /* entity */

}  /* py */

} // namespace amorphous



#endif  /*  __PYMODULE_ENTITY_H__  */
