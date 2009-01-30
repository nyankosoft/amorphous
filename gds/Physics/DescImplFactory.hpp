#ifndef __PhysDescImplFactory_H__
#define __PhysDescImplFactory_H__


#include "Support/Singleton.hpp"


#define PhysDescImplFactoryHolder (CDescImplFactoryHolder::Get())
#define PhysDescImplFactory (CDescImplFactoryHolder::Get()->GetFactory())


class CDescImplFactory
{
public:

	CDescImplFactory();
	~CDescImplFactory();

	virtual CActorDescImpl *CreateActorDescImpl() = 0;

	virtual CBoxShapeDescImpl *CreateBoxShapeDescImpl();
	virtual CCapsuleShapeDescImpl *CreateCapsuleShapeDescImpl() { return }
	virtual CSphereShapeDescImpl *CreateSphereShapeDescImpl();
	virtual CTriangleMeshShapeDescImpl *CreateTriangleMeshShapeDescImpl();
};


class CDescImplFactoryHolder
{
	CDescImplFactory *m_pFactory;

public:

	CDescImplFactoryHolder() : m_pFactory(NULL) {}

	~CDescImplFactoryHolder() { SafeDelete( m_pFactory ); }

	/// \param owned reference
	void Init( CDescImplFactory *pFactory ) { m_pFactory = pFactory; }

	CDescImplFactory *GetFactory() { return m_pFactory; }
};


#endif /* __PhysDescImplFactory_H__ */
