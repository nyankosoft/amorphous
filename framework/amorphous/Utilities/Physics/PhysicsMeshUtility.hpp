#ifndef __PhysicsMeshUtility_HPP__
#define __PhysicsMeshUtility_HPP__


#include <string>
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Physics/SceneUtility.hpp"


namespace amorphous
{


namespace physics
{


class PhysicsMeshUtility : public CSceneUtility
{
public:

	PhysicsMeshUtility( CScene *pScene )
		:
	CSceneUtility(pScene)
	{}

	// NOTE: not implemented.
	CActor *CreateStaticTriangleMeshActorFromMeshFile( const std::string& filepath );

	CActor *CreateConvexActorFromMesh( const CustomMesh& src_mesh, const Matrix34& world_pose = Matrix34Identity(), Vector3 linear_velocity = Vector3(0,0,0), float mass = 1.0f, int material_index = 0 );

	CCloth *CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose );

	CCloth *CreateClothFromMesh( CustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data );
};


} // namespace physics

} // namespace amorphous



#endif /* __PhysicsMeshUtility_HPP__ */
