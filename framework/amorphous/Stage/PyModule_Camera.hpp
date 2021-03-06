#ifndef __PYMODULE_CAMERA_H__
#define __PYMODULE_CAMERA_H__


#include "fwd.hpp"
#include "amorphous/Script/fwd.hpp"
#include "amorphous/Script/PythonHeaders.hpp"


/*
 =======================================
 Using scripted camera for cutscene
 =======================================

 1. create camera controller
    >>> Stage.CreateNamedEntityHrz( "cc", "CameraController", 0,0, 0,0, 0,0 )

 2. create camera entity
    >>> cam.CreateCamera( "cam0" )

 3. start camera script settings mode

 4. set pose / camera params / screen effects

 5. end camera script settings mode
 */

namespace amorphous
{

namespace py
{

namespace cam
{


void RegisterPythonModule_Camera( PythonScriptManager& mgr );

extern void SetStageForEntityScriptCallback( CStage* pStage );

extern PyObject* SetTimeOffset( PyObject* self, PyObject* args );
extern PyObject* SetPose( PyObject* self, PyObject* args );


}  /* cam */

}  /* py */

} // namespace amorphous



#endif  /*  __PYMODULE_CAMERA_H__  */
