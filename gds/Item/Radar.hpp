#ifndef  __Radar_H__
#define  __Radar_H__

//#include "3DMath/Matrix34.hpp"
#include "GameItem.hpp"
#include "Stage/EntityHandle.hpp"
#include "Stage/fwd.hpp"
#include "../base.hpp"


class CRadar;


class HUD_TargetInfo
{
public:
	enum param { MAX_TITLE_LENGTH = 20 };
	enum type_flag
	{
		TGT_AIR		= (1 << 0),
		TGT_SURFACE	= (1 << 1),
		PLAYER		= (1 << 2),
		ENEMY		= (1 << 3),
		ALLY		= (1 << 4),
		MISSILE		= (1 << 5),
		NEUTRAL		= (1 << 6),
		FOCUSED		= (1 << 7),
		LOCKED_ON	= (1 << 8),
		NOT_LOCKABLE= (1 << 9),
	};

	HUD_TargetInfo() {}

	HUD_TargetInfo( const Vector3& pos, char* _title, int _type )
		: position(pos), title(_title), type(_type), entity_id(0) {}

	Vector3 position;

	Vector3 direction;

	float radius;

	char *title;

	int type;

	U32 entity_id;
};


class CRadarInfo
{
	std::vector<HUD_TargetInfo> m_vecTargetInfo;

	std::vector<int> m_vecVisibleTargetIndex;

//	std::vector<int> m_vecLocalTargetIndex;

	int m_FocusedTargetIndex;

public:

	CRadarInfo() : m_FocusedTargetIndex(-1) {}
	~CRadarInfo() {}

	const std::vector<HUD_TargetInfo>& GetAllTargetInfo() const { return m_vecTargetInfo; }

	std::vector<HUD_TargetInfo>& TargetInfo() { return m_vecTargetInfo; }

//	size_t GetNumLocalTargets() const { return m_vecLocalTargetIndex.size(); }

//	const HUD_TargetInfo& GetLocalTarget( int index ) const { return m_vecTargetInfo[ m_vecLocalTargetIndex[index] ]; }

	size_t GetNumVisibleTargets() const { return m_vecVisibleTargetIndex.size(); }

	const HUD_TargetInfo& GetVisibleTarget( int index ) const { return m_vecTargetInfo[ m_vecVisibleTargetIndex[index] ]; }

	inline void ClearTargetInfo();

	const HUD_TargetInfo *GetFocusedTarget() const { return 0 <= m_FocusedTargetIndex ? &m_vecTargetInfo[m_FocusedTargetIndex] : NULL; }

	/// Returns true on success
	inline bool SetFocusedTargetIndex( int index );

//	void Update( float frametime );

	friend class CBE_PlayerPseudoAircraft;
	friend class CRadar;
	friend class HUD_PlayerAircraft;
};


inline void CRadarInfo::ClearTargetInfo()
{
	m_vecTargetInfo.resize(0);
	m_vecVisibleTargetIndex.resize(0);
//	m_vecLocalTargetIndex.resize(0);

	m_FocusedTargetIndex = -1;
}


inline bool CRadarInfo::SetFocusedTargetIndex( int index )
{
	if( index < 0 || (int)m_vecTargetInfo.size() <= index )
		return false;

	m_vecTargetInfo[index].type |= HUD_TargetInfo::FOCUSED;
	m_FocusedTargetIndex = index;

	return true;
}



class CRadar : public CGameItem
{
protected:

	/// range of the radar
	float m_fEffectiveRangeRadius;

	/// Stores entities picked up by the radar
	std::vector< CEntityHandle<> > m_vecEntityBuffer;

	/// temporary buffer to hold raw pointer of entities
	std::vector<CCopyEntity *> m_vecpEntityBuffer;

//	CEntityHandle<> m_FocusedTarget;

	int m_SensoringFrequency;

	int m_TargetInfoUpdateFrequency;

	double m_fNextSensoringTime;

	double m_fNextTargetUpdateTime;

	CRadarInfo m_RadarInfo;

	Matrix34 m_RadarWorldPose;

	//----------------- used when m_LauncherType == TYPE_LOAD_AND_RELEASE

//	float m_fTargetSensoringInterval;

//	float m_fFrameTimeAccumulation;

protected:

	void UpdateEntitiesList();

	void UpdateTargetInfo();

public:

	CRadar();

	virtual ~CRadar() {}

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam ) { return false; }

	virtual unsigned int GetArchiveObjectID() const { return ID_RADAR; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	/// Sets the focused target
	/// - The focused target is stored with the FOCUSED flag when it is converted to target info
//	void SetFocusedTarget( CEntityHandle<> focused_target ) { m_FocusedTarget = focused_target; }

	CRadarInfo& RadarInfo() { return m_RadarInfo; }

	void SetEffectiveRangeRadius( float radius ) { m_fEffectiveRangeRadius = radius; }

	std::vector< CEntityHandle<> >& EntityBuffer() { return m_vecEntityBuffer; }

	std::vector<CCopyEntity *>& EntityRawPtrBuffer() { return m_vecpEntityBuffer; }

	const Matrix34& GetRadarWorldPose() const { return m_RadarWorldPose; }

	void SetRadarWorldPose( const Matrix34& pose ) { m_RadarWorldPose = pose; }
};



#endif  __Radar_H__
