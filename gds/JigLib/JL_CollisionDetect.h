#ifndef  __JL_COLLISIONDETECT_H__
#define  __JL_COLLISIONDETECT_H__

#include "Support/PreAllocDynamicLinkList.h"

class CJL_PhysicsActor;
class CJL_Shape_Box;
class CJL_CollisionFunctor;
class CTriangleMesh;
class CJL_PhysicsManager;


extern void JL_CollisionDetect_All( TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList,
								    CJL_CollisionFunctor& rCollisionFunctor,
							        CJL_PhysicsManager *pPhysicsManager );

extern void JL_CollisionDetect_FrozenActor( CJL_PhysicsActor *pActor,
									        TCPreAllocDynamicLinkList<CJL_PhysicsActor>& rActorList,
									        CJL_CollisionFunctor& rCollisionFunctor );


extern void JL_CollisionDetect_Box_TriangleMesh( CJL_Shape_Box& rBox, CTriangleMesh& rTriMesh,
									             CJL_CollisionFunctor& rColFunctor );

extern void JL_CollisionDetect_Box_Box( CJL_Shape_Box& rBox0, CJL_Shape_Box& rBox1,
									    CJL_CollisionFunctor& rColFunctor );



//namespace JigLib
//{

class CJL_Shape_Capsule;


void JL_CollisionDetect_Capsule_Capsule( CJL_Shape_Capsule& rCapsule0, CJL_Shape_Capsule& rCapsule1,
									            CJL_CollisionFunctor& rColFunctor );

void JL_CollisionDetect_Box_Capsule( CJL_Shape_Box& rBox, CJL_Shape_Capsule& rCapsule,
									 CJL_CollisionFunctor& rColFunctor );

void JL_CollisionDetect_Capsule_TriangleMesh( CJL_Shape_Capsule& rCapsule, CTriangleMesh& rTriMesh,
											         CJL_CollisionFunctor& rColFunctor);

//}


#endif