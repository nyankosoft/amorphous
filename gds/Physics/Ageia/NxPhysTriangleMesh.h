#ifndef __NxPhysTriangleMesh_H__
#define __NxPhysTriangleMesh_H__


#include "fwd.h"
#include "3DMath.h"

//#include "NxPhysTriangleMeshDesc.h"
#include "../TriangleMesh.h"


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


#endif  /*  __NxPhysTriangleMesh_H__  */
