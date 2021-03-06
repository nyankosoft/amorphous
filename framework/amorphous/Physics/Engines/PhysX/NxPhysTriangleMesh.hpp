#ifndef __NxPhysTriangleMesh_H__
#define __NxPhysTriangleMesh_H__


#include "fwd.hpp"
#include "3DMath.hpp"

//#include "NxPhysTriangleMeshDesc.hpp"
#include "amorphous/Physics/TriangleMesh.hpp"


namespace amorphous
{


namespace physics
{


class CNxPhysTriangleMesh : public CTriangleMesh
{
	NxTriangleMesh *m_pNxTriangleMesh;

	/// owner of the scene (borrowed reference)
	NxPhysicsSDK* m_pPhysicsSDK;

public:

//	CNxPhysTriangleMesh() {}

	CNxPhysTriangleMesh( NxTriangleMesh *pNxTriangleMesh, NxPhysicsSDK* pPhysicsSDK )
		:
	m_pNxTriangleMesh(pNxTriangleMesh),
	m_pPhysicsSDK(pPhysicsSDK)
	{}

	virtual ~CNxPhysTriangleMesh() { m_pPhysicsSDK->releaseTriangleMesh(*m_pNxTriangleMesh); }

	NxTriangleMesh *GetNxTriangleMesh() { return m_pNxTriangleMesh; }
};


} // namespace physics

} // namespace amorphous



#endif  /*  __NxPhysTriangleMesh_H__  */
