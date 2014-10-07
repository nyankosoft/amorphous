#ifndef  __Radar_H__
#define  __Radar_H__


#include "GameItem.hpp"


namespace amorphous
{


class Radar;


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


class RadarInfo
{
	std::vector<HUD_TargetInfo> m_vecTargetInfo;

//	std::vector<int> m_vecLocalTargetIndex;

	int m_FocusedTargetIndex;

public:

	RadarInfo() : m_FocusedTargetIndex(-1) {}
	~RadarInfo() {}

	const std::vector<HUD_TargetInfo>& GetAllTargetInfo() const { return m_vecTargetInfo; }

	std::vector<HUD_TargetInfo>& TargetInfo() { return m_vecTargetInfo; }

	const std::vector<HUD_TargetInfo>& GetTargetInfo() const { return m_vecTargetInfo; }

//	size_t GetNumLocalTargets() const { return m_vecLocalTargetIndex.size(); }

//	const HUD_TargetInfo& GetLocalTarget( int index ) const { return m_vecTargetInfo[ m_vecLocalTargetIndex[index] ]; }

	inline void ClearTargetInfo();

	const HUD_TargetInfo *GetFocusedTarget() const { return 0 <= m_FocusedTargetIndex ? &m_vecTargetInfo[m_FocusedTargetIndex] : NULL; }

	/// Returns true on success
	inline bool SetFocusedTargetIndex( int index );

//	void Update( float frametime );

	friend class CBE_PlayerPseudoAircraft;
	friend class Radar;
	friend class HUD_PlayerAircraft;
};


inline void RadarInfo::ClearTargetInfo()
{
	m_vecTargetInfo.resize(0);
//	m_vecLocalTargetIndex.resize(0);

	m_FocusedTargetIndex = -1;
}


inline bool RadarInfo::SetFocusedTargetIndex( int index )
{
	if( index < 0 || (int)m_vecTargetInfo.size() <= index )
		return false;

	m_vecTargetInfo[index].type |= HUD_TargetInfo::FOCUSED;
	m_FocusedTargetIndex = index;

	return true;
}



class Radar : public GameItem
{
protected:

	/// range of the radar
	float m_fEffectiveRangeRadius;

	/// Stores entities picked up by the radar
	std::vector< EntityHandle<> > m_vecEntityBuffer;

	/// temporary buffer to hold raw pointer of entities
	std::vector<CCopyEntity *> m_vecpEntityBuffer;

//	EntityHandle<> m_FocusedTarget;

	double m_fSensoringFrequency;

	int m_TargetInfoUpdateFrequency;

	double m_fNextSensoringTime;

	double m_fNextTargetUpdateTime;

	RadarInfo m_RadarInfo;

	Matrix34 m_RadarWorldPose;

	//----------------- used when m_LauncherType == TYPE_LOAD_AND_RELEASE

//	float m_fTargetSensoringInterval;

//	float m_fFrameTimeAccumulation;

	enum Params
	{
		MAX_ALLOWED_TARGET_GROUP_ID = 64,
	};

	/// m_vecIsTargetEntityGroup[group_id] == 1: register the entity to the target list
	/// m_vecIsTargetEntityGroup[group_id] == 0: do not register the entity to the target list
//	std::vector<int> m_vecIsTargetEntityGroup;

	/// stores if entity group ids(indices) that should be considered target
	/// m_vecIsTargetEntityGroup[group_id] = 1:
	std::map<int,int> m_mapTargetEntityGroup;

protected:

	void UpdateEntitiesList();

	void UpdateTargetInfo();

public:

	Radar();

	virtual ~Radar() {}

	virtual void Update( float dt );

	virtual bool HandleInput( int input_code, int input_type, float fParam ) { return false; }

	virtual unsigned int GetArchiveObjectID() const { return ID_RADAR; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( XMLNode& reader );

	/// Sets the focused target
	/// - The focused target is stored with the FOCUSED flag when it is converted to target info
//	void SetFocusedTarget( EntityHandle<> focused_target ) { m_FocusedTarget = focused_target; }

	RadarInfo& RadarInfo() { return m_RadarInfo; }

	void SetEffectiveRangeRadius( float radius ) { m_fEffectiveRangeRadius = radius; }

	std::vector< EntityHandle<> >& EntityBuffer() { return m_vecEntityBuffer; }

	std::vector<CCopyEntity *>& EntityRawPtrBuffer() { return m_vecpEntityBuffer; }

	const Matrix34& GetRadarWorldPose() const { return m_RadarWorldPose; }

	void SetRadarWorldPose( const Matrix34& pose ) { m_RadarWorldPose = pose; }

//	void AddTargetEntityGroup( int group_id );
//	void ClearTargetEntityGroup
};


} // namespace amorphous



#endif  __Radar_H__
