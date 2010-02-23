#ifndef  __NxPhysCloth_H__
#define  __NxPhysCloth_H__

#include <math.h>

#include "../Cloth.hpp"
#include "fwd.hpp"
#include "NxMathConv.hpp"
#include "NxPhysConv.hpp"
#include "NxPhysShape.hpp"


#include "NxPhysics.h"


namespace physics
{


class CNxPhysCloth : public CCloth
{
	NxCloth *m_pCloth;

public:

	CNxPhysCloth( NxCloth *pCloth ) : m_pCloth(pCloth) {}
//	CNxPhysCloth() : m_pCloth(NULL) {}

	virtual ~CNxPhysCloth() {}

//	bool saveToDesc(ClothDesc& desc ) const;

//	NxClothMesh* GetClothMesh() const;

	void SetBendingStiffness( Scalar stiffness ) { m_pCloth->setBendingStiffness( stiffness ); }

	Scalar GetBendingStiffness() const { return m_pCloth->getBendingStiffness(); }

	void SetStretchingStiffness( Scalar stiffness ) { m_pCloth->setStretchingStiffness( stiffness ); }

	Scalar GetStretchingStiffness() const { return m_pCloth->getStretchingStiffness(); }

	void SetDampingCoefficient( Scalar dampingCoefficient ) { m_pCloth->setDampingCoefficient( dampingCoefficient ); }

	Scalar GetDampingCoefficient() const { return m_pCloth->getDampingCoefficient(); }

	void SetFriction( Scalar friction ) { m_pCloth->setFriction( friction ); }

	Scalar GetFriction() const { return m_pCloth->getFriction(); }

	void SetPressure( Scalar pressure ) { m_pCloth->setPressure( pressure ); }

	Scalar GetPressure() const { return m_pCloth->getPressure(); }

	void SetTearFactor( Scalar factor ) { m_pCloth->setTearFactor( factor ); }

	Scalar GetTearFactor() const { return m_pCloth->getTearFactor(); }

	void SetAttachmentTearFactor( Scalar factor ) { m_pCloth->setAttachmentTearFactor( factor ); }

	Scalar GetAttachmentTearFactor() const { return m_pCloth->getAttachmentTearFactor(); }

	void SetThickness( Scalar thickness ) { m_pCloth->setThickness( thickness ); }

	Scalar GetThickness() const { return m_pCloth->getThickness(); }

	Scalar GetDensity() const { return m_pCloth->getDensity(); }

	Scalar GetRelativeGridSpacing() const { return m_pCloth->getRelativeGridSpacing(); }

	U32 GetSolverIterations() const { return m_pCloth->getSolverIterations(); }

	void SetSolverIterations( U32 iterations ) { m_pCloth->setSolverIterations( iterations ); }

	void GetWorldBounds( AABB3& bounds ) const { NxBounds3 nx_bounds; m_pCloth->getWorldBounds(nx_bounds); bounds = ToAABB3(nx_bounds); }

	inline void AttachToShape( const CShape *pShape, U32 attachment_flags );

	inline void AttachToCollidingShapes( U32 attachment_flags );

	inline void DetachFromShape( const CShape *pShape );

	inline void AttachVertexToShape( U32 vertexId, const CShape *pShape, const Vector3 &localPos, U32 attachment_flags );

	void AttachVertexToGlobalPosition( const U32 vertexId, const Vector3 &pos ) { m_pCloth->attachVertexToGlobalPosition( vertexId, ToNxVec3(pos) ); }

	void FreeVertex( const U32 vertexId ) { m_pCloth->freeVertex( vertexId ); }

	void DominateVertex( U32 vertexId, Scalar expiration_time, Scalar dominance_weight ) { m_pCloth->dominateVertex( vertexId, expiration_time, dominance_weight ); }

//	NxClothVertexAttachmentStatus GetVertexAttachmentStatus( U32 vertexId ) const;

//	CShape* GetVertexAttachmentShape( U32 vertexId ) const;

//	Vector3 GetVertexAttachmentPosition( U32 vertexId ) const;

//	void AttachToCore( CActor *actor, Scalar impulseThreshold, Scalar penetrationDepth, Scalar maxDeformationDistance );

	bool TearVertex( const U32 vertexId, const Vector3 &normal ) { return m_pCloth->tearVertex( vertexId, ToNxVec3(normal) ); }

//	bool Raycast( const NxRay& worldRay, Vector3 &hit, U32 &vertexId ) { return false; }

	void SetGroup( U16 collision_group ) { m_pCloth->setGroup( collision_group ); }

	U16 GetGroup() const { return m_pCloth->getGroup(); }

//	void SetGroupsMask( const NxGroupsMask& groupsMask );

//	const NxGroupsMask GetGroupsMask() const;

	inline void SetMeshData( CMeshData& mesh_data );

	inline void GetMeshData( CMeshData& mesh_data );

//	NxMeshData GetMeshData();

	void SetValidBounds( const AABB3& valid_bounds ) { m_pCloth->setValidBounds( ToNxBounds3(valid_bounds) ); }

	void GetValidBounds( AABB3& valid_bounds ) const { NxBounds3 nx_bounds; m_pCloth->getValidBounds( nx_bounds ); valid_bounds = ToAABB3(nx_bounds); }

	void SetPosition( const Vector3& position, U32 vertexId ) { m_pCloth->setPosition( ToNxVec3(position), vertexId ); }

//	void SetPositions( void* buffer, U32 byteStride = sizeof(Vector3) );

	Vector3 GetPosition( U32 vertexId ) const { return ToVector3(m_pCloth->getPosition(vertexId)); }

//	void GetPositions(void* buffer, U32 byteStride = sizeof(Vector3) );

	void SetVelocity( const Vector3& velocity, U32 vertexId ) { m_pCloth->setVelocity( ToNxVec3(velocity), vertexId ); }

//	void SetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) );

	Vector3 GetVelocity( U32 vertexId ) const { return ToVector3(m_pCloth->getVelocity(vertexId)); }

//	void GetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) );

	U32 GetNumParticles() { return m_pCloth->getNumberOfParticles(); }
/*
	virtual U32 queryShapePointers( ) = 0;

	virtual U32 GetStateByteSize( ) = 0;

	virtual void GetShapePointers(NxShape** shapePointers,U32 *flags=0 ) = 0;

	virtual void SetShapePointers(NxShape** shapePointers,unsigned int numShapes ) = 0;

	virtual void SaveStateToStream(NxStream& stream, bool permute = false ) = 0;

	virtual void LoadStateFromStream(NxStream& stream ) = 0;

	virtual void SetCollisionResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetCollisionResponseCoefficient( ) const = 0;

	virtual void SetAttachmentResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetAttachmentResponseCoefficient( ) const = 0;

	virtual void SetFromFluidResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetFromFluidResponseCoefficient( ) const = 0;

	virtual void SetToFluidResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetToFluidResponseCoefficient( ) const = 0;

	virtual void SetExternalAcceleration(Vector3 acceleration ) = 0;

	virtual Vector3 GetExternalAcceleration( ) const = 0;

	virtual void SetMinAdhereVelocity( Scalar velocity ) = 0;

	virtual Scalar GetMinAdhereVelocity( ) const = 0;

	virtual void SetWindAcceleration(Vector3 acceleration ) = 0;

	virtual Vector3 GetWindAcceleration( ) const = 0;

	virtual bool isSleeping( ) const = 0;

	virtual Scalar GetSleepLinearVelocity( ) const = 0;

	virtual void SetSleepLinearVelocity( Scalar threshold ) = 0;

	virtual void wakeUp( Scalar wakeCounterValue = NX_SLEEP_INTERVAL ) = 0;

	virtual void putToSleep( ) = 0;

	virtual void SetFlags( U32 flags ) = 0;

	virtual U32 GetFlags( ) const = 0;

	virtual void SetName( const char* name ) = 0;

	virtual void addForceAtVertex( const Vector3& force, U32 vertexId, NxForceMode mode = NX_FORCE ) = 0;

	virtual void addForceAtPos( const Vector3& position, Scalar magnitude, Scalar radius, NxForceMode mode = NX_FORCE ) = 0;

	virtual void addDirectedForceAtPos( const Vector3& position, const Vector3& force, Scalar radius, NxForceMode mode = NX_FORCE ) = 0;

	virtual bool overlapAABBTriangles( const NxBounds3& bounds, U32& nb, const U32*& indices ) const = 0;

	virtual NxScene& GetScene( ) const = 0;

	virtual const char* GetName( ) const = 0;

	virtual NxCompartment *         getCompartment( ) const = 0;

	virtual     U32               getPPUTime()                                    const   = 0;

	virtual NxForceFieldMaterial    getForceFieldMaterial( ) const = 0;

	virtual void                    setForceFieldMaterial(NxForceFieldMaterial)  = 0;
*/

	NxCloth *GetNxCloth() { return m_pCloth; }
};


//================================= inline implementations =================================

inline NxShape *GetNxShape( const CShape *pShape )
{
	switch( pShape->GetType() )
	{
	case PhysShape::Sphere:  { const CNxPhysSphereShape *pNxSphere   = dynamic_cast<const CNxPhysSphereShape *>(pShape);  return pNxSphere ?  pNxSphere->GetNxShape() : NULL; }
	case PhysShape::Capsule: { const CNxPhysCapsuleShape *pNxCapsule = dynamic_cast<const CNxPhysCapsuleShape *>(pShape); return pNxCapsule ? pNxCapsule->GetNxShape() : NULL; }
	case PhysShape::Box:     { const CNxPhysBoxShape    *pNxBox      = dynamic_cast<const CNxPhysBoxShape *>(pShape);     return pNxBox ?     pNxBox->GetNxShape() : NULL; }
//	case PhysShape::TriangleMesh: { CNxTriangleMeshShapeImpl *pNxBox     = dynamic_cast<CNxPhysBoxShape *>(pShape);     return pNxBox ?     pNxBox->GetNxShape() : NULL; }
	default:
		return NULL;
	}

	return NULL;
}


inline void CNxPhysCloth::AttachToShape( const CShape *pShape, U32 attachment_flags )
{
	NxShape *pNxShape = GetNxShape(pShape);
	if( !pNxShape )
		return;

	NxU32 nx_flags = (NxU32)attachment_flags;
	m_pCloth->attachToShape( pNxShape, nx_flags );
}


inline void CNxPhysCloth::AttachToCollidingShapes( U32 attachment_flags )
{
}


inline void CNxPhysCloth::DetachFromShape( const CShape *pShape )
{
	NxShape *pNxShape = GetNxShape(pShape);
	if( !pNxShape )
		return;

	m_pCloth->detachFromShape( pNxShape );
}


inline void CNxPhysCloth::AttachVertexToShape( U32 vertexId, const CShape *pShape, const Vector3 &localPos, U32 attachment_flags )
{
	NxShape *pNxShape = GetNxShape(pShape);
	if( !pNxShape )
		return;

	NxU32 nx_flags = (NxU32)attachment_flags;
	m_pCloth->attachVertexToShape( vertexId, pNxShape, ToNxVec3(localPos), nx_flags );
}


inline void CNxPhysCloth::SetMeshData( CMeshData& mesh_data )
{
	NxMeshData nx_mesh_data;
	nx_mesh_data.verticesPosBegin         = mesh_data.pVerticesPosBegin;
	nx_mesh_data.verticesPosByteStride    = mesh_data.VerticesPosByteStride;
	nx_mesh_data.verticesNormalBegin      = mesh_data.pVerticesNormalBegin;
	nx_mesh_data.verticesNormalByteStride = mesh_data.VerticesNormalByteStride;
	nx_mesh_data.maxVertices              = mesh_data.NumMaxVertices;
	nx_mesh_data.indicesBegin             = mesh_data.pIndicesBegin;
	nx_mesh_data.maxIndices               = mesh_data.NumMaxIndices;
	nx_mesh_data.indicesByteStride        = mesh_data.IndicesByteStride;
//	nx_mesh_data.flags                    = mesh_data.Flags;
//	nx_mesh_data.dirtyBufferFlagsPtr      = ???
	// ...

	m_pCloth->setMeshData( nx_mesh_data );
}


inline void CNxPhysCloth::GetMeshData( CMeshData& mesh_data )
{
	NxMeshData nx_mesh_data;
	nx_mesh_data = m_pCloth->getMeshData();

	mesh_data.pVerticesPosBegin        = nx_mesh_data.verticesPosBegin;
	mesh_data.VerticesPosByteStride    = nx_mesh_data.verticesPosByteStride;
	mesh_data.pVerticesNormalBegin     = nx_mesh_data.verticesNormalBegin;
	mesh_data.VerticesNormalByteStride = nx_mesh_data.verticesNormalByteStride;
	mesh_data.NumMaxVertices           = nx_mesh_data.maxVertices;
	mesh_data.pIndicesBegin	           = nx_mesh_data.indicesBegin;
	mesh_data.NumMaxIndices	           = nx_mesh_data.maxIndices;
	mesh_data.IndicesByteStride        = nx_mesh_data.indicesByteStride;
//	mesh_data.Flags                    = nx_mesh_data.flags;
//	???                                = nx_mesh_data.dirtyBufferFlagsPtr;
//	...
}



} // namespace physics


#endif		/*  __NxPhysCloth_H__  */
