//
//  CopyEntity.inl
//  inline functions of CCopyEntity
//


inline CCopyEntity::CCopyEntity()
{
	m_ID = 0; // 0 == invalid id

	m_TypeID = 0; // TODO: set CCopyEntityTypeID::DEFAULT

	inuse = false;

//	m_pStage = NULL;

	bInSolid = false;
	bNoClip = false;

	m_pNextRawPtr = NULL;

	m_EntityLink.pOwner = this;

	m_pNextEntityInZSortTable = NULL;

	fZinCameraSpace = 0;

	m_CreatedTime = 0.0;

	EntityFlag = 0;
	EntityFlag |= BETYPE_FLOATING;

	pBaseEntity = NULL;

	const Vector3 vZero = Vector3(0,0,0);

	touch_plane.dist = 0;
	touch_plane.normal = vZero;
	sState = 0;
	fRadius = 0;
	local_aabb.SetMaxAndMin( vZero, vZero );
	world_aabb.SetMaxAndMin( vZero, vZero );

	WorldPose.Identity();

	vPrevPosition = vZero;
//	vDirection = vZero;
	vVelocity = vZero;
	fSpeed = 0;

	vForce = vZero;
	vTorque = vZero;

	fLife = 0;

	s1 = 0;
	f1 = 0;
	f2 = 0;
	f3 = 0;
	f4 = 0;
	f5 = 0;
	v1 = vZero;
	v2 = vZero;
	v3 = vZero;

	GroupIndex = 0;//CE_GROUP_GENERAL;

	iExtraDataIndex = CE_INVALID_EXTRA_DATA_INDEX;

//	pTarget = NULL;

	MeshObjectHandle.Release();

	// initialize light information
/*	sNumLights = 0;
	int i;
	for( i=0; i<NUM_MAX_LIGHTS_AT_ENTITY; i++ )
		asLightIndex[i] = CE_INVALID_LIGHT_INDEX;
*/
	// initialize parent/children information
	m_pParent = NULL;

	iNumChildren = 0;
//	for( int i=0; i<NUM_MAX_CHILDREN_PER_ENTITY; i++ )
//		apChild[i] = NULL;

	pPhysicsActor = NULL;

	pUserData = NULL;
}


/// Do not call this from 'CEntityNode'
inline void CCopyEntity::Unlink()
{
	m_EntityLink.Unlink();
}


/// - Unlink the entity from entity tree node
/// - Set 'inuse' to false
inline void CCopyEntity::Terminate()
{
	size_t i, num_callbacks = vecpCallback.size();
	for( i=0; i<num_callbacks; i++ )
		vecpCallback[i]->OnCopyEntityDestroyed( this );
	vecpCallback.resize( 0 );

	// termination routine for derived entity
	// - does nothing by default
	TerminateDerived();

	if( 0 < iNumChildren || m_pParent )
		DisconnectFromParentAndChildren();

	// mark this as not in use
	this->inuse = false;

	m_ID = 0;

	// clear general variables
	s1 = 0;
	f1 = f2 = f3 = f4 = f5 = 0;
	v1 = v2 = v3 = Vector3(0,0,0);

	vForce = vTorque = Vector3(0,0,0);
	vVelocity = vAngularVelocity = Vector3(0,0,0);

	WorldPose.matOrient.SetIdentity();

	vecpTouchedEntity.clear();

	m_Target.Reset();

	EntityFlag = 0;
	EntityFlag |= BETYPE_FLOATING;

	iExtraDataIndex = CE_INVALID_EXTRA_DATA_INDEX;

	GroupIndex = 0;//CE_GROUP_GENERAL;

	sState = 0;

	Unlink();

	if( pPhysicsActor )
		ReleasePhysicsActor();

	pUserData = NULL;
}


inline void CCopyEntity::AddTouchedEntity( CCopyEntity* pCopyEnt )
{
	if( vecpTouchedEntity.size() == NUM_MAX_ENTITY_TOUCHES_PER_FRAME )
		return;

	for(int i=0; i<vecpTouchedEntity.size(); i++)
	{
		if( vecpTouchedEntity[i] == pCopyEnt )
			return;	// already made a note of this entity
	}	

	vecpTouchedEntity.push_back( pCopyEnt );
}


inline void CCopyEntity::GetOrientation( D3DXMATRIX& rmatOut ) const
{
	WorldPose.GetRowMajorMatrix44( (float *)&rmatOut );
	rmatOut._41 = 0;
	rmatOut._42 = 0;
	rmatOut._43 = 0;
	rmatOut._44 = 1;
}

/*
inline void CCopyEntity::GetWorldTransform(D3DXMATRIX& rmatWorld )
{
	rmatWorld = matRot;
	rmatWorld._41 = vPosition.x; rmatWorld._42 = vPosition.y; rmatWorld._43 = vPosition.z; rmatWorld._44 = 1;
}*/


inline void CCopyEntity::GetPointVelocity(Vector3& rvPointVelocity, Vector3& rvPoint)
{
	Vector3 vOut = rvPoint - WorldPose.vPosition;
	Vec3Cross( rvPointVelocity, vAngularVelocity, vOut );
	rvPointVelocity += vVelocity;
}



//========================================================
// light control
//========================================================

inline void CCopyEntity::AddLight( CEntityHandle<CLightEntity>& light_entity )
{
	if( GetNumLights() < NUM_MAX_LIGHTS_AT_ENTITY )
		m_vecLight.push_back( light_entity );
}


inline void CCopyEntity::InsertLight( int pos, CEntityHandle<CLightEntity>& light_entity )
{
	if( GetNumLights() < NUM_MAX_LIGHTS_AT_ENTITY )
		m_vecLight.insert_at( pos, light_entity );
}


//========================================================
// parent/child control
//========================================================

inline CCopyEntity *CCopyEntity::GetChild( int i )
{
	if( i < 0 || iNumChildren <= i )
		return NULL;

	return m_aChild[i].GetRawPtr();
/*
	if( m_aChild[i] && m_aChild[i]->inuse )
		return m_aChild[i];
	else
		return NULL;*/
}

inline CCopyEntity *CCopyEntity::GetParent()
{
	if( m_pParent && m_pParent->inuse )
		return m_pParent;
	else
		return NULL;
}


/// Add a child
/// - Return an index to the newly added child.
/// - The index will become invalid if you delete other children.
/// - Return -1 if no more children can be added.
inline int CCopyEntity::AddChild( boost::weak_ptr<CCopyEntity> pChild )
{
	if( iNumChildren == NUM_MAX_CHILDREN_PER_ENTITY )
		return -1;

	m_aChild[iNumChildren++] = CEntityHandle<>( pChild );
	return (iNumChildren - 1);
}


inline void CCopyEntity::CopyParentPose()
{
	if( m_pParent )
		SetWorldPose( m_pParent->GetWorldPose() );
}


inline void CCopyEntity::UpdateMesh()
{
	if( EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS )
		pBaseEntity->CreateAlphaEntities( this );
}



//======================================================================
// CEntityHandleBase
//======================================================================

inline bool CEntityHandleBase::IsEntityInUse( CCopyEntity *pEntity )
{
	return pEntity->inuse;
}



//======================================================================
// CEntityHandle
//======================================================================

/// Needs to be defined after the definitions of CCopyEntity
/// since these guys use the methods of CCopyEntity

template<class T>
inline CEntityHandle<T>::CEntityHandle( boost::weak_ptr<T> pEntity )
:
m_pEntity(pEntity),
m_StockID(-1)
{
	boost::shared_ptr<T> pEntitySharedPtr = m_pEntity.lock();
	if( pEntitySharedPtr )
		m_StockID = pEntitySharedPtr->GetStockID();
}


template<class T>
inline CEntityHandle<T>::CEntityHandle( boost::shared_ptr<T> pEntity )
:
m_pEntity(pEntity),
m_StockID(pEntity->GetStockID())
{}


template<class T>
inline boost::shared_ptr<T> CEntityHandle<T>::Get()
{
	boost::shared_ptr<T> pEntity = m_pEntity.lock();
	if( !pEntity )
	{
		// Lock failed, which means,
		// - For non-pooled entity,
		//   - The entity has been released or the stage has been released.
		// - For pooled entity,
		//   - The stage has been released.
		return boost::shared_ptr<T>();
	}
	else if( IsEntityInUse(pEntity.get()) == false )
	{
		// Already terminated by Terminate().
		// - Think of this state as 'marked as invalid'
		// - The entity may still be in the link list of the active entities,
		//   but it must not take any action. This entity is soon to be released.
		return boost::shared_ptr<T>();
	}
	else
	{
		// pEntity && pEntity->inuse

		if( m_StockID == -2 )
		{
			// non-pooled entity
			return pEntity;
		}
		else
		{
			// pooled entity
			// - Never deleted unless the stage is released.

//			if( m_StockID == -3 )
//			{
//				// Not initialized yet
//				m_StockID = pEntity->GetStockID();
//			}

			if( 0 <= m_StockID && pEntity->GetStockID() == m_StockID )
				return pEntity;
			else
			{
				// The pooled instance is currently used as another entity
				return boost::shared_ptr<T>();
			}
		}
	}
}



//======================================================================
// global function(s) for copy entity
//======================================================================

inline bool IsValidEntity( CCopyEntity *pEntity )
{
	return ( pEntity && pEntity->IsInUse() );
}
