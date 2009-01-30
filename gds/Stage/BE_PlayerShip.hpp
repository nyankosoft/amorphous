#ifndef  __BE_PLAYERSHIP_H__
#define  __BE_PLAYERSHIP_H__

#include "BE_Player.hpp"
#include "BaseEntityHandle.hpp"
#include "CopyEntity.hpp"
#include "GameMessage.hpp"
#include "Graphics/fwd.hpp"
#include "Graphics/Camera.hpp"
#include "fwd.hpp"

class CShockWaveCameraEffect;

/*
class ThrustDirection
{
public:
	enum Name
	{
		Forward = 0,
		Backward,
		Right,
		Left,
		Up,
		Down
	};
}
*/

#define PLAYERSHIP_ROTATION_YAW		0
#define PLAYERSHIP_ROTATION_PITCH	1


class CBE_PlayerShip : public CBE_Player
{
	float m_afThrust[6];
	float m_afRotationSpeedGain[2];

//	CBaseEntityHandle m_LaserDot;
//	CBaseEntityHandle m_HeadLight;

//	CCopyEntity *m_pLaserDotEntity;
//	CCopyEntity *m_pHeadLightEntity;
//	bool m_bHeadLightOn;

private:

	CInputHandler_PlayerBase *CreatePlayerInputHandler();

public:

	CBE_PlayerShip();
	~CBE_PlayerShip();

	void Init();
	void InitCopyEntity(CCopyEntity* pCopyEnt);

//	void Act(CCopyEntity* pCopyEnt);
//	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
//	void Draw(CCopyEntity *pCopyEnt);

	void Move( CCopyEntity *pCopyEnt );

	virtual bool HandleInput( SPlayerEntityAction& input );

	void MoveEx( CCopyEntity *pEntity );	// rigid body simulation is applied when MoveEx() is used

//	void PlayerDead(CCopyEntity* pCopyEnt);

	inline void SetThrust( int iThrustDirection, float fValue );
	inline void SetRotationSpeedGain( int iRotDirection, float fSpeedGain );
	inline void AddRotationSpeedGain( int iRotDirection, float fSpeedGain );
	inline void SetBoost( float fValue );	// fValue = 0 means normal speed

    virtual void Serialize( IArchive& ar, const unsigned int version );

//	void LaserAimingDevice( bool bLaserDot );
//	void ToggleHeadLight();
};


// ================================ inline implementations ================================ 

inline void CBE_PlayerShip::SetRotationSpeedGain( int iRotDirection, float fSpeedGain )
{
	m_afRotationSpeedGain[iRotDirection] = fSpeedGain;
}

inline void CBE_PlayerShip::AddRotationSpeedGain( int iRotDirection, float fSpeedGain )
{
	m_afRotationSpeedGain[iRotDirection] += fSpeedGain;
}

inline void CBE_PlayerShip::SetThrust( int iThrustDirection, float fValue )
{
	m_afThrust[iThrustDirection] = fValue;
}

inline void CBE_PlayerShip::SetBoost( float fValue )
{
	// m_fBoost == 0.0f : normal speed
	// m_fBoost == 1.0f : double speed
	m_fBoost = fValue * 0.65f;
}


#endif		/*  __BE_PLAYERSHIP_H__  */
