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
	CActor *CreateStaticTriangleMeshFromMeshFile( const std::string& filepath );

	CCloth *CreateClothFromMeshFile( const std::string& filepath, const Matrix34& world_pose );

	CCloth *CreateClothFromMesh( CCustomMesh& mesh, const Matrix34& world_pose, bool set_mesh_data );
};


} // namespace physics


#endif /* __PhysicsMeshUtility_HPP__ */
