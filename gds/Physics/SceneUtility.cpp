#include "SceneUtility.hpp"
#include "gds/Physics.hpp"

using namespace physics;


CActor *CSceneUtility::CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, float mass )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.fMass = mass;

	CBoxShapeDesc box;
	box.MaterialIndex = 1;
	box.vSideLength = side_lengths;
	actor_desc.vecpShapeDesc.push_back(&box);

	return m_pScene->CreateActor( actor_desc );
}


CActor *CSceneUtility::CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, U32 body_flags )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.Flags = body_flags;

	CBoxShapeDesc box;
	box.MaterialIndex = 0;
	box.vSideLength = side_lengths;
	actor_desc.vecpShapeDesc.push_back(&box);

	return m_pScene->CreateActor( actor_desc );
}


CActor *CSceneUtility::CreateStaticBoxActor( const Vector3& side_lengths, const Matrix34& world_pose )
{
	return CreateBoxActor( side_lengths, world_pose, (U32)PhysBodyFlag::Static );
}


CActor *CSceneUtility::CreateKinematicBoxActor( const Vector3& side_lengths, const Matrix34& world_pose )
{
	return CreateBoxActor( side_lengths, world_pose, (U32)PhysBodyFlag::Kinematic );
}


CActor *CSceneUtility::CreateStaticCapsuleActor( float radius, float length, const Matrix34& world_pose )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = world_pose;
	actor_desc.BodyDesc.Flags = PhysBodyFlag::Static;

	CCapsuleShapeDesc cap;
	cap.MaterialIndex = 0;
	cap.fRadius = radius;
	cap.fLength = length;
	actor_desc.vecpShapeDesc.push_back(&cap);

	return m_pScene->CreateActor( actor_desc );
}


CActor *CSceneUtility::CreateStaticPlaneActor( const Plane& plane, int material_index )
{
	if( !m_pScene )
		return NULL;

	CActorDesc actor_desc;
	actor_desc.WorldPose = Matrix34Identity();
	actor_desc.BodyDesc.Flags = PhysBodyFlag::Static;

	CPlaneShapeDesc plane_desc;
	plane_desc.MaterialIndex = material_index;
	plane_desc.plane = plane;
	actor_desc.vecpShapeDesc.push_back(&plane_desc);

	return m_pScene->CreateActor( actor_desc );
}
