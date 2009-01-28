#ifndef  __WEAPON_GRAVITYGUN_H__
#define  __WEAPON_GRAVITYGUN_H__

#include "Stage/fwd.h"
#include "Stage/EntityHandle.h"
#include "GI_Weapon.h"


class CGI_GravityGun : public CGI_Weapon
{

	CEntityHandle<> m_Target;	/// item the gravity gun is currently holding

	int m_iHoldingTargetToggle;

	float m_fMaxRange;
	float m_fGraspRange;

	float m_fPower;

	float m_fPosGain;
	float m_fSpeedGain;

private:

    bool GraspObjectInAimDirection();

	/// release the object currently grasped by the gravity gun
	void ReleaseObject();

public:

	inline CGI_GravityGun();

	~CGI_GravityGun() {}

//	void Update( CWeaponSystem& rWeaponSystem );
//	void ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem );

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	unsigned int GetArchiveObjectID() const { return ID_GRAVITY_GUN; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class CItemDatabaseBuilder;
};

//----------------------------- inline implementations -----------------------------


inline CGI_GravityGun::CGI_GravityGun()
{
	m_iHoldingTargetToggle = 0;

	m_fPosGain   = 5.5f;
	m_fSpeedGain = 2.0f;

	m_fMaxRange = 20.0f;
	m_fGraspRange = 3.0f;

	m_fPower = 40.0f;
}


inline void CGI_GravityGun::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Weapon::Serialize( ar, version );

	ar & m_fMaxRange;
	ar & m_fGraspRange;

	ar & m_fPower;

	ar & m_fPosGain;
	ar & m_fSpeedGain;

	if( ar.GetMode() == IArchive::MODE_INPUT )
	{
		m_Target = CEntityHandle<>();
		m_iHoldingTargetToggle = 0;
	}
}


#endif		/*  __WEAPON_GRAVITYGUN_H__  */
