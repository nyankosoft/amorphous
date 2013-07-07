#ifndef __NxPhysConvexMesh_H__
#define __NxPhysConvexMesh_H__


#include "amorphous/fwd.hpp"
#include "amorphous/3DMath.hpp"

//#include "NxPhysConvexMeshDesc.hpp"
#include "amorphous/Physics/ConvexMesh.hpp"


namespace amorphous
{


namespace physics
{


class CNxPhysConvexMesh : public CConvexMesh
{
	NxConvexMesh *m_pNxConvexMesh;

	/// owner of the scene (borrowed reference)
	NxPhysicsSDK* m_pPhysicsSDK;

public:

//	CNxPhysConvexMesh() {}

	CNxPhysConvexMesh( NxConvexMesh *pNxConvexMesh, NxPhysicsSDK* pPhysicsSDK )
		:
	m_pNxConvexMesh(pNxConvexMesh),
	m_pPhysicsSDK(pPhysicsSDK)
	{}

	virtual ~CNxPhysConvexMesh() { m_pPhysicsSDK->releaseConvexMesh(*m_pNxConvexMesh); }

	NxConvexMesh *GetNxConvexMesh() { return m_pNxConvexMesh; }
};


} // namespace physics

} // amorphous



#endif  /*  __NxPhysConvexMesh_H__  */
