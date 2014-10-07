#ifndef  __RotatableTurret_H__
#define  __RotatableTurret_H__


#include "GameItem.hpp"
#include "amorphous/3DMath/Quaternion.hpp"
#include "amorphous/3DMath/CriticalDamping.hpp"


namespace amorphous
{


class CGI_Weapon;
class CGI_Ammunition;

/*
class Magazine : public GameItem
{
	std::vector< boost::shared_ptr<CGI_Ammunition> > m_vecpAmmunition;
};
*/


class CRotatableTurret : public GameItem
{
	// heading rotation (1 DOF)
	// - Used as a turn table
	Matrix34 m_MountLocalPose;

	// pitch rotation (1 DOF)
	// - Mainly used to hold elevation from the mount (translation in the positive direction of y-axis).
	// - The gun does not do heading rotations. Mount does.
	Matrix34 m_GunLocalPose;

	Matrix34 m_ParentWorldPose;
	Matrix34 m_MountWorldPose;
	Matrix34 m_GunWorldPose;

//	Matrix33 m_matTurnTableRotation;

	cdv<Quaternion> m_LocalTurnTableOrient;
//	cdv<Quaternion> m_LocalGunTubeOrient;
	cdv<float> m_LocalGunTubePitchAngle;

	std::string m_WeaponName;
	boost::shared_ptr<CGI_Weapon> m_pWeapon;

//	boost::shared_ptr<CGI_Ammunition> m_pAmmunition;
	class CAmmunitionAttributes : public IArchiveObjectBase
	{
	public:
		std::string m_AmmunitionName;
		int m_InitQuantity;
		boost::shared_ptr<CGI_Ammunition> pItem;

		CAmmunitionAttributes()
			:
		m_InitQuantity(0)
		{}

		void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & m_AmmunitionName;
			ar & m_InitQuantity;

			if( ar.GetMode() == IArchive::MODE_INPUT )
				pItem.reset();
		}
	};

	std::vector<CAmmunitionAttributes> m_vecAmmunition;

	std::vector< boost::shared_ptr<CGI_Ammunition> > m_vecpAmmunition;

	EntityHandle<> m_Target;

	Vector3 m_vAimDirection;

	// used when the meshes of mount/gun are not placed at the origin
	// but are in the local space of the owner item mesh.
	// - Used to ease the modeling work.
	Matrix34 m_MeshTransform;
	Matrix34 m_MountMeshTransform;
	Matrix34 m_GunMeshTransform;

	boost::shared_ptr<GameItem> m_pOwner;

/*	enum StyleFlags
	{
		SF_USE_ENTITY_AS_PARENT_OBJECT = (1 << 0), ///< Use obtain parent world pose from the entity
	}
*/

protected:

	void UpdateAimInfo();

public:

	CRotatableTurret();

	virtual ~CRotatableTurret() {}

	bool LoadMeshObject();

	Result::Name OnLoadedFromDatabase();

	void Update( float dt );

	void Render();

	unsigned int GetArchiveObjectID() const { return ID_ROTATABLE_TURRET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( XMLNode& reader );

	void SetTarget( EntityHandle<> target ) { m_Target = target; }

	void SetParentWorldPose( const Matrix34& pose ) { m_ParentWorldPose = pose; }

	void SetMeshTransform( Matrix34& transform );
//	void SetGunMeshTransform( Matrix34& transform ) { m_GunMeshTransform = transform; }

	void SetOwner( boost::shared_ptr<GameItem> pOwner ) { m_pOwner = pOwner; }
};


} // namespace amorphous



#endif  __RotatableTurret_H__
