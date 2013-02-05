#ifndef  __BE_PLAYERPSEUDOLEGGEDVEHICLE_H__
#define  __BE_PLAYERPSEUDOLEGGEDVEHICLE_H__

#include "BE_Player.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"
#include "GameMessage.hpp"
#include "Graphics/Camera.hpp"
#include "fwd.hpp"


namespace amorphous
{


#define PLAYERPLV_ROTATION_YAW		0
#define PLAYERPLV_ROTATION_PITCH	1


class CVehicleLeg
{
	Vector3 m_vLocalPos;
	Vector3 m_vLocalDir;

	float m_fLength;
	
	float m_fSpring;
	float m_fDamper;

	bool m_OnGround;

	CStage *m_pStage;

public:

	CVehicleLeg() {}

	CVehicleLeg( Vector3 vLocalPos, Vector3 vLocalDir, float fLength );

	void Update( CCopyEntity *pEntity, float dt );

	inline bool IsOnGround() { return m_OnGround; }

	void SetStage( CStage *pStage) { m_pStage = pStage; }

	friend class CBE_PlayerPseudoLeggedVehicle;
};


class CBE_PlayerPseudoLeggedVehicle : public CBE_Player
{
	enum eParam
	{
		NUM_VEHICLE_LEGS = 4,
	};


	float m_afThrust[6];
	float m_afRotationSpeedGain[2];

	Vector3 m_vJumpThrust;
	float m_fJumpThrustTime;

	CVehicleLeg m_aVehicleLeg[NUM_VEHICLE_LEGS];

private:

	CInputHandler_PlayerBase *CreatePlayerInputHandler();

public:

	CBE_PlayerPseudoLeggedVehicle();

	~CBE_PlayerPseudoLeggedVehicle();

	void Init();
//	void InitCopyEntity(CCopyEntity* pCopyEnt);

//	void Act(CCopyEntity* pCopyEnt);
//	void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
//	void Draw(CCopyEntity *pCopyEnt);

	void Move( CCopyEntity *pCopyEnt );

	void UpdateCamera(CCopyEntity* pCopyEnt);

	virtual void UpdatePhysics( CCopyEntity *pCopyEnt, float dt );


	bool HandleInput( SPlayerEntityAction& input );

//	void PlayerDead(CCopyEntity* pCopyEnt);

//	void AdaptToNewScreenSize() {}
//	void LoadGraphicsResources( const GraphicsParameters& rParam );
//	void ReleaseGraphicsResources();


	inline void SetThrust( int iThrustDirection, float fValue );
	inline void SetRotationSpeedGain( int iRotDirection, float fSpeedGain );
	inline void AddRotationSpeedGain( int iRotDirection, float fSpeedGain );
	inline void SetBoost( float fValue );	// fValue = 0 means normal speed

	/// returns true if all the legs are on the ground
	inline bool IsOnGround();

	/// add thrust for jump if the vehicle is on the ground
	inline void AddJumpThrust( Vector3& vThrust );

	/// prevents the vehicle from turning upside down
	void LimitOrientation( CCopyEntity *pCopyEnt );

//	void LaserAimingDevice( bool bLaserDot );
//	void ToggleHeadLight();

	virtual unsigned int GetArchiveObjectID() const { return BE_PLAYERPSEUDOLEGGEDVEHICLE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CVehicleLeg;
};


// ================================ inline implementations ================================ 

inline void CBE_PlayerPseudoLeggedVehicle::SetRotationSpeedGain( int iRotDirection, float fSpeedGain )
{
	m_afRotationSpeedGain[iRotDirection] = fSpeedGain;
}

inline void CBE_PlayerPseudoLeggedVehicle::AddRotationSpeedGain( int iRotDirection, float fSpeedGain )
{
	m_afRotationSpeedGain[iRotDirection] += fSpeedGain;
}

inline void CBE_PlayerPseudoLeggedVehicle::SetThrust( int iThrustDirection, float fValue )
{
	m_afThrust[iThrustDirection] = fValue;
}

inline void CBE_PlayerPseudoLeggedVehicle::SetBoost( float fValue )
{
	// m_fBoost == 0.0f : normal speed
	// m_fBoost == 1.0f : double speed
	m_fBoost = fValue * 0.65f;
}


inline bool CBE_PlayerPseudoLeggedVehicle::IsOnGround()
{
	for( int i=0; i<NUM_VEHICLE_LEGS; i++ )
	{
		if( !m_aVehicleLeg[i].IsOnGround() )
			return false;
	}

	return true;
}


inline void CBE_PlayerPseudoLeggedVehicle::AddJumpThrust( Vector3& vThrust )
{
	if( IsOnGround() )
	{
		m_vJumpThrust += vThrust;
		m_fJumpThrustTime = 0;
	}
}


} // namespace amorphous



#endif		/*  __BE_PLAYERPSEUDOLEGGEDVEHICLE_H__  */
