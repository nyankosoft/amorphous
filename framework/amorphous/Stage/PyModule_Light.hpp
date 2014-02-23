#ifndef __PyModule_Light_H__
#define __PyModule_Light_H__


#include "fwd.hpp"
#include "amorphous/Script/fwd.hpp"
#include "amorphous/Script/PythonHeaders.hpp"


namespace amorphous
{

namespace py
{

namespace light
{


void RegisterPythonModule_Light( PythonScriptManager& mgr );

extern void SetEntityForLight( CCopyEntity* pEntity );


extern PyObject* SetColor( PyObject* self, PyObject* args );
extern PyObject* SetColorU32( PyObject* self, PyObject* args );
extern PyObject* SetPosition( PyObject* self, PyObject* args );
extern PyObject* SetTargetLight( PyObject* self, PyObject* args );

extern PyObject* CreatePointLight(         PyObject* self, PyObject* args, PyObject *keywords );        
extern PyObject* CreateDirectionalLight(   PyObject* self, PyObject* args, PyObject *keywords );  
extern PyObject* CreateSpotlight(          PyObject* self, PyObject* args, PyObject *keywords );         
extern PyObject* CreateHSPointLight(       PyObject* self, PyObject* args, PyObject *keywords );      
extern PyObject* CreateHSDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords );
extern PyObject* CreateHSSpotlight(        PyObject* self, PyObject* args, PyObject *keywords );       
//extern PyObject* CreateTriPointLight( PyObject* self, PyObject* args );     
//extern PyObject* CreateTriDirectionalLight( PyObject* self, PyObject* args );
//extern PyObject* CreateTriSpotlight( PyObject* self, PyObject* args );      


}  /* light */

}  /* py */

} // namespace amorphous



#endif  /*  __PyModule_Light_H__  */
