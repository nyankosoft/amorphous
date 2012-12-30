#ifndef __SceneUtility_HPP__
#define __SceneUtility_HPP__


#include "../Graphics/fwd.hpp"
#include "../3DMath/Matrix34.hpp"
#include "../3DMath/Plane.hpp"
#include "fwd.hpp"


namespace amorphous
{


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

	CActor *CreateStaticPlaneActor( const Plane& plane, int material_index = 0 );
};


} // namespace physics

} // namespace amorphous



#endif /* __SceneUtility_HPP__ */
