#ifndef __PyModule_Light_H__
#define __PyModule_Light_H__


#include "fwd.hpp"
#include "gds/Script/PythonHeaders.hpp"


namespace gsf
{

namespace py
{

namespace light
{


extern PyMethodDef g_PyModuleLightMethod[];

extern void SetEntityForLight( CCopyEntity* pEntity );


extern PyObject* SetColor( PyObject* self, PyObject* args );
extern PyObject* SetColorU32( PyObject* self, PyObject* args );
extern PyObject* SetPosition( PyObject* self, PyObject* args );
extern PyObject* SetTargetLight( PyObject* self, PyObject* args );

extern PyObject* CreatePointLight( PyObject* self, PyObject* args, PyObject *keywords );        
extern PyObject* CreateDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords );  
extern PyObject* CreateSpotlight( PyObject* self, PyObject* args, PyObject *keywords );         
extern PyObject* CreateHSPointLight( PyObject* self, PyObject* args, PyObject *keywords );      
extern PyObject* CreateHSDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords );
extern PyObject* CreateHSSpotlight( PyObject* self, PyObject* args, PyObject *keywords );       
//extern PyObject* CreateTriPointLight( PyObject* self, PyObject* args );     
//extern PyObject* CreateTriDirectionalLight( PyObject* self, PyObject* args );
//extern PyObject* CreateTriSpotlight( PyObject* self, PyObject* args );      

extern PyObject* LoadPointLight( PyObject* self, PyObject* args, PyObject *keywords );        
extern PyObject* LoadDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords );  
//extern PyObject* LoadSpotlight( PyObject* self, PyObject* args, PyObject *keywords );         
extern PyObject* LoadHSPointLight( PyObject* self, PyObject* args, PyObject *keywords );      
extern PyObject* LoadHSDirectionalLight( PyObject* self, PyObject* args, PyObject *keywords );
//extern PyObject* LoadHSSpotlight( PyObject* self, PyObject* args );       
//extern PyObject* LoadTriPointLight( PyObject* self, PyObject* args );
//extern PyObject* LoadTriDirectionalLight( PyObject* self, PyObject* args );
//extern PyObject* LoadTriSpotlight( PyObject* self, PyObject* args );

}  /* light */

}  /* py */

}  /* gsf */


#endif  /*  __PyModule_Light_H__  */
