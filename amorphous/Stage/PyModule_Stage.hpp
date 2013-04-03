#ifndef __PyModule_Stage_H__
#define __PyModule_Stage_H__


#include "fwd.hpp"
#include "gds/Script/fwd.hpp"
#include "gds/Script/PythonHeaders.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Physics/fwd.hpp"


namespace amorphous
{


void RegisterPythonModule_Stage( PythonScriptManager& mgr );


extern void SetStageForStageScriptCallback( CStage* pStage );
extern CStage *GetStageForScriptCallback();

extern CCopyEntity *CreateNamedEntity( const char *entity_name,
								       const char *base_name,
								       const Matrix34& pose,
								       const Vector3& vel,
									   physics::CActorDesc *pPhysActorDesc = NULL );

} // namespace amorphous



#endif  /*  __PyModule_Stage_H__  */
