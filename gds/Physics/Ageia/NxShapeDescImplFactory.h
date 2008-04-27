#ifndef __NxShapeDescImplFactory_H__
#define __NxShapeDescImplFactory_H__


class CNxShapeDescImplFactory : public CShapeDescImplFactory
{
public:

	virtual CBoxShapeDescImpl *CreateBoxShapeDescImpl() { return CNxBoxShapeDescImpl(); }

	virtual CSphereShapeDescImpl *CreateSphereShapeDescImpl() { return CNxSphereShapeDescImpl(); }

	virtual CCapsuleShapeDescImpl *CreateCapsuleShapeDescImpl() { return CNxCapsuleShapeDescImpl(); }

	virtual CTriangleMeshShapeDescImpl *CreateTriangleMeshShapeDescImpl() {}
};


#endif /* __NxShapeDescImplFactory_H__ */
