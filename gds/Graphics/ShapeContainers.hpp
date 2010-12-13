#ifndef __ShapeContainers_HPP__
#define __ShapeContainers_HPP__


#include "../3DMath/AABB3.hpp"
#include "../3DMath/OBB3.hpp"
#include "../3DMath/Capsule.hpp"
#include "../Support/SafeDeleteVector.hpp"
#include "../Support/Serialization/Serialization.hpp"
#include "../Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;


class CAABB3Container;
class COBB3Container;
class CSphereContainer;
class CCapsuleContainer;
class CConvexContainer;


class CShapeContainerVisitor
{
public:

	virtual ~CShapeContainerVisitor() {}
	virtual void VisitAABB3Container( CAABB3Container& aabb3_container ) {}
	virtual void VisitOBB3Container( COBB3Container& obb3_container ) {}
	virtual void VisitSphereContainer( CSphereContainer& sphere_container ) {}
	virtual void VisitCapsuleContainer( CCapsuleContainer& capsule_container ) {}
	virtual void VisitConvexContainer( CConvexContainer& convex_container ) {}
};


class CShapeContainer : public IArchiveObjectBase
{
public:
	CShapeContainer() {}
	virtual ~CShapeContainer() {}

	virtual void Accept( CShapeContainerVisitor& visitor ) {}

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

class CAABB3Container : public CShapeContainer
{
public:
	AABB3 aabb;

	CAABB3Container( const AABB3& _aabb = AABB3() )
		:
	aabb(_aabb)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & aabb; }

	unsigned int GetArchiveObjectID() const { return AABB3_CONTAINER; }

	void Accept( CShapeContainerVisitor& visitor ) { visitor.VisitAABB3Container( *this ); }
};

class COBB3Container : public CShapeContainer
{
public:
	OBB3 obb;

	COBB3Container( const OBB3& _obb = OBB3() )
		:
	obb(_obb)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & obb; }

	unsigned int GetArchiveObjectID() const { return OBB3_CONTAINER; }

	void Accept( CShapeContainerVisitor& visitor ) { visitor.VisitOBB3Container( *this ); }
};

class CSphereContainer : public CShapeContainer
{
public:
	Sphere sphere;

	CSphereContainer( const Sphere& _sphere = Sphere() )
		:
	sphere(_sphere)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & sphere; }

	unsigned int GetArchiveObjectID() const { return SPHERE_CONTAINER; }

	void Accept( CShapeContainerVisitor& visitor ) { visitor.VisitSphereContainer( *this ); }
};

class CCapsuleContainer : public CShapeContainer
{
public:
	Capsule capsule;

	CCapsuleContainer( const Capsule& cap = Capsule() )
		:
	capsule(cap)
	{}

	void Serialize( IArchive& ar, const unsigned int version ) { ar & capsule; }

	unsigned int GetArchiveObjectID() const { return CAPSULE_CONTAINER; }

	void Accept( CShapeContainerVisitor& visitor ) { visitor.VisitCapsuleContainer( *this ); }
};


class CConvexContainer : public CShapeContainer
{
public:
	std::vector<Vector3> points;
	std::vector<int> indices;

	void Serialize( IArchive& ar, const unsigned int version ) { ar & points & indices; }

	unsigned int GetArchiveObjectID() const { return CONVEX_CONTAINER; }

	void Accept( CShapeContainerVisitor& visitor ) { visitor.VisitConvexContainer( *this ); }
};


class CShapeContainerFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject(const unsigned int id)
	{
		switch( id )
		{
		case CShapeContainer::AABB3_CONTAINER:   return new CAABB3Container;
		case CShapeContainer::OBB3_CONTAINER:    return new COBB3Container;
		case CShapeContainer::SPHERE_CONTAINER:  return new CSphereContainer;
		case CShapeContainer::CAPSULE_CONTAINER: return new CCapsuleContainer;
		case CShapeContainer::CONVEX_CONTAINER:  return new CConvexContainer;
		default:
			return NULL;
		}
	}
};


class CShapeContainerSet : public IArchiveObjectBase
{
public:

	std::vector<CShapeContainer *> m_pShapes;

	CShapeContainerSet() {}
	~CShapeContainerSet() { SafeDeleteVector(m_pShapes); }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeContainerFactory factory;
		ar.Polymorphic( m_pShapes, factory );
	}
};



#endif /* __ShapeContainers_HPP__ */
