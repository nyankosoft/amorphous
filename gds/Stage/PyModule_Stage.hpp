#ifndef __PyModule_Stage_H__
#define __PyModule_Stage_H__


#include <string>
#include <Python.h>
#include "Stage/fwd.hpp"
#include "3DMath/Vector3.hpp"


extern PyMethodDef g_PyModuleStageMethod[];


extern void SetStageForStageScriptCallback( CStage* pStage );
extern CStage *GetStageForScriptCallback();

extern CCopyEntity *CreateNamedEntity( const char *entity_name,
								const char *base_name,
								const Vector3& pos,
								const Vector3& dir,
								const Vector3& vel );



#endif  /*  __PyModule_Stage_H__  */
