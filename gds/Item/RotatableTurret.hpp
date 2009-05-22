#ifndef  __RotatableTurret_H__
#define  __RotatableTurret_H__


#include "GameItem.hpp"
#include "3DMath/Quaternion.hpp"
#include "3DMath/Matrix34.hpp"
#include "GameCommon/CriticalDamping.hpp"
#include "../base.hpp"


class CGI_Weapon;
class CGI_Ammunition;

/*
class CMagazine : public CGameItem
{
	std::vector< boost::shared_ptr<CGI_Ammunition> > m_vecpAmmunition;
};
*/


class CRotatableTurret : public CGameItem
{
	// heading rotation (1 DOF)
	// - Used as a turn table
	Matrix34 m_MountLocalPose;

	// pitch rotation (1 DOF)
	// - Mainly used to hold elevation from the mount (translation in the positive direction of y-axis).
	// - The gun does not do heading rotations. Mount does.
	Matrix34 m_GunLocalPose;

	Matrix34 m_ParentWorldPose;

//	Matrix33 m_matTurnTableRotation;

	cdv<Quaternion> m_LocalTurnTableOrient;
//	cdv<Quaternion> m_LocalGunTubeOrient;
	cdv<float> m_LocalGunTubePitchAngle;

	boost::shared_ptr<CGI_Weapon> m_pWeapon;

//	boost::shared_ptr<CGI_Ammunition> m_pAmmunition;
	std::vector< boost::shared_ptr<CGI_Ammunition> > m_vecpAmmunition;

	CEntityHandle<> m_Target;

	Vector3 m_vAimDirection;

/*	enum StyleFlags
	{
		SF_USE_ENTITY_AS_PARENT_OBJECT = (1 << 0), ///< Use obtain parent world pose from the entity
	}
*/

protected:

	void UpdateAimInfo();

public:

	void Update( float dt );

	unsigned int GetArchiveObjectID() const { return ID_ROTATABLE_TURRET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	void SetTarget( CEntityHandle<> target ) { m_Target = target; }

	void SetParentWorldPose( const Matrix34& pose ) { m_ParentWorldPose = pose; }
};



#endif  __RotatableTurret_H__
