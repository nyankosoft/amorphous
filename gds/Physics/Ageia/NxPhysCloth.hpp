#ifndef  __NxPhysCloth_H__
#define  __NxPhysCloth_H__

#include <math.h>

#include "../Cloth.hpp"
#include "fwd.hpp"
#include "NxMathConv.hpp"
#include "NxPhysConv.hpp"


#include "NxPhysics.h"


namespace physics
{


class CNxPhysCloth : public CCloth
{
	NxCloth *m_pCloth;

public:

	CCloth() {}

	virtual ~CCloth() {}

//	bool saveToDesc(ClothDesc& desc ) const;

//	NxClothMesh* GetClothMesh() const;

	void SetBendingStiffness( Scalar stiffness );

	Scalar GetBendingStiffness() const;

	void SetStretchingStiffness( Scalar stiffness );

	Scalar GetStretchingStiffness() const;

	void SetDampingCoefficient( Scalar dampingCoefficient );

	Scalar GetDampingCoefficient() const;

	void SetFriction( Scalar friction );

	Scalar GetFriction() const;

	void SetPressure( Scalar pressure );

	Scalar GetPressure() const;

	void SetTearFactor( Scalar factor );

	Scalar GetTearFactor() const;

	void SetAttachmentTearFactor( Scalar factor );

	Scalar GetAttachmentTearFactor() const;

	void SetThickness( Scalar thickness );

	Scalar GetThickness() const;

	Scalar GetDensity() const;

	Scalar GetRelativeGridSpacing() const;

	U32 GetSolverIterations() const;

	void SetSolverIterations( U32 iterations );

	void getWorldBounds(NxBounds3& bounds ) const;

	void AttachToShape( const NxShape *shape, U32 attachmentFlags );

	void AttachToCollidingShapes( U32 attachmentFlags );

	void detachFromShape( const NxShape *shape );

	void AttachVertexToShape( U32 vertexId, const NxShape *shape, const Vector3 &localPos, U32 attachmentFlags );

	void AttachVertexToGlobalPosition( const U32 vertexId, const Vector3 &pos );

	void FreeVertex( const U32 vertexId );

	void DominateVertex( U32 vertexId, Scalar expirationTime, Scalar dominanceWeight );

	NxClothVertexAttachmentStatus GetVertexAttachmentStatus( U32 vertexId ) const;

	NxShape* GetVertexAttachmentShape( U32 vertexId ) const;

	Vector3 GetVertexAttachmentPosition( U32 vertexId ) const;

	void AttachToCore(NxActor *actor, Scalar impulseThreshold, Scalar penetrationDepth = 0.0f, Scalar maxDeformationDistance = 0.0f );

	bool TearVertex( const U32 vertexId, const Vector3 &normal );

	bool raycast( const NxRay& worldRay, Vector3 &hit, U32 &vertexId );

	void SetGroup( U16 collisionGroup );

	U16 GetGroup() const;

	void SetGroupsMask( const NxGroupsMask& groupsMask );

	const NxGroupsMask GetGroupsMask() const;

	void SetMeshData( NxMeshData& meshData );

//	NxMeshData GetMeshData();

	void SetValidBounds( const NxBounds3& validBounds );

	void GetValidBounds( NxBounds3& validBounds ) const;

	void SetPosition( const Vector3& position, U32 vertexId );

	void SetPositions( void* buffer, U32 byteStride = sizeof(Vector3) );

	Vector3 GetPosition( U32 vertexId ) const;

	void GetPositions(void* buffer, U32 byteStride = sizeof(Vector3) );

	void SetVelocity( const Vector3& velocity, U32 vertexId );

	void SetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) );

	Vector3 GetVelocity( U32 vertexId ) const;

	void GetVelocities(void* buffer, U32 byteStride = sizeof(Vector3) );

	U32 GetNumberOfParticles();
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
};


//================================= inline implementations =================================


} // namespace physics


#endif		/*  __NxPhysCloth_H__  */
