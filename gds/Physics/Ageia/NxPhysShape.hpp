#ifndef  __NxPhysShape_H__
#define  __NxPhysShape_H__


#include "fwd.hpp"
#include "../Shape.hpp"
#include "NxPhysConv.hpp"

#include "NxPhysics.h"


namespace physics
{


class CNxPhysBoxShape : public CBoxShape
{
	NxBoxShape *m_pBox;

public:

//	CNxPhysBoxShape() {}
	CNxPhysBoxShape( NxBoxShape *pBox ) : m_pBox(pBox) { m_pBox->userData = this; }

	virtual ~CNxPhysBoxShape() {}

	virtual int GetMaterialID() const { return (int)m_pBox->getMaterial(); }

	virtual bool Raycast ( const CRay &world_ray, Scalar max_dist, U32 hintFlags, CRaycastHit &hit, bool first_hit ) const;

	void SetCollisionGroup ( U16 group ) { m_pBox->setGroup( group ); }
	U16 GetCollisionGroup() const { return (U16)m_pBox->getGroup(); }

	/// returns radii of the box
	virtual Vector3 GetDimensions () const { return ToVector3( m_pBox->getDimensions() ); }

	NxShape *GetNxShape() const { return m_pBox; }
};


bool CNxPhysBoxShape::Raycast( const CRay &world_ray,
							   Scalar max_dist, U32 hint_flags,
							   CRaycastHit &hit,
							   bool first_hit ) const
{
	NxRaycastHit nx_rayhit;// = ToNxRaycastHit(hit);
	bool res = m_pBox->raycast( ToNxRay(world_ray), max_dist, hint_flags, nx_rayhit, first_hit );
	hit = FromNxRaycastHit(nx_rayhit);
	return res;
}


class CNxPhysSphereShape : public CSphereShape
{
	NxSphereShape *m_pSphere;

public:

//	CNxPhysSphereShape() {}
	CNxPhysSphereShape( NxSphereShape *pSphere ) : m_pSphere(pSphere) { m_pSphere->userData = this; }

	virtual ~CNxPhysSphereShape() {}

	virtual bool Raycast ( const CRay &world_ray, Scalar max_dist, U32 hintFlags, CRaycastHit &hit, bool first_hit ) const;

	void SetCollisionGroup ( U16 group ) { m_pSphere->setGroup( group ); }
	U16 GetCollisionGroup() const { return (U16)m_pSphere->getGroup(); }

	virtual Scalar GetRadius () const { return (Scalar)(m_pSphere->getRadius()); }

	virtual void SetRadius( Scalar radius ) { m_pSphere->setRadius( radius ); }

	NxShape *GetNxShape() const { return m_pSphere; }
};


bool CNxPhysSphereShape::Raycast( const CRay &world_ray, Scalar max_dist, U32 hint_flags, CRaycastHit &hit, bool first_hit ) const
{
	return m_pSphere->raycast( ToNxRay(world_ray), max_dist, hint_flags, ToNxRaycastHit(hit), first_hit );
}


class CNxPhysCapsuleShape : public CCapsuleShape
{
	NxCapsuleShape *m_pCapsule;

public:

//	CNxPhysCapsuleShape() {}
	CNxPhysCapsuleShape( NxCapsuleShape *pCapsule ) : m_pCapsule(pCapsule) { m_pCapsule->userData = this; }

	virtual ~CNxPhysCapsuleShape() {}

	virtual bool Raycast ( const CRay &world_ray, Scalar max_dist, U32 hintFlags, CRaycastHit &hit, bool first_hit ) const;

	void SetCollisionGroup ( U16 group ) { m_pCapsule->setGroup( group ); }
	U16 GetCollisionGroup() const { return (U16)m_pCapsule->getGroup(); }

	virtual Scalar GetRadius () const { return m_pCapsule->getRadius(); }
	virtual Scalar GetLength () const { return m_pCapsule->getHeight(); }

	virtual void SetRadius( Scalar radius ) { m_pCapsule->setRadius( radius ); }
	virtual void SetLength( Scalar length ) { m_pCapsule->setHeight( length ); }

	NxShape *GetNxShape() const { return m_pCapsule; }
};


bool CNxPhysCapsuleShape::Raycast ( const CRay &world_ray,
										   Scalar max_dist,
										   U32 hint_flags, 
										   CRaycastHit &hit, bool first_hit ) const
{
	return m_pCapsule->raycast( ToNxRay(world_ray), max_dist, hint_flags, ToNxRaycastHit(hit), first_hit );
}



class CNxPhysTriangleMeshShape : public CTriangleMeshShape
{
	NxTriangleMeshShape *m_pTriangleMesh;

public:

	CNxPhysTriangleMeshShape( NxTriangleMeshShape *pTriangleMesh )
		:
	m_pTriangleMesh(pTriangleMesh) { m_pTriangleMesh->userData = this; }

	virtual ~CNxPhysTriangleMeshShape() {}

	virtual bool Raycast ( const CRay &world_ray, Scalar max_dist, U32 hintFlags, CRaycastHit &hit, bool first_hit ) const;

	void SetCollisionGroup ( U16 group ) { m_pTriangleMesh->setGroup( group ); }
	U16 GetCollisionGroup() const { return (U16)m_pTriangleMesh->getGroup(); }

	NxShape *GetNxShape() const { return m_pTriangleMesh; }
};


bool CNxPhysTriangleMeshShape::Raycast ( const CRay &world_ray,
										   Scalar max_dist,
										   U32 hint_flags,
										   CRaycastHit &hit, bool first_hit ) const
{
	NxRaycastHit nx_hit = ToNxRaycastHit(hit);
	bool res = m_pTriangleMesh->raycast( ToNxRay(world_ray), max_dist, hint_flags, nx_hit, first_hit );
	hit = FromNxRaycastHit( nx_hit );
	return res;
}


} // namespace physics


#endif /* __NxPhysShape_H__ */
