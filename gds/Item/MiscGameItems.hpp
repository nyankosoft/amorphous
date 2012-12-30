#ifndef __Item_MiscGameItems_HPP__
#define __Item_MiscGameItems_HPP__


#include "GameItem.hpp"


namespace amorphous
{


//===========================================================================
// CGI_Battery
//===========================================================================

class CGI_Battery
{
public:
	bool m_bEnabled;

	float m_fBatteryLeft;
	float m_fMaxBatteryLife;
	float m_fChargeSpeed;

public:

	CGI_Battery() : m_bEnabled(false), m_fBatteryLeft(0), m_fMaxBatteryLife(1), m_fChargeSpeed(1) {}

	inline void Update( float dt );
	inline bool IsBatteryFull() const { return (m_fBatteryLeft == m_fMaxBatteryLife); }
};


inline void CGI_Battery::Update( float dt )
{
	if( m_bEnabled )
	{
		m_fBatteryLeft -= dt;
		if( m_fBatteryLeft <= 0 )
		{
			m_fBatteryLeft = 0;
			m_bEnabled = false;
		}
	}
	else
	{
		if( m_fBatteryLeft < m_fMaxBatteryLife )
		{
			m_fBatteryLeft += dt * m_fChargeSpeed;
		}

		if( m_fMaxBatteryLife <= m_fBatteryLeft )
		{
			m_fBatteryLeft = m_fMaxBatteryLife;
			return;
		}
	}
}


//===========================================================================
// CGI_Binocular
//===========================================================================

class CGI_Binocular : public CGameItem
{
	float m_fCurrentZoom;
	float m_fZoomSpeed;
	float m_fTargetZoom;

	float m_fMaxZoomSpeed;
	float m_fMaxZoom;

	float m_fFocusDelay;

public:

	CGI_Binocular();

	unsigned int GetArchiveObjectID() const { return ID_BINOCULAR; }

	virtual void OnSelected();

	virtual bool HandleInput( int input_code, int input_type, float fParam );

	virtual void Update( float dt );

	/// render item status on HUD
	virtual void GetStatus( std::string& dest_buffer );

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_Binocular::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fMaxZoom;

//	ar & m_fCurrentZoom;
//	ar & m_fTargetZoom;
}



//===========================================================================
// CGI_NightVision
//===========================================================================

class CGI_NightVision : public CGameItem 
{
	/// indicates if the object is registered as an active item 
	bool m_bActive;

	bool m_bEnabled;

	float m_fBatteryLeft;
	float m_fMaxBatteryLife;
	float m_fChargeSpeed;

public:

	CGI_NightVision() { m_TypeFlag = TYPE_UTILITY; m_bActive = false; m_bEnabled = false; m_fMaxBatteryLife = 10.0f; m_fBatteryLeft = m_fMaxBatteryLife; m_fChargeSpeed = 1.6f; }

	unsigned int GetArchiveObjectID() const { return ID_NIGHT_VISION; }

	virtual void OnSelected();

	virtual void Update( float dt );

	/// render item status on HUD
	virtual void GetStatus( std::string& dest_buffer );

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_NightVision::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fMaxBatteryLife;
	ar & m_fChargeSpeed;
	ar & m_fBatteryLeft;
}


//===========================================================================
// CGI_CamouflageDevice
//===========================================================================

class CGI_CamouflageDevice : public CGameItem
{
	bool m_bEnabled;

	float m_fEffectiveTimeLeft;
	float m_fMaxEffectiveTime;

public:

	CGI_CamouflageDevice() { m_TypeFlag = TYPE_UTILITY; m_bEnabled = false; m_fMaxEffectiveTime = 10.0f; m_fEffectiveTimeLeft = m_fMaxEffectiveTime; }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_CAMFLOUGE_DEVICE; }

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;
};


inline void CGI_CamouflageDevice::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_bEnabled;

	ar & m_fMaxEffectiveTime;
	ar & m_fEffectiveTimeLeft;
}



//===========================================================================
// CGI_Suppressor
//===========================================================================

class CGI_Suppressor : public CGameItem
{
	bool m_bAttached;

	float m_fSuppressorPerformance;

public:

	CGI_Suppressor() { m_bAttached = false; }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_SUPPRESSOR; }

	inline void Serialize( IArchive& ar, const unsigned int version );
};


inline void CGI_Suppressor::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	ar & m_fSuppressorPerformance;
}



//===========================================================================
// CGI_Key
//===========================================================================

class CGI_Key : public CGameItem
{
	enum eKeyParam
	{
		KEY_CODE_LENGTH = 12,
	};

	char m_acKeyCode[KEY_CODE_LENGTH];

public:

	CGI_Key() { memset( m_acKeyCode, 0, sizeof(char) * KEY_CODE_LENGTH ); }

	void OnSelected();

	unsigned int GetArchiveObjectID() const { return ID_KEY; }

	inline void Serialize( IArchive& ar, const unsigned int version );
};


inline void CGI_Key::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	for( int i=0; i<KEY_CODE_LENGTH; i++ )
        ar & m_acKeyCode[i];
}


} // namespace amorphous



#endif /* __Item_MiscGameItems_HPP__ */
