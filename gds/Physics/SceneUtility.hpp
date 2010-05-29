#ifndef __SceneUtility_HPP__
#define __SceneUtility_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Physics/fwd.hpp"


namespace physics
{


class CSceneUtility
{
	CScene *m_pScene;

	CActor *CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, U32 body_flags );

protected:

	CScene *GetScene() { return m_pScene; }

public:

	CSceneUtility( CScene *pScene )
		:
	m_pScene(pScene)
	{}

	virtual ~CSceneUtility() {}

	CActor *CreateBoxActor( const Vector3& side_lengths, const Matrix34& world_pose, float mass = 1.0f );

	CActor *CreateStaticBoxActor( const Vector3& side_lengths, const Matrix34& world_pose );

	CActor *CreateKinematicBoxActor( const Vector3& side_lengths, const Matrix34& world_pose );

	CActor *CreateStaticCapsuleActor( float radius, float length, const Matrix34& world_pose );

	CActor *CreateStaticSphereActor( Scalar radius, const Matrix34& world_pose );
};


} // namespace physics


#endif /* __SceneUtility_HPP__ */
