#ifndef __PhysicsMeshUtility_HPP__
#define __PhysicsMeshUtility_HPP__


#include <string>
#include "gds/Graphics/fwd.hpp"
#include "gds/Physics/SceneUtility.hpp"


namespace physics
{


class CPhysicsMeshUtility : public CSceneUtility
{
public:

	CPhysicsMeshUtility( CScene *pScene )
		:
	CSceneUtility(pScene)
	{}

	// NOTE: not implemented.
	CActor *CreateStaticTriangleMeshActorFromMeshFile( const std::string& filepath );

	CActor *CreateConvexActorFromMesh( const CCustomMesh& src_mesh, const Matrix34& world_pose = Matrix34Identity(), Vector3 linear_velocity = Vector3(0,0,0), float mass = 1.0f, int material_index = 0 );

	CCloth *CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose );

	CCloth *CreateClothFromMesh( CCustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data );
};


} // namespace physics


#endif /* __PhysicsMeshUtility_HPP__ */
