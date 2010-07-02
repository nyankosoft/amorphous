#include "BE_PhysicsBaseEntity.hpp"
#include "EntityCollisionGroups.hpp"
#include "CopyEntity.hpp"
#include "Physics/Enums.hpp"
#include "Support/SafeDeleteVector.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"

using namespace std;
using namespace physics;


CBE_PhysicsBaseEntity::CBE_PhysicsBaseEntity()
{
	this->m_EntityFlag |= ( BETYPE_RIGIDBODY | BETYPE_USE_PHYSSIM_RESULTS );
/*
	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
*/

	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;

}


CBE_PhysicsBaseEntity::~CBE_PhysicsBaseEntity()
{
	SafeDeleteVector( m_ActorDesc.vecpShapeDesc );
}


/**
 * base entites derived from CPhysicsBaseEntity must call this functiont explicitly
 * in their own LoadSpecificPropertiesFromFile()
 * i.e. put CPhysicsBaseEntity::LoadSpecificPropertiesFromFile(pcLine) at the start of
 * the function
*/
bool CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	Vector3 vBoxSideLength, vLocalPos;
	float fAngle;
	float fRadius, fLength;
	bool enable_physics_sim;
	string coll_group;

	if( scanner.TryScanBool( "PHYSICS", "TRUE/FALSE", enable_physics_sim ) )
	{
		if( enable_physics_sim )	RaiseEntityFlag( BETYPE_RIGIDBODY );
		if( !enable_physics_sim )	ClearEntityFlag( BETYPE_RIGIDBODY );
	}

	if( scanner.TryScanLine( "PHYS_COLL_GROUP", coll_group ) )
	{
		if( coll_group == "STATICGEOMETRY" )	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_STATICGEOMETRY;
		else if( coll_group == "PLAYER" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_PLAYER;
		else if( coll_group == "DOOR" )			m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_DOOR;
		else if( coll_group == "OTHERS" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
		else if( coll_group == "NOCLIP" )		m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_NOCLIP;
		else if( coll_group == "NO_COLLISION" )	m_ActorDesc.CollisionGroup = ENTITY_COLL_GROUP_NOCLIP;
	}

	if( scanner.TryScanLine( "MASS", m_ActorDesc.BodyDesc.fMass ) ) return true;

	if( scanner.TryScanLine( "SHAPE_BOX", vBoxSideLength ) )
	{
		// parse the full lengths of the 3 edges of the box
		CBoxShapeDesc *pBoxDesc = new CBoxShapeDesc;
		pBoxDesc->vSideLength = vBoxSideLength * 2.0f;
		m_ActorDesc.vecpShapeDesc.push_back( pBoxDesc );
		return true;
	}

	if( scanner.TryScanLine( "SHAPE_CAPSULE", fRadius, fLength ) )
	{
		CCapsuleShapeDesc *pCapsuleDesc = new CCapsuleShapeDesc;
		pCapsuleDesc->fLength = fLength;
		pCapsuleDesc->fRadius = fRadius;
		m_ActorDesc.vecpShapeDesc.push_back( pCapsuleDesc );
		return true;
	}

	if( scanner.TryScanLine( "SHAPE_LOCAL_POSITION", vLocalPos ) )
	{
		if( m_ActorDesc.vecpShapeDesc.size() == 0 )
			return false;

		m_ActorDesc.vecpShapeDesc.back()->LocalPose.vPosition = vLocalPos;
		return true;
	}

	if( scanner.GetTagString().find( "SHAPE_LOCAL_ROT" ) == 0 )
	{
		// the tag starts with "SHAPE_LOCAL_ROT"
		string str;
		scanner.ScanLine( str, fAngle );

		// convert from degree to radisan
		fAngle = fAngle / 360.0f * 2.0f * 3.141592f;

		if( m_ActorDesc.vecpShapeDesc.size() == 0 )
			return false;

		const string& tag = scanner.GetTagString();
		Matrix33 matRot;
		if( tag == "SHAPE_LOCAL_ROT_X" )		matRot.SetRotationX(fAngle);
		else if( tag == "SHAPE_LOCAL_ROT_Y" )	matRot.SetRotationY(fAngle);
		else if( tag == "SHAPE_LOCAL_ROT_Z" )	matRot.SetRotationZ(fAngle);
		else return false;

		Matrix33& rmatLocalOrient = m_ActorDesc.vecpShapeDesc.back()->LocalPose.matOrient;
		rmatLocalOrient = matRot * rmatLocalOrient;	// the rotation is applied following ealier rotations

		return true;
	}

	if( scanner.GetTagString() == "DISABLE_FREEZING" )
	{
//		m_ActorDesc.bAllowFreezing = false;
		m_ActorDesc.ActorFlags |= ActorFlag::DISABLE_FREEZING;
		return true;
	}

//	if( scanner.TryScanLine( "S_FRICTION", m_fStaticFriction ) ) return true;
//	if( scanner.TryScanLine( "D_FRICTION", m_fDynamicFriction ) ) return true;

	return false;
}


void CBE_PhysicsBaseEntity::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_ActorDesc;

//	ar & m_fStaticFriction & m_fDynamicFriction;
}
