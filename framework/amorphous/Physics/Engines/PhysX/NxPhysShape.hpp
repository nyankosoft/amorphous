#ifndef  __NxPhysShape_H__
#define  __NxPhysShape_H__


#include "fwd.hpp"
#include "amorphous/Physics/Shape.hpp"
#include "NxPhysConv.hpp"

#include "NxPhysics.h"


namespace amorphous
{


namespace physics
{


class CNxPhysShapeImpl : public CShapeImpl
{
	NxShape *m_pNxShape;

public:

	CNxPhysShapeImpl( NxShape *pNxShape ) : m_pNxShape(pNxShape) {}

	virtual ~CNxPhysShapeImpl() {}

	void SetMaterialID( int material_id ) { m_pNxShape->setMaterial( (NxMaterialIndex)material_id ); }

	int GetMaterialID() const { return (int)m_pNxShape->getMaterial(); }

	Matrix34 GetLocalPose() const { return ToMatrix34( m_pNxShape->getLocalPose() ); }

	inline bool Raycast ( const CRay &world_ray, Scalar max_dist, U32 hintFlags, CRaycastHit &hit, bool first_hit ) const;

	void SetCollisionGroup( U16 group ) { m_pNxShape->setGroup( group ); }

	U16 GetCollisionGroup() const { return (U16)m_pNxShape->getGroup(); }

	NxShape *GetNxShape() const { return m_pNxShape; }
};


inline bool CNxPhysShapeImpl::Raycast( const CRay &world_ray,
							   Scalar max_dist, U32 hint_flags,
							   CRaycastHit &hit,
							   bool first_hit ) const
{
	NxRaycastHit nx_rayhit;// = ToNxRaycastHit(hit);
	bool res = m_pNxShape->raycast( ToNxRay(world_ray), max_dist, hint_flags, nx_rayhit, first_hit );
	if( res )
	{
		hit = FromNxRaycastHit(nx_rayhit);
		return true;
	}
	else
	{
		return false;
	}
}



class CNxPhysBoxShape : public CNxPhysShapeImpl
{
	NxBoxShape *m_pBox;

public:

//	CNxPhysBoxShape() {}
	CNxPhysBoxShape( NxBoxShape *pBox ) : CNxPhysShapeImpl(pBox), m_pBox(pBox) { m_pBox->userData = this; }

	virtual ~CNxPhysBoxShape() {}

	/// returns radii of the box
	virtual Vector3 GetDimensions() const { return ToVector3( m_pBox->getDimensions() ); }

	NxShape *GetNxShape() const { return m_pBox; }
};


class CNxPhysSphereShape : public CNxPhysShapeImpl
{
	NxSphereShape *m_pSphere;

public:

//	CNxPhysSphereShape() {}
	CNxPhysSphereShape( NxSphereShape *pSphere ) : CNxPhysShapeImpl(pSphere), m_pSphere(pSphere) { m_pSphere->userData = this; }

	virtual ~CNxPhysSphereShape() {}

	virtual Scalar GetRadius() const { return (Scalar)(m_pSphere->getRadius()); }

	virtual void SetRadius( Scalar radius ) { m_pSphere->setRadius( radius ); }
};


class CNxPhysCapsuleShape : public CNxPhysShapeImpl
{
	NxCapsuleShape *m_pCapsule;

public:

//	CNxPhysCapsuleShape() {}
	CNxPhysCapsuleShape( NxCapsuleShape *pCapsule ) : CNxPhysShapeImpl(pCapsule), m_pCapsule(pCapsule) { m_pCapsule->userData = this; }

	virtual ~CNxPhysCapsuleShape() {}

	Scalar GetRadius() const { return m_pCapsule->getRadius(); }
	Scalar GetLength() const { return m_pCapsule->getHeight(); }

	void SetRadius( Scalar radius ) { m_pCapsule->setRadius( radius ); }
	void SetLength( Scalar length ) { m_pCapsule->setHeight( length ); }
};


class CNxPhysTriangleMeshShape : public CNxPhysShapeImpl
{
	NxTriangleMeshShape *m_pTriangleMesh;

public:

	CNxPhysTriangleMeshShape( NxTriangleMeshShape *pTriangleMesh )
		:
	CNxPhysShapeImpl(pTriangleMesh),
	m_pTriangleMesh(pTriangleMesh)
	{
		m_pTriangleMesh->userData = this;
	}

	virtual ~CNxPhysTriangleMeshShape() {}
};


class CNxPhysConvexShape : public CNxPhysShapeImpl
{
	NxConvexShape *m_pConvex;

public:

	CNxPhysConvexShape( NxConvexShape *pConvex ) : CNxPhysShapeImpl(pConvex), m_pConvex(pConvex) { m_pConvex->userData = this; }

	virtual ~CNxPhysConvexShape() {}
};


class CNxPhysPlaneShape : public CNxPhysShapeImpl
{
	NxPlaneShape *m_pPlane;

public:

	CNxPhysPlaneShape( NxPlaneShape *pPlane ) : CNxPhysShapeImpl(pPlane), m_pPlane(pPlane) { m_pPlane->userData = this; }

	virtual ~CNxPhysPlaneShape() {}
};



} // namespace physics

} // namespace amorphous



#endif /* __NxPhysShape_H__ */
