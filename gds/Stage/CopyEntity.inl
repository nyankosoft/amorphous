//
//  CopyEntity.inl
//  inline functions of CCopyEntity
//


inline CCopyEntity::CCopyEntity()
{
	m_ID = 0; // 0 == invalid id

	m_TypeID = 0; // TODO: set CCopyEntityTypeID::DEFAULT

	inuse = false;

	bInSolid = false;
	bNoClip = false;

	m_pNext = NULL;

	m_EntityLink.pOwner = this;

	m_pNextEntityInZSortTable = NULL;

	fZinCameraSpace = 0;
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

	pTarget = NULL;

	MeshObjectHandle.Release();

	// initialize light information
/*	sNumLights = 0;
	int i;
	for( i=0; i<NUM_MAX_LIGHTS_AT_ENTITY; i++ )
		asLightIndex[i] = CE_INVALID_LIGHT_INDEX;
*/
	// initialize parent/children information
	pParent = NULL;

	iNumChildren = 0;
	for( int i=0; i<NUM_MAX_CHILDREN_PER_ENTITY; i++ )
		apChild[i] = NULL;

	pPhysicsActor = NULL;

	pUserData = NULL;
}


inline void CCopyEntity::Unlink()	// Do not call this from 'CEntityNode'
{
	m_EntityLink.Unlink();
}


inline void CCopyEntity::Terminate()	// unlink from entity tree node and set 'inuse' to false
{
	size_t i, num_callbacks = vecpCallback.size();
	for( i=0; i<num_callbacks; i++ )
		vecpCallback[i]->OnCopyEntityDestroyed( this );
	vecpCallback.resize( 0 );

	// termination routine for derived entity
	// - does nothing by default
	TerminateDerived();

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

	pTarget = NULL;

	EntityFlag = 0;
	EntityFlag |= BETYPE_FLOATING;

	iExtraDataIndex = CE_INVALID_EXTRA_DATA_INDEX;

	GroupIndex = 0;//CE_GROUP_GENERAL;

	sState = 0;

	Unlink();

	if( 0 < iNumChildren || pParent )
		DisconnectFromParentAndChildren();

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

inline void CCopyEntity::AddLightIndex( short sLightIndex )
{
	if( GetNumLights() < NUM_MAX_LIGHTS_AT_ENTITY )
		vecLightIndex.push_back( sLightIndex );
}


inline void CCopyEntity::InsertLightIndex( int pos, short sLightIndex )
{
	if( GetNumLights() < NUM_MAX_LIGHTS_AT_ENTITY )
		vecLightIndex.insert_at( sLightIndex );
}


//========================================================
// parent/child control
//========================================================

inline CCopyEntity *CCopyEntity::GetChild( int i )
{
	if( apChild[i] && apChild[i]->inuse )
		return apChild[i];
	else
		return NULL;
}

inline CCopyEntity *CCopyEntity::GetParent()
{
	if( pParent && pParent->inuse )
		return pParent;
	else
		return NULL;
}


// add a child
// return an index to the newly added child
// the index will become invalid if you delete other children
// return -1 if no more children can be added
inline int CCopyEntity::AddChild( CCopyEntity *pChild )
{
	if( iNumChildren == NUM_MAX_CHILDREN_PER_ENTITY )
		return -1;

	apChild[iNumChildren++] = pChild;
	return (iNumChildren - 1);
}


inline void CCopyEntity::UpdateMesh()
{
	if( EntityFlag & BETYPE_SUPPORT_TRANSPARENT_PARTS )
		pBaseEntity->CreateAlphaEntities( this );
}


// global function for copy entity

inline bool IsValidEntity( CCopyEntity *pEntity )
{
	return ( pEntity && pEntity->IsInUse() );
}
