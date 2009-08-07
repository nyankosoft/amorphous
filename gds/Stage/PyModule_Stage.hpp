#ifndef __PyModule_Stage_H__
#define __PyModule_Stage_H__


#include "Script/PythonHeaders.hpp"
#include "Stage/fwd.hpp"
#include "3DMath/Matrix34.hpp"
#include "Physics/fwd.hpp"


extern PyMethodDef g_PyModuleStageMethod[];


extern void SetStageForStageScriptCallback( CStage* pStage );
extern CStage *GetStageForScriptCallback();

extern CCopyEntity *CreateNamedEntity( const char *entity_name,
								       const char *base_name,
								       const Matrix34& pose,
								       const Vector3& vel,
									   physics::CActorDesc *pPhysActorDesc = NULL );


#endif  /*  __PyModule_Stage_H__  */
