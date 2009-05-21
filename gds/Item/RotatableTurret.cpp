#include "RotatableTurret.hpp"
#include "3DMath/MathMisc.hpp"
#include "Item/GI_Weapon.hpp"
#include "Item/GI_Ammunition.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/Stage.hpp"
#include "XML/XMLNodeReader.hpp"
#include "XML/LoadFromXMLNode_3DMath.hpp"

using namespace std;
using namespace boost;


void CRotatableTurret::UpdateAimInfo()
{
	shared_ptr<CCopyEntity> pMyEntity = m_Entity.Get();
	if( !pMyEntity )
		return;

	shared_ptr<CCopyEntity> pTargetEntity = m_Target.Get();
	if( !pTargetEntity )
		return;

	Matrix34 init_mount_world_pose = m_ParentWorldPose * m_MountLocalPose;

	Vector3 vTurretPosition = init_mount_world_pose.vPosition;
//	Vector3 vTurretPosition = pMyEntity->Position();

	// update target pos and dir
	Vector3 vTargetPos = pTargetEntity->GetWorldPose().vPosition;

	Vector3 vAimAtPos = vTargetPos;

	Vector3 vAimDirection = vAimAtPos - vTurretPosition;
	Vec3Normalize( m_vAimDirection, vAimDirection );
}


void CRotatableTurret::Update( float dt )
{
	Matrix34 init_world_pose
		= m_ParentWorldPose * m_MountLocalPose;

	// Why calculate on local space?
	// - world pose of the turret keeps changes if the owner entity keeps moving.

//	Vector3 vTargetDirection = m_vAimDirection;
	Vector3 vLocalTTableAxis = m_MountLocalPose.matOrient.GetColumn(1); // y-axis

	Vector3 vLocalTargetDir;
	init_world_pose.InvTransform( vLocalTargetDir, m_vAimDirection );

	Vector3 vLocalTgtDirOnTTable
		= vLocalTargetDir - vLocalTTableAxis * Vec3Dot( vLocalTTableAxis, vLocalTargetDir );

	Vec3Normalize( vLocalTgtDirOnTTable, vLocalTgtDirOnTTable );

	Matrix33 matLocalTargetRotation;
	matLocalTargetRotation.SetColumn( 0, Vec3Cross( vLocalTTableAxis, vLocalTgtDirOnTTable ) );
	matLocalTargetRotation.SetColumn( 1, vLocalTTableAxis );
	matLocalTargetRotation.SetColumn( 2, vLocalTgtDirOnTTable );
	matLocalTargetRotation.Orthonormalize();

	m_LocalTurnTableOrient.target.FromRotationMatrix( matLocalTargetRotation );

	// calc how much the gun tube needs to be elevated (pitch rotation)

	float target_pitch = Vec3GetAngleBetween( vLocalTargetDir, vLocalTgtDirOnTTable );

	Limit( target_pitch, 0.0f, deg_to_rad(85.0f) );

	m_LocalGunTubePitchAngle.target = target_pitch;

//	vLocalTTableRightDir = m_LocalTurnTableOrient.current.ToRotationMatrix().GetColumn(0);

	Matrix34 local_mount_pose, local_gun_tube_pose;
	local_mount_pose    = Matrix34( m_MountLocalPose.vPosition, m_LocalTurnTableOrient.current.ToRotationMatrix() );
//	local_gun_tube_pose = Matrix34( m_GunLocalPose.vPosition,   Matrix33RotationAxis( m_LocalGunTubePitchAngle.current, vLocalTTableRightDir ) );
	local_gun_tube_pose = Matrix34( m_GunLocalPose.vPosition,   Matrix33RotationAxis( m_LocalGunTubePitchAngle.current, m_GunLocalPose.matOrient.GetColumn(0) ) );

	Matrix34 world_mount_pose    = m_ParentWorldPose * local_mount_pose;
	Matrix34 world_gun_tube_pose = world_mount_pose * local_gun_tube_pose;

/*
	Matrix33 matLocalGTubeTgtRotation = CreateOrientFromFwdDir( vLocalTargetDir );

	m_LocalGunTubeOrient.target.FromRotationMatrix( matLocalGTubeTgtRotation );
*/
/*
	Vector3 vTurnTableAxis = init_world_pose.matOrient.GetColumn(1); // y-axis

	Vector3 vTargetDirection = GetCurrentTargetDirection();

	// project the target direction to the plane of the turn table
	Vector3 vTargetDirOnTTable
		= vTargetDirection - vTurnTableAxis * Vec3Dot( vTurnTableAxis, vTargetDirection );

	Vec3Normalize( vTargetDirOnTTable, vTargetDirOnTTable );

//	float angle_to_turn = acos( Vec3Dot( m_matTurnTableRotation.GetColumn(2), vTargetDirOnTTable ) );
	float target_angle  = acos( Vec3Dot( init_world_pose.matOrient.GetColumn(2), vTargetDirOnTTable ) );

	Vector3 vCross = Vec3Dot( m_matTurnTableRotation.GetColumn(2), vTargetDirOnTTable );
	float turn_direction = 1.0f;
	if( Vec3Dot( vTurnTableAxis, vCross ) < 0 )
		turn_direction = -1.0f; // angle to rotate to direct the turntable to the target direction

	delta_angle = deg_to_rad(turn_speed) * dt;
	Limit( delta_angle, 0, angle_to_turn );

	m_TurnTableAngle.target = 

	Matrix33 matTTableTgtRotation;
	matTTableTgtRotation.SetColumn( 0, Vec3Cross( vTurnTableAxis, vTargetDirOnTTable ) );
	matTTableTgtRotation.SetColumn( 1, vTurnTableAxis );
	matTTableTgtRotation.SetColumn( 2, vTargetDirOnTTable );
	m_TurnTableRotation.target.FromRotationMatrix( matTTableTgtRotation );
*/
}


void CRotatableTurret::Serialize( IArchive& ar, const unsigned int version )
{
}


void CRotatableTurret::LoadFromXMLNode( CXMLNodeReader& reader )
{
	string weapon_name, ammo_name, ammo_quantity;

	reader.GetChildElementTextContent( "Weapon",      weapon_name );
	m_pWeapon = ItemDatabaseManager().GetItem<CGI_Weapon>( weapon_name, 1 );

	reader.GetChildElementTextContent( "Ammunition",  ammo_name );
	ammo_quantity = reader.GetChild( "Ammunition" ).GetAttributeText( "quantity" );
	if( ammo_quantity == "" )
		ammo_quantity = "infinite";
	if( ammo_quantity == "infinite" )
	{
		// TODO: set infinite
		ammo_quantity = "100000000";
	}
	m_pAmmunition = ItemDatabaseManager().GetItem<CGI_Ammunition>( ammo_name, to_int(ammo_quantity) );

	::LoadFromXMLNode( reader.GetChild( "MountLocalPose" ), m_MountLocalPose );
	::LoadFromXMLNode( reader.GetChild( "GunLocalPose" ),   m_GunLocalPose );
}
