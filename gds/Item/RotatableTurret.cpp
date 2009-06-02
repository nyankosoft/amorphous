#include "RotatableTurret.hpp"
#include "3DMath/MathMisc.hpp"
#include "Item/GI_Weapon.hpp"
#include "Item/GI_Ammunition.hpp"
#include "Item/ItemDatabaseManager.hpp"
#include "Stage/CopyEntity.hpp"
#include "Stage/Stage.hpp"
#include "XML/XMLNodeReader.hpp"
#include "XML/LoadFromXMLNode_3DMath.hpp"
#include "Support/Serialization/Serialization_BoostSmartPtr.hpp"

using namespace std;
using namespace boost;


CRotatableTurret::CRotatableTurret()
:
m_MountLocalPose(Matrix34Identity()),
m_GunLocalPose(Matrix34Identity()),
m_MeshTransform(Matrix34Identity()),
m_GunMeshTransform(Matrix34Identity()),
m_MountMeshTransform(Matrix34Identity()),
m_vAimDirection(Vector3(0,0,1))
{
	m_LocalTurnTableOrient.smooth_time   = 1.0f;
	m_LocalGunTubePitchAngle.smooth_time = 1.0f;

	m_LocalTurnTableOrient.current.FromRotationMatrix( Matrix33Identity() );
	m_LocalTurnTableOrient.target.FromRotationMatrix( Matrix33Identity() );
	m_LocalTurnTableOrient.vel.FromRotationMatrix( Matrix33Identity() );
	m_LocalGunTubePitchAngle.SetZeroState();
}


// Calculate and update m_vAimDirection from the current target.
// Does nothing if the turret has not target.
void CRotatableTurret::UpdateAimInfo()
{
	shared_ptr<CCopyEntity> pLinkedEntity = m_Entity.Get();
	if( !pLinkedEntity )
	{
		// Does not linked to the entity - check the owner's entity
		if( m_pOwner )
			pLinkedEntity = m_pOwner->GetItemEntity().Get();
	}

	if( !pLinkedEntity )
		return;

	shared_ptr<CCopyEntity> pTargetEntity = m_Target.Get();
	if( !pTargetEntity )
		return;

	Matrix34 init_mount_world_pose = m_ParentWorldPose * m_MountLocalPose;

	Vector3 vTurretPosition = init_mount_world_pose.vPosition;
//	Vector3 vTurretPosition = pLinkedEntity->Position();

	// update target pos and dir
	Vector3 vTargetPos = pTargetEntity->GetWorldPose().vPosition;

	Vector3 vAimAtPos = vTargetPos;

	Vector3 vAimDirection = vAimAtPos - vTurretPosition;
	Vec3Normalize( m_vAimDirection, vAimDirection );
}


bool CRotatableTurret::LoadMeshObject()
{
	bool base = CGameItem::LoadMeshObject();
	bool weapon = false;
	if( m_pWeapon )
		weapon = m_pWeapon->LoadMeshObject();

	return (base && weapon);
}


Result::Name CRotatableTurret::OnLoadedFromDatabase()
{
	m_pWeapon = ItemDatabaseManager().GetItem<CGI_Weapon>( m_WeaponName, 1 );

	for( size_t i=0; i<m_vecAmmunition.size(); i++ )
	{
		CAmmunitionAttributes& ammo = m_vecAmmunition[i];
		ammo.pItem = ItemDatabaseManager().GetItem<CGI_Ammunition>( ammo.m_AmmunitionName, ammo.m_InitQuantity );
	}

	return Result::SUCCESS;
}


void CRotatableTurret::Update( float dt )
{
	UpdateAimInfo();

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

	// update critical damping
	m_LocalGunTubePitchAngle.Update( dt );
	m_LocalTurnTableOrient.Update( dt );

//	vLocalTTableRightDir = m_LocalTurnTableOrient.current.ToRotationMatrix().GetColumn(0);

	// update local poses
	Matrix34 local_mount_pose, local_gun_tube_pose;
	local_mount_pose    = Matrix34( m_MountLocalPose.vPosition, m_LocalTurnTableOrient.current.ToRotationMatrix() );
//	local_gun_tube_pose = Matrix34( m_GunLocalPose.vPosition,   Matrix33RotationAxis( m_LocalGunTubePitchAngle.current, vLocalTTableRightDir ) );
	local_gun_tube_pose = Matrix34( m_GunLocalPose.vPosition,   Matrix33RotationAxis( m_LocalGunTubePitchAngle.current, m_GunLocalPose.matOrient.GetColumn(0) ) );

	m_MountWorldPose = m_ParentWorldPose * local_mount_pose;
	m_GunWorldPose   = m_MountWorldPose * local_gun_tube_pose;
}


void CRotatableTurret::Render()
{
	// local & parent transforms are already combined - apply only the mesh transform to the mesh container
	if( 0 < m_MeshContainerRootNode.GetNumMeshContainers() )
	{
		m_MeshContainerRootNode.MeshContainer(0)->m_MeshTransform = m_MountMeshTransform;
		m_MeshContainerRootNode.Render( m_MountWorldPose );
	}

	if( m_pWeapon
	 && 0 < m_pWeapon->MeshContainerRootNode().GetNumMeshContainers() )
	{
		m_pWeapon->MeshContainerRootNode().MeshContainer(0)->m_MeshTransform = m_GunMeshTransform;
		m_pWeapon->MeshContainerRootNode().Render( m_GunWorldPose );
	}
}


void CRotatableTurret::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_MountLocalPose;
	ar & m_GunLocalPose;
	ar & m_WeaponName;
	ar & m_vecAmmunition;


	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		// load ammo?
	}
}


void CRotatableTurret::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGameItem::LoadFromXMLNode( reader );

	string ammo_name, ammo_quantity;

	reader.GetChildElementTextContent( "Weapon",      m_WeaponName );

	vector<CXMLNodeReader> vecAmmo = reader.GetImmediateChildren( "Ammunition" );
//	m_vecpAmmunition.resize( vecAmmo.size() );
	m_vecAmmunition.resize( vecAmmo.size() );
	for( size_t i=0; i<vecAmmo.size(); i++ )
	{
		// name
		m_vecAmmunition[i].m_AmmunitionName = vecAmmo[i].GetTextContent();

		// quantity
		ammo_quantity = vecAmmo[i].GetAttributeText( "quantity" );
		if( ammo_quantity == "" )
			ammo_quantity = "infinite";
		if( ammo_quantity == "infinite" )
		{
			// TODO: set infinite
			ammo_quantity = "100000000";
		}
		m_vecAmmunition[i].m_InitQuantity = to_int(ammo_quantity);

//		m_vecpAmmunition[i] = ItemDatabaseManager().GetItem<CGI_Ammunition>( ammo_name, to_int(ammo_quantity) );
	}
/*
	vector<CXMLNodeReader> vecAmmoLoading = reader.GetChildElementTextContent( "AmmunitionLoading/Name",  ammo_name );
	const size_t num_loadings = vecAmmoLoading.size();
	m_vecpAmmunitionLoading.reserve( 8 );
	for( size_t i=0; i<num_loadings; i++ )
	{
		ammo_name = vecAmmoLoading[i].GetTextContent();
		num_loads = vecAmmoLoading[i].GetAttributeText();
		shared_ptr<CGI_Ammunition> pAmmunition = get_ammo( ammo_name );
		for( int j=0; j<num_loads; j++ )
			m_vecpAmmunitionLoading.push_back( pAmmunition );
	}
*/
	::LoadFromXMLNode( reader.GetChild( "Mount/LocalPose" ), m_MountLocalPose );
	::LoadFromXMLNode( reader.GetChild( "Gun/LocalPose" ),   m_GunLocalPose );

	m_MountMeshTransform = m_MeshTransform * m_MountLocalPose.GetInverseROT();
	m_GunMeshTransform   = m_MeshTransform * m_GunLocalPose.GetInverseROT();
}


void CRotatableTurret::SetMeshTransform( Matrix34& transform )
{
	m_MeshTransform = transform;
	m_MountMeshTransform = m_MeshTransform * m_MountLocalPose.GetInverseROT();
	m_GunMeshTransform   = m_MeshTransform * m_GunLocalPose.GetInverseROT();
}
