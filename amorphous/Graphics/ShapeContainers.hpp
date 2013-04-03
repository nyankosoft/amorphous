#ifndef __ShapeContainers_HPP__
#define __ShapeContainers_HPP__


#include "../3DMath/AABB3.hpp"
#include "../3DMath/OBB3.hpp"
#include "../3DMath/Capsule.hpp"
#include "../Support/SafeDeleteVector.hpp"
#include "../Support/Serialization/Serialization.hpp"
#include "../Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{
using namespace serialization;


class AABB3Container;
class OBB3Container;
class SphereContainer;
class CapsuleContainer;
class ConvexContainer;


class ShapeContainerVisitor
{
public:

	virtual ~ShapeContainerVisitor() {}
	virtual void VisitAABB3Container( AABB3Container& aabb3_container ) {}
	virtual void VisitOBB3Container( OBB3Container& obb3_container ) {}
	virtual void VisitSphereContainer( SphereContainer& sphere_container ) {}
	virtual void VisitCapsuleContainer( CapsuleContainer& capsule_container ) {}
	virtual void VisitConvexContainer( ConvexContainer& convex_container ) {}
};


class ShapeContainer : public IArchiveObjectBase
{
public:
	ShapeContainer() {}
	virtual ~ShapeContainer() {}

	virtual void Accept( ShapeContainerVisitor& visitor ) {}

	enum ContainerArchiveID
	{
		AABB3_CONTAINER,
		OBB3_CONTAINER,
		SPHERE_CONTAINER,
		CAPSULE_CONTAINER,
		CONVEX_CONTAINER,
		NUM_CONTAINER_ARCHIVE_IDS
	};
};

class AABB3Container : public ShapeContainer
{
public:
	AABB3 aabb;

	AABB3Container( const AABB3& _aabb = AABB3() )
		:
	aabb(_aabb)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & aabb; }

	unsigned int GetArchiveObjectID() const { return AABB3_CONTAINER; }

	void Accept( ShapeContainerVisitor& visitor ) { visitor.VisitAABB3Container( *this ); }
};

class OBB3Container : public ShapeContainer
{
public:
	OBB3 obb;

	OBB3Container( const OBB3& _obb = OBB3() )
		:
	obb(_obb)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & obb; }

	unsigned int GetArchiveObjectID() const { return OBB3_CONTAINER; }

	void Accept( ShapeContainerVisitor& visitor ) { visitor.VisitOBB3Container( *this ); }
};

class SphereContainer : public ShapeContainer
{
public:
	Sphere sphere;

	SphereContainer( const Sphere& _sphere = Sphere() )
		:
	sphere(_sphere)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & sphere; }

	unsigned int GetArchiveObjectID() const { return SPHERE_CONTAINER; }

	void Accept( ShapeContainerVisitor& visitor ) { visitor.VisitSphereContainer( *this ); }
};

class CapsuleContainer : public ShapeContainer
{
public:
	Capsule capsule;

	CapsuleContainer( const Capsule& cap = Capsule() )
		:
	capsule(cap)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & capsule; }

	unsigned int GetArchiveObjectID() const { return CAPSULE_CONTAINER; }

	void Accept( ShapeContainerVisitor& visitor ) { visitor.VisitCapsuleContainer( *this ); }
};


class ConvexContainer : public ShapeContainer
{
public:
	std::vector<Vector3> points;
	std::vector<int> indices;

	void Serialize( IArchive& ar, const unsigned int version ) { ar & points & indices; }

	unsigned int GetArchiveObjectID() const { return CONVEX_CONTAINER; }

	void Accept( ShapeContainerVisitor& visitor ) { visitor.VisitConvexContainer( *this ); }
};


class ShapeContainerFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject(const unsigned int id)
	{
		switch( id )
		{
		case ShapeContainer::AABB3_CONTAINER:   return new AABB3Container;
		case ShapeContainer::OBB3_CONTAINER:    return new OBB3Container;
		case ShapeContainer::SPHERE_CONTAINER:  return new SphereContainer;
		case ShapeContainer::CAPSULE_CONTAINER: return new CapsuleContainer;
		case ShapeContainer::CONVEX_CONTAINER:  return new ConvexContainer;
		default:
			return NULL;
		}
	}
};


class ShapeContainerSet : public IArchiveObjectBase
{
public:

	std::vector<ShapeContainer *> m_pShapes;

	ShapeContainerSet() {}
	~ShapeContainerSet() { SafeDeleteVector(m_pShapes); }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ShapeContainerFactory factory;
		ar.Polymorphic( m_pShapes, factory );
	}
};


} // namespace amorphous



#endif /* __ShapeContainers_HPP__ */
