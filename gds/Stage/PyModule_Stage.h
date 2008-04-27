#ifndef __PyModule_Stage_H__
#define __PyModule_Stage_H__


#include <string>
#include <Python.h>
#include "Stage/fwd.h"


extern PyMethodDef g_PyModuleStageMethod[];


extern void SetStageForStageScriptCallback( CStage* pStage );
extern CStage *GetStageForScriptCallback();


#endif  /*  __PyModule_Stage_H__  */
