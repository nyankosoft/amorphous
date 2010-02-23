#ifndef __NxPhysClothMesh_H__
#define __NxPhysClothMesh_H__


#include "fwd.hpp"
#include "3DMath.hpp"

//#include "NxPhysClothMeshDesc.hpp"
#include "../TriangleMesh.hpp"


namespace physics
{


class CNxPhysClothMesh : public CTriangleMesh
{
	NxClothMesh *m_pNxClothMesh;

	/// owner of the scene (borrowed reference)
	NxPhysicsSDK* m_pPhysicsSDK;

public:

//	CNxPhysClothMesh() {}

	CNxPhysClothMesh( NxClothMesh *pNxClothMesh, NxPhysicsSDK* pPhysicsSDK )
		:
	m_pNxClothMesh(pNxClothMesh),
	m_pPhysicsSDK(pPhysicsSDK)
	{}

	virtual ~CNxPhysClothMesh() { m_pPhysicsSDK->releaseClothMesh(*m_pNxClothMesh); }

	NxClothMesh *GetNxClothMesh() { return m_pNxClothMesh; }
};


} // namespace physics


#endif  /*  __NxPhysClothMesh_H__  */
