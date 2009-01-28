#ifndef __COPYENTITY_H__
#define __COPYENTITY_H__


#include "../base.h"
#include "3DMath/Matrix34.h"
#include "3DMath/3DStructs.h"
#include "Support/FixedVector.h"
#include "Graphics/MeshObjectHandle.h"
#include "Physics/fwd.h"

#include "EntityHandle.h"
#include "CopyEntityCallback.h"
#include "LinkNode.h"

#include "fwd.h"
#include "BaseEntity.h"



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
	BETYPE_PLAYER                      = (1 << 0), ///< The entity is the single player object in the stage
	BETYPE_TERRESTRIAL                 = (1 << 1),
	BETYPE_FLOATING                    = (1 << 2),
	BETYPE_ENEMY                       = (1 << 3),
	BETYPE_ITEM                        = (1 << 4),
	BETYPE_RIGIDBODY                   = (1 << 5),
	BETYPE_INDESTRUCTIBLE              = (1 << 6),
	BETYPE_LIGHTING                    = (1 << 7),  ///< Apply lighting to entity (rendering)
	BETYPE_NOCLIP                      = (1 << 8),
	BETYPE_NOCLIP_WITH_STATIC_GEOMETRY = (1 << 9),
	BETYPE_GLARESOURCE                 = (1 << 10), ///< Deprecated
	BETYPE_GLAREHINDER                 = (1 << 11), ///< Deprecated
	BETYPE_USE_ZSORT                   = (1 << 12), ///< Use z-sort for tranparent entities (rendering)
	BETYPE_SUPPORT_TRANSPARENT_PARTS   = (1 << 13), ///< Create separate entities to z-sort transparent parts of an entity (rendering)
	BETYPE_VISIBLE                     = (1 << 14), ///< The entity is rendered by Draw() method of its base entity (rendering)
	BETYPE_USE_PHYSSIM_RESULTS         = (1 << 15), ///< The entity pose & velocity is determined by physics simulator (physics)
	BETYPE_ENVMAPTARGET                = (1 << 16),
	BETYPE_SHADOW_CASTER               = (1 << 17), ///< If the entity casts shadows to other entities
	BETYPE_SHADOW_RECEIVER             = (1 << 18), ///< If the entity receives shadows cast by other entities  others
	BETYPE_COPY_PARENT_POSE            = (1 << 19), ///< If the entity has a parent entity, it automatically copies the pose of the parent
};

/*
/// glare types - not used any more
/// - deprecated
#define CEGT_NOGLARE		0
#define CEGT_GLARESOURCE	1
#define CEGT_GLAREHINDER	2
*/

#define CE_INVALID_LIGHT_INDEX	-1
#define CE_INVALID_EXTRA_DATA_INDEX	-1


/**
 Stores extrinsic properties of entity
  -intrinsic properties are stored in base entity
*/
class CCopyEntity
{
public:

	enum FixedParams
	{
		NUM_MAX_LIGHTS_AT_ENTITY         = 4,
		NUM_MAX_CHILDREN_PER_ENTITY      = 4,
		NUM_MAX_ENTITY_TOUCHES_PER_FRAME = 4,
	};

private:

	std::string strName;	///< individual name for entity

	U32 m_ID; ///< unique id for entity (0 == invalid id)

	int m_TypeID;

	/// true if currenly in use
	bool inuse;

	int m_StockIndex;
	int m_StockID;

	CStage *m_pStage;

	boost::weak_ptr<CCopyEntity> m_pSelf;

	/// next entity in the chain list 'm_pEntityInUse'
	boost::shared_ptr<CCopyEntity> m_pNext;

	CCopyEntity* m_pNextRawPtr;

	CLinkNode<CCopyEntity> m_EntityLink; ///< next & prev entity in the chain linked to entity tree-node 'CEntityNode'

	/// next entity in the link list for z-sorting (used to render transparent entities)
	CCopyEntity* m_pNextEntityInZSortTable;

	/// used for z-sort of translucent entities (rendering)
	float fZinCameraSpace;

	/// the time at which the entity is created in the stage [sec]
	double m_CreatedTime;

private:

	void SetNext( boost::shared_ptr<CCopyEntity> pNext )
	{
		m_pNext       = pNext;
		m_pNextRawPtr = pNext.get();
	}

	void SetNextToNull()
	{
		m_pNext       = boost::shared_ptr<CCopyEntity>();
		m_pNextRawPtr = NULL;
	}

protected:

	CStage *GetStage() { return m_pStage; }

public:

	CBaseEntity* pBaseEntity;

	boost::weak_ptr<CCopyEntity>& Self() { return m_pSelf; }

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

	CEntityHandle<> m_aChild[NUM_MAX_CHILDREN_PER_ENTITY];
	int iNumChildren;

	CCopyEntity *m_pParent;

	CEntityHandle<> m_Target;

	/// general purpose variables
	/// Be careful since each base entity use these variables differently from others
	float f1, f2, f3, f4, f5;
	short s1;
	Vector3 v1, v2, v3;

	int iExtraDataIndex;

	/// Used to differentiate targets (e.g. target of homing missile)
	/// - Default: 0
	short GroupIndex;

	/// holds indices to lights
	/// directional lights have to be placed before point lights
	TCFixedVector<CEntityHandle<CLightEntity>,NUM_MAX_LIGHTS_AT_ENTITY> m_vecLight;

	physics::CActor *pPhysicsActor;

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
	/// copy entity is given a name when it needs individual identification.
	/// Such a copy entity is refered to as a 'named entity'.
	/// Temporary entities such as bullets/explosions/blasts usually have no names
	const std::string& GetName() const { return strName; }

	const U32 GetID() const { return m_ID; }

	int GetEntityTypeID() const { return m_TypeID; }

	int GetStockIndex() const { return m_StockIndex; }
	int GetStockID() const { return m_StockID; }

	void SetName( const std::string& name ) { strName = name; }

	void SetStockIndex( int index ) { m_StockIndex = index; }
	void SetStockID( int id ) { m_StockID = id; }

	double GetCreatedTime() const { return m_CreatedTime; }

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

	inline float GetRadius() const { return fRadius; }


	inline void ClearForces() { vForce = Vector3(0,0,0); vTorque = Vector3(0,0,0); }

	inline void GetPointVelocity(Vector3& rvPointVelocity, Vector3& rvPoint);
	void ApplyWorldImpulse( Vector3& vImpulse, Vector3& vContactPoint );
	inline void AddWorldForce( Vector3& vF ) { vForce += vF; }

	void ReleasePhysicsActor();
	void UpdatePhysics();

	/// Called when the entity is created.
	/// - Called after CBaseEntity::InitCopyEntity().
	virtual void Init( CCopyEntityDesc& desc ) {}

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

	virtual void RenderStage() {}

	/// called when the entity is used as a camera entity
	virtual void CreateRenderTasks() {}

	// Lighting
	inline bool Lighting() { return ( (EntityFlag & BETYPE_LIGHTING) != 0 ); }
	inline int GetNumLights() const { return m_vecLight.size(); }
	inline CEntityHandle<CLightEntity>& GetLight(int i) { return m_vecLight[i]; }
	inline void AddLight( CEntityHandle<CLightEntity>& light_entity );
	inline void InsertLight( int pos, CEntityHandle<CLightEntity>& light_entity );
	inline void ClearLights() { m_vecLight.resize(0); }

	// Parent/Children
	inline CCopyEntity *GetParent();	// return pointer to a parent if there is one
	inline CCopyEntity *GetChild( int i );
	inline int GetNumChildren() const { return iNumChildren; }
//	inline int AddChild( CCopyEntity *pChild );
	inline int AddChild( boost::weak_ptr<CCopyEntity> pChild );
	void DisconnectFromParentAndChildren();
	inline void CopyParentPose();

	inline void AddCallback( CCopyEntityCallbackBase* pCallback ) { vecpCallback.push_back(pCallback); }

	/// Release and create alpha entities if BETYPE_SUPPORT_TRANSPARENT_PARTS is on.
	inline void UpdateMesh();

	friend class CBaseEntity;
	friend class CEntityHandleBase;
	friend class CEntitySet;
	friend class CEntityNode;
	friend class CEntityRenderManager;
};


#include "CopyEntity.inl"


#endif	/*  __COPYENTITY_H__  */
