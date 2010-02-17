#ifndef __PhysCloth_HPP__
#define __PhysCloth_HPP__


#include <vector>
#include "../base.hpp"
#include "3DMath.hpp"
#include "Support/SafeDeleteVector.hpp"

#include "fwd.hpp"
#include "Enums.hpp"
#include "ClothDesc.hpp"


namespace physics
{


class CCloth
{
public:

	CCloth() {}

	virtual ~CCloth() {}

//	virtual bool saveToDesc(ClothDesc& desc ) const = 0;

//	virtual NxClothMesh* GetClothMesh() const = 0;

	virtual void SetBendingStiffness( Scalar stiffness ) = 0;

	virtual Scalar GetBendingStiffness() const = 0;

	virtual void SetStretchingStiffness( Scalar stiffness ) = 0;

	virtual Scalar GetStretchingStiffness() const = 0;

	virtual void SetDampingCoefficient( Scalar dampingCoefficient ) = 0;

	virtual Scalar GetDampingCoefficient() const = 0;

	virtual void SetFriction( Scalar friction ) = 0;

	virtual Scalar GetFriction() const = 0;

	virtual void SetPressure( Scalar pressure ) = 0;

	virtual Scalar GetPressure() const = 0;

	virtual void SetTearFactor( Scalar factor ) = 0;

	virtual Scalar GetTearFactor() const = 0;

	virtual void SetAttachmentTearFactor( Scalar factor ) = 0;

	virtual Scalar GetAttachmentTearFactor() const = 0;

	virtual void SetThickness( Scalar thickness ) = 0;

	virtual Scalar GetThickness() const = 0;

	virtual Scalar GetDensity() const = 0;

	virtual Scalar GetRelativeGridSpacing() const = 0;

	virtual U32 GetSolverIterations() const = 0;

	virtual void SetSolverIterations( U32 iterations ) = 0;

	virtual void getWorldBounds(NxBounds3& bounds ) const = 0;

	virtual void AttachToShape( const NxShape *shape, U32 attachmentFlags ) = 0;

	virtual void AttachToCollidingShapes( U32 attachmentFlags ) = 0;

	virtual void detachFromShape( const NxShape *shape ) = 0;

	virtual void AttachVertexToShape( U32 vertexId, const NxShape *shape, const Vector3 &localPos, U32 attachmentFlags ) = 0;

	virtual void AttachVertexToGlobalPosition( const U32 vertexId, const Vector3 &pos ) = 0;

	virtual void freeVertex( const U32 vertexId ) = 0;

	virtual void dominateVertex( U32 vertexId, Scalar expirationTime, Scalar dominanceWeight ) = 0;

	virtual NxClothVertexAttachmentStatus getVertexAttachmentStatus( U32 vertexId ) const = 0;

	virtual NxShape* GetVertexAttachmentShape( U32 vertexId ) const = 0;

	virtual Vector3 GetVertexAttachmentPosition( U32 vertexId ) const = 0;

	virtual void AttachToCore(NxActor *actor, Scalar impulseThreshold, Scalar penetrationDepth = 0.0f, Scalar maxDeformationDistance = 0.0f ) = 0;

	virtual bool TearVertex( const U32 vertexId, const Vector3 &normal ) = 0;

	virtual bool raycast( const NxRay& worldRay, Vector3 &hit, U32 &vertexId ) = 0;

	virtual void SetGroup( U16 collisionGroup ) = 0;

	virtual U16 GetGroup() const = 0;

	virtual void SetGroupsMask( const NxGroupsMask& groupsMask ) = 0;

	virtual const NxGroupsMask GetGroupsMask() const = 0;

	virtual void SetMeshData(NxMeshData& meshData ) = 0;

//	virtual NxMeshData GetMeshData( ) = 0;

	virtual void SetValidBounds( const NxBounds3& validBounds ) = 0;

	virtual void getValidBounds(NxBounds3& validBounds ) const = 0;

	virtual void SetPosition( const Vector3& position, U32 vertexId ) = 0;

	virtual void SetPositions(void* buffer, U32 byteStride = sizeof(Vector3) ) = 0;

	virtual Vector3 GetPosition( U32 vertexId ) const = 0;

	virtual void GetPositions(void* buffer, U32 byteStride = sizeof(Vector3) ) = 0;

	virtual void SetVelocity( const Vector3& velocity, U32 vertexId ) = 0;

	virtual void SetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) ) = 0;

	virtual Vector3 GetVelocity( U32 vertexId ) const = 0;

	virtual void GetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) ) = 0;

	virtual U32 GetNumberOfParticles( ) = 0;
/*
	virtual U32 queryShapePointers( ) = 0;

	virtual U32 GetStateByteSize( ) = 0;

	virtual void GetShapePointers(NxShape** shapePointers,U32 *flags=0 ) = 0;

	virtual void SetShapePointers(NxShape** shapePointers,unsigned int numShapes ) = 0;

	virtual void SaveStateToStream(NxStream& stream, bool permute = false ) = 0;

	virtual void LoadStateFromStream(NxStream& stream ) = 0;

	virtual void SetCollisionResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetCollisionResponseCoefficient() const = 0;

	virtual void SetAttachmentResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetAttachmentResponseCoefficient() const = 0;

	virtual void SetFromFluidResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetFromFluidResponseCoefficient() const = 0;

	virtual void SetToFluidResponseCoefficient( Scalar coefficient ) = 0;

	virtual Scalar GetToFluidResponseCoefficient() const = 0;

	virtual void SetExternalAcceleration(Vector3 acceleration ) = 0;

	virtual Vector3 GetExternalAcceleration() const = 0;

	virtual void SetMinAdhereVelocity( Scalar velocity ) = 0;

	virtual Scalar GetMinAdhereVelocity() const = 0;

	virtual void SetWindAcceleration(Vector3 acceleration ) = 0;

	virtual Vector3 GetWindAcceleration() const = 0;

	virtual bool isSleeping() const = 0;

	virtual Scalar GetSleepLinearVelocity() const = 0;

	virtual void SetSleepLinearVelocity( Scalar threshold ) = 0;

	virtual void wakeUp( Scalar wakeCounterValue = NX_SLEEP_INTERVAL ) = 0;

	virtual void putToSleep( ) = 0;

	virtual void SetFlags( U32 flags ) = 0;

	virtual U32 GetFlags() const = 0;

	virtual void SetName( const char* name ) = 0;

	virtual void addForceAtVertex( const Vector3& force, U32 vertexId, NxForceMode mode = NX_FORCE ) = 0;

	virtual void addForceAtPos( const Vector3& position, Scalar magnitude, Scalar radius, NxForceMode mode = NX_FORCE ) = 0;

	virtual void addDirectedForceAtPos( const Vector3& position, const Vector3& force, Scalar radius, NxForceMode mode = NX_FORCE ) = 0;

	virtual bool overlapAABBTriangles( const NxBounds3& bounds, U32& nb, const U32*& indices ) const = 0;

	virtual NxScene& GetScene() const = 0;

	virtual const char* GetName() const = 0;

	virtual NxCompartment *         getCompartment() const = 0;

	virtual     U32               getPPUTime()                                    const   = 0;

	virtual NxForceFieldMaterial    getForceFieldMaterial() const = 0;

	virtual void                    setForceFieldMaterial(NxForceFieldMaterial)  = 0;
*/
};


} // namespace physics


//================== inline implementations =======================

//#include "Cloth.inl"


#endif  /*  __PhysCloth_HPP__  */
