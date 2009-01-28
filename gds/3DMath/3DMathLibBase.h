#ifndef  __3DMATHLIBBASE_H__
#define  __3DMATHLIBBASE_H__


// NOTE: precision has to be float in order to use D3DX library as Vector3 class
#include "precision.h"


// If you want to use D3DXVECTOR3 class as Vector3 class,
// please define USE_D3DXVECTOR3 as a preprocessor
// preprocessor is defined either below or in project->settings->C/C++


// >>> comment this out to use platform-independent(?) Vector3 class
#ifdef WIN32
#define USE_D3DXVECTOR
#endif
// <<< comment this out to use platform-independent(?) Vector3 class


#endif  /*  __3DMATHLIBBASE_H__  */
