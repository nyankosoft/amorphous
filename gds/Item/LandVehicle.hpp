#ifndef __Item_LandVehicle_H__
#define __Item_LandVehicle_H__


#include <boost/shared_ptr.hpp>
#include "Item/GameItem.hpp"
#include "Stage/EntityMotionPathRequest.hpp"


// How to direct the turret

// calculate the current world pose of the turret
// from the parent transform.


class CRadar;
class CRotatableTurret;



namespace item
{


class CLandVehicle : public CGameItem
{
	// performance
	float m_fMaxAccel;
	float m_fMaxTurnSpeed;

	// states
	float m_fCurrentAccel;
	float m_fCurrentTurnSpeed;

	CBEC_MotionPath m_Path;

	Matrix34 m_PrevPose;

	// used for trace test
	std::vector<CCopyEntity *> m_vecpEntityBuffer;

	boost::shared_ptr<CGameItem> m_pOwner;

protected:

	inline boost::shared_ptr<CItemEntity> GetVehicleEntity();

public:

	CLandVehicle() {}

	virtual void Update( float dt );

	virtual void Render();

	unsigned int GetArchiveObjectID() const { return ID_LAND_VEHICLE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	void Release() {}

	void SetOwner( boost::shared_ptr<CGameItem> pOwner ) { m_pOwner = pOwner; }
};


class CGroundContactPoint
{
public:

	Vector3 vLocalPosition;

	float fCurrentY;
};


class CArmedVehicle : public CGameItem
{
	std::string m_RadarName;
	boost::shared_ptr<CRadar> m_pRadar;

//	std::vector< boost::shared_ptr<CWeapon> > m_vecpWeapon;

	std::string m_LandVehicleName;
	boost::shared_ptr<item::CLandVehicle> m_pLandVehicleItem;

	// current target
	CEntityHandle<> m_Target;

//	std::vector< boost::shared_ptr<CRotatableTurret> > m_vecpTurret;

	class CTurretHolder : public IArchiveObjectBase
	{
	public:
		std::string TurretName;
		boost::shared_ptr<CRotatableTurret> pTurret;
		Matrix34 LocalPose;
		bool UseInvLocalTransformForMeshTransform;

	public:
		CTurretHolder()
			: UseInvLocalTransformForMeshTransform(true) {}

		void Serialize( IArchive& ar, const unsigned int version );
	};

	std::vector<CTurretHolder> m_vecTurret;

protected:

	bool IsTargetGroupIndex( int group );

public:

	CArmedVehicle() {}
	~CArmedVehicle() {}

	void UpdateTarget();

	void Init();

	bool LoadMeshObject();

	void Update( float dt );

	void Render();

	Result::Name OnLoadedFromDatabase() { Init(); return Result::SUCCESS; }

	unsigned int GetArchiveObjectID() const { return ID_ARMED_VEHICLE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );
};


} // namespace item


#endif  /*  __Item_LandVehicle_H__  */
