#ifndef __COPYENTITY_H__
#define __COPYENTITY_H__


#include "../base.h"
#include "3DMath/Matrix34.h"
#include "3DMath/3DStructs.h"
#include "Support/FixedVector.h"
#include "3DCommon/MeshObjectHandle.h"

#include "CopyEntityCallback.h"

#include "fwd.h"
#include "BaseEntity.h"

class CJL_PhysicsActor;


// all the copy entities are linked to either 'm_pEngagedEntity' list or 'm_pEmptyEntity' list
//

enum eEntityState
{
	CESTATE_ATREST						= (1 << 0),
	CESTATE_ONGROUND					= (1 << 1),
	CESTATE_FROZEN						= (1 << 2),
	CESTATE_MOVED_DURING_LAST_FRAME		= (1 << 3),
	CESTATE_LIGHT_INFORMATION_INVALID	= (1 << 4),	///< when turned on, light information is updated before drawing the entity
};


enum eEntityFlag
{
	BETYPE_PLAYER                      = (1 << 0),
	BETYPE_TERRESTRIAL                 = (1 << 1),
	BETYPE_FLOATING                    = (1 << 2),
	BETYPE_ENEMY                       = (1 << 3),
	BETYPE_ITEM                        = (1 << 4),
	BETYPE_RIGIDBODY                   = (1 << 5),
	BETYPE_INDESTRUCTIBLE              = (1 << 6),
	BETYPE_LIGHTING                    = (1 << 7),  ///< set to enable simple lighting for entities
	BETYPE_NOCLIP                      = (1 << 8),
	BETYPE_NOCLIP_WITH_STATIC_GEOMETRY = (1 << 9),
	BETYPE_GLARESOURCE                 = (1 << 10),
	BETYPE_GLAREHINDER                 = (1 << 11),
	BETYPE_USE_ZSORT                   = (1 << 12), ///< use z-sort for translucent entities (rendering)
	BETYPE_VISIBLE                     = (1 << 13), ///< if turned on, the entity is rendered by Draw() method of its base entity
	BETYPE_USE_PHYSSIM_RESULTS         = (1 << 14), ///< if turned on, the entity pose & velocity is determined by physics simulator
	BETYPE_ENVMAPTARGET                = (1 << 15),
	BETYPE_SHADOW_CASTER               = (1 << 16), ///< turned on if the entity casts shadows to other entities
	BETYPE_SHADOW_RECEIVER             = (1 << 17), ///< turned on if the entity receives shadows cast by other entities  others
};

/*
/// glare types - not used any more
/// - deprecated
#define CEGT_NOGLARE		0
#define CEGT_GLARESOURCE	1
#define CEGT_GLAREHINDER	2
*/

#define NUM_MAX_ENTITY_TOUCHES_PER_FRAME	4
#define NUM_MAX_LIGHTS_AT_ENTITY	4
#define NUM_MAX_CHILDREN_PER_ENTITY	4

#define CE_INVALID_LIGHT_INDEX	-1

#define CE_INVALID_EXTRA_DATA_INDEX	-1


/**
 Stores extrinsic properties of entity
  -intrinsic properties are stored in base entity
*/
class CCopyEntity
{
	std::string strName;	///< individual name for entity

	U32 m_ID; ///< unique id for entity (0 == invalid id)

	int m_TypeID;

	int m_StockID;

	CStage *m_pStage;

	/// next entity in the chain list 'm_pEntityInUse'
	CCopyEntity* m_pNext;

	CCopyEntity* m_pNextEntity;   ///< next entity in the chain linked to entity tree-node 'CEntityNode'
	CCopyEntity* m_pPrevEntity;   ///< prev entity in the chain linked to entity tree-node 'CEntityNode'

	/// next entity in the link list for z-sorting (used to render transparent entities)
	CCopyEntity* m_pNextEntityInZSortTable;

	/// used for z-sort of translucent entities (rendering)
	float fZinCameraSpace;

	/// true if currenly in use
	bool inuse;

protected:

	CStage *GetStage() { return m_pStage; }

public:

	CBaseEntity* pBaseEntity;

	/// represents various attributes of the entity
	unsigned int EntityFlag;

	char bvType;

	bool bNoClip;

	bool bInSolid;

	SPlane touch_plane;

	short sState;

	float fRadius;

	/// axis-aligned bounding box of this entity (local space)
	AABB3 local_aabb;
	
	/// axis-aligned bounding box of this entity (world space)
	AABB3 world_aabb;

	/// position and orientation in world space
	Matrix34 WorldPose;

	Vector3 vPrevPosition;

	Vector3 vVelocity;
	Vector3 vAngularVelocity;

	/// speed (used only when necessary and not updated for every entity)
	float fSpeed;

	/// store external forces - currently not in use
	Vector3 vForce;
	Vector3 vTorque;

	TCFixedPointerVector<CCopyEntity, NUM_MAX_ENTITY_TOUCHES_PER_FRAME> vecpTouchedEntity;

	float fLife;

	CCopyEntity *apChild[NUM_MAX_CHILDREN_PER_ENTITY];
	int iNumChildren;
	CCopyEntity *pParent;

	CCopyEntity* pTarget;

	/// general purpose variables
	/// Be careful since each base entity use these variables differently from others
	float f1, f2, f3, f4, f5;
	short s1;
	Vector3 v1, v2, v3;

	int iExtraDataIndex;

	/// used to differentiate targets (e.g. target of homing missile)
	short GroupIndex;

	/// holds indices to lights
	/// directional lights have to be placed before point lights
	TCFixedVector<short,NUM_MAX_LIGHTS_AT_ENTITY> vecLightIndex;

	CJL_PhysicsActor *pPhysicsActor;

	/// borrowed reference
	std::vector<CCopyEntityCallbackBase *> vecpCallback;

	CMeshObjectHandle MeshObjectHandle;

public:

	void *pUserData;

// ================================= methods =================================

public:

	inline CCopyEntity();

	inline bool IsInUse() const { return inuse; }

	CBaseEntity *GetBaseEntity() { return pBaseEntity; }

	/// returns the name given to each copy entity.
	/// copy entity is given a name when it needs individual identification
	/// such a copy entity is refered to as 'named entity'
	/// temporary entities such as bullets/explosions/blasts usually have no names
	const std::string& GetName() const { return strName; }

	const U32 GetID() const { return m_ID; }

	int GetEntityTypeID() const { return m_TypeID; }

	int GetStockID() const { return m_StockID; }

	void SetName( const std::string& name ) { strName = name; }

	void SetStockID( int id ) { m_StockID = id; }

	/// updates entity - called once every frame
	inline void Act() { pBaseEntity->Act(this); }

	inline void Unlink();	//Do not call this from 'CEntityNode'

	inline void Terminate();

	inline void ClipTrace(STrace& rLocalTrace) { pBaseEntity->ClipTrace( rLocalTrace, this ); }

	inline void Touch(CCopyEntity* pCopyEnt_Other)
	{	//'this' copy entity touched 'pCopyEnt_Other' copy entity
//		if( pCopyEnt_Other && !pCopyEnt_Other->inuse )
//			return;	//touched entity is already dead
		pBaseEntity->Touch( this, pCopyEnt_Other );
		return;
	}

	inline void AddTouchedEntity( CCopyEntity* pCopyEnt );

	inline Vector3& Position() { return WorldPose.vPosition; }
	inline Vector3& Velocity() { return vVelocity; }
	inline Vector3& AngularVelocity() { return vAngularVelocity; }
	inline float GetSpeed() const { return fSpeed; }

	inline void GetOrientation( D3DXMATRIX& rmatOut ) const;

	inline void GetOrientation( Matrix33& matDestOrient ) const { matDestOrient = WorldPose.matOrient; }

	inline Vector3 GetRightDirection()	const { return WorldPose.matOrient.GetColumn(0); }	/// x-axis in this entity's local coord
	inline Vector3 GetUpDirection()		const { return WorldPose.matOrient.GetColumn(1); }	/// y-axis in this entity's local coord
	inline Vector3 GetDirection()		const { return WorldPose.matOrient.GetColumn(2); }	/// z-axis in this entity's local coord

	inline const Matrix34& GetWorldPose() const { return WorldPose; }
	inline void GetWorldPose( Matrix34& rDestWorldPose ) const { rDestWorldPose = WorldPose; }

	inline Vector3& PrevPosition() { return vPrevPosition; }


	inline void SetPosition(const Vector3 &v) { WorldPose.vPosition = v; }
	inline void SetVelocity(const Vector3 &v) { vVelocity = v; }
	inline void SetAngularVelocity(const Vector3 &v) { vAngularVelocity = v; }

	void SetDirection_Right( const Vector3& v ) { WorldPose.matOrient.SetColumn(0,v); }	/// x-axis in this entity's local coord
	void SetDirection_Up( const Vector3& v )    { WorldPose.matOrient.SetColumn(1,v); }	/// y-axis in this entity's local coord
	void SetDirection( const Vector3& v )       { WorldPose.matOrient.SetColumn(2,v); }	/// z-axis in this entity's local coord

	inline void SetOrientation( const Matrix33& matOrient ) { WorldPose.matOrient = matOrient; }

	inline void SetWorldPose( const Matrix34& rSrcWorldPose ) { WorldPose = rSrcWorldPose; }


	inline void ClearForces() { vForce = Vector3(0,0,0); vTorque = Vector3(0,0,0); }

	inline void GetPointVelocity(Vector3& rvPointVelocity, Vector3& rvPoint);
	void ApplyWorldImpulse( Vector3& vImpulse, Vector3& vContactPoint );
	inline void AddWorldForce( Vector3& vF ) { vForce += vF; }

	void ReleasePhysicsActor();
	void UpdatePhysics();

	/// virtual functions for derived class of CCopyEntity
	/// - By default, member functions of base entity do the job instead of these functions.
	/// - However, if a type of your entities have many individual properties and should become a class of its own,
	///   - Create a derived class of CCopyEntity
	///   - Implement the following virtual functions
	///     - HandleMessage( SGameMessage& msg )
	///     - Update( float dt ) or UpdatePhysics( float dt )
	///   - At runtime, create the entity as a copy entity of CBE_AIEntity

	/// update entity
	/// default: do nothing
	virtual void Update( float dt ) {}

	/// update entity in physics engine update cycle
	/// \param dt fixed delta time used synchronously with physics engine
	/// defualt: do nothing
	/// DON'T GET CONFUSED WITH CCopyEntity::UpdatePhysics()
	virtual void UpdatePhysics( float dt ) {}

	/// handles a game message
	/// defualt: do nothing
	virtual void HandleMessage( SGameMessage& msg ) {}

	/// render the entity
	/// default: let the base entity render the entity
	virtual void Draw() { pBaseEntity->Draw( this ); }

	/// called when the entity is terminated
	/// - default: do nothing
	/// - called from CCopyEntity::Terminate()
	virtual void TerminateDerived() {}

	// Lighting
	inline bool Lighting() { return ( (EntityFlag & BETYPE_LIGHTING) != 0 ); }
	inline int GetNumLights() const { return vecLightIndex.size(); }
	inline short GetLightIndex(int i) const { return vecLightIndex[i]; }
	inline void AddLightIndex(short sLightIndex);
	inline void InsertLightIndex(int pos, short sLightIndex);
	inline void ClearLightIndices() { vecLightIndex.resize(0); }

	// Parent/Children
	inline CCopyEntity *GetParent();	// return pointer to a parent if there is one
	inline CCopyEntity *GetChild( int i );
	inline int GetNumChildren() const { return iNumChildren; }
	inline int AddChild( CCopyEntity *pChild );
	void DisconnectFromParentAndChildren();

	inline void AddCallback( CCopyEntityCallbackBase* pCallback ) { vecpCallback.push_back(pCallback); }

	friend class CBaseEntity;
	friend class CEntitySet;
	friend class CEntityNode;
	friend class CEntityRenderManager;
};


#include "CopyEntity.inl"


#endif	/*  __COPYENTITY_H__  */
