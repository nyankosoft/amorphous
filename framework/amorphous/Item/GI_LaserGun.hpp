#ifndef  __WEAPON_GI_LaserGun_H__
#define  __WEAPON_GI_LaserGun_H__

#include "Stage/fwd.hpp"
#include "Stage/EntityHandle.hpp"
#include "GI_Weapon.hpp"


namespace amorphous
{


class LaserGun : public CGI_Weapon
{
	float m_fMaxRange;
	float m_fRadius;

	float m_fPower; ///< damage points per second

	EntityHandle<> m_LaserEntity;

public:

	LaserGun();

	~LaserGun();

//	void Update( CWeaponSystem& rWeaponSystem );
//	void ImmediateTriggerAction( int iTrigger, CWeaponSystem& rWeaponSystem );

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	unsigned int GetArchiveObjectID() const { return ID_GRAVITY_GUN; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( XMLNode& reader );

	friend class CItemDatabaseBuilder;
};

} // namespace amorphous



#endif		/*  __WEAPON_GI_LaserGun_H__  */
