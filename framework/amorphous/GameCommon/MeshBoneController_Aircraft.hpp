#ifndef  __MeshBoneController_Aircraft_H__
#define  __MeshBoneController_Aircraft_H__


#include "MeshBoneControllerBase.hpp"
#include "amorphous/3DMath/CriticalDamping.hpp"
#include "amorphous/GameCommon/RangedSet.hpp"


namespace amorphous
{


/**
 * init steps for MeshBoneController_Aircraft derived classes
 * 1. create an instance: pController = new MeshBoneController_Flap()
 * 2. set target bone names
 * 3. set control params
 * 4. set target mesh: pController->SetTargetMesh( pMesh )
 * 5. set pseudo aircraft simulator: pController->SetPseudoAircraftSimulator( pPseudoSim )
 * 6. call Init(): pController->Init()
 * 
 * steps 1-3. can be replaced with a serializaiton
 */


/*
// aircraft mesh controller descs

struct AMC_FlapControllerDesc
{
	std::string name;
	float AnglePerPitch;
	float RollPerPitch;
};


struct AMC_VFlapControllerDesc
{
	enum type
	{
		TYPE_SINGLE,
		TYPE_TWIN,
		NUM_TYPES
	};

	std::string name;
	float AnglePerYaw;
};


struct AMC_RotorDesc
{
	enum direction
	{
		DIR_CW,
		DIR_CCW,
		NUM_DIRS
	};

	std::string name;
	int RotationDirection;
};


class CAircraftMeshBoneControllerDesc
{
public:
	
	std::vector<AMC_FlapControllerDesc> Flap;
	std::vector<AMC_VFlapControllerDesc> VFlap;
	std::vector<AMC_RotorDesc> Rotor;

-------------------
	flap;
		vector<string> vecFlapName;
		angle_per_pitch;
		angle_per_roll;
	vflap;
		name;
		angle_per_yaw;
//	rotor
	std::vector<std::string> vecRotorName;
	vector<float> vecfRotationDirection;

//	gear;
//		name;
-------------------
};
*/


class CPseudoAircraftSimulator;
class MeshBoneController_AircraftBase;
class MeshBoneController_GearUnit;
class MeshBoneController_Cover;


class CConstraintComponent : public IArchiveObjectBase
{
public:

	/// name of the component that constrains the motions of owner component
	std::string Name;

	boost::shared_ptr<MeshBoneController_Cover> m_pComponent;

	/// range of angle in which the constrained component can move
	RangedSet<float> AllowedAngleRange;

public:

	void LoadFromXMLNode( XMLNode& reader );

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & Name;
		ar & AllowedAngleRange;

		if( ar.GetMode() == IArchive::MODE_INPUT )
			m_pComponent.reset();
	}
};


class MeshBoneController_AircraftBase : public MeshBoneControllerBase
{
protected:

	CPseudoAircraftSimulator *m_pSimulator;

	double m_CurrentTime;

//	boost::shared_ptr<MeshBoneController_AircraftBase> m_pParent;

public:

	MeshBoneController_AircraftBase()
		:
	MeshBoneControllerBase(boost::shared_ptr<SkeletalMesh>()),
	m_pSimulator(NULL)
	{}

	MeshBoneController_AircraftBase( boost::shared_ptr<SkeletalMesh> pTargetMesh,
		CPseudoAircraftSimulator *pSimulator,
		boost::shared_ptr<MeshBoneController_AircraftBase> pParent = boost::shared_ptr<MeshBoneController_AircraftBase>() )
		:
	MeshBoneControllerBase( pTargetMesh ),
	m_pSimulator( pSimulator )//,
//	m_pParent(pParent)
	{}

	virtual ~MeshBoneController_AircraftBase() {}

	virtual void Init( const SkeletalMesh& target_skeletal_mesh );

	void SetPseudoAircraftSimulator( CPseudoAircraftSimulator *pSimulator ) { m_pSimulator = pSimulator; }

//	void SetParent( boost::shared_ptr<MeshBoneController_AircraftBase> pParent ) { m_pParent = pParent; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneControllerBase::Serialize( ar, version );
	}

	virtual void Update( float dt ) {}

	void SetCurrentTime( double current_time ) { m_CurrentTime = current_time; }

	friend class CItemDatabaseBuilder;
};

/**
 * controls a pair of flaps
 * always holds a pair of right & left flaps. does not hold single right / left flap
 */
class MeshBoneController_Flap : public MeshBoneController_AircraftBase
{
	enum flap
	{
		FLAP_R,
		FLAP_L,
		NUM_FLAPS
	};

	float m_fAnglePerPitchAccel;
	float m_fAnglePerRollAccel;

public:

	MeshBoneController_Flap() : m_fAnglePerPitchAccel(0.4f), m_fAnglePerRollAccel(0.4f) {}

	MeshBoneController_Flap( boost::shared_ptr<SkeletalMesh> pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: MeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_fAnglePerPitchAccel(0.4f), m_fAnglePerRollAccel(0.4f) {}

	virtual void UpdateTransforms();

	virtual void Init( const SkeletalMesh& target_skeletal_mesh );

	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_FLAP; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneController_AircraftBase::Serialize( ar, version );
		ar & m_fAnglePerPitchAccel & m_fAnglePerRollAccel;
	}

	friend class CItemDatabaseBuilder;
};


class MeshBoneController_VFlap : public MeshBoneController_AircraftBase
{
	enum flap
	{
		VFLAP_R,
		VFLAP_L,
		NUM_VFLAPS
	};

	float m_fAnglePerYawAccel;

	int m_Type;

public:

	enum vflap_type
	{
		TYPE_SINGLE,
		TYPE_TWIN,
		NUM_VFLAP_TYPES
	};

	MeshBoneController_VFlap() : m_fAnglePerYawAccel(0.5f), m_Type(TYPE_SINGLE) {}

	MeshBoneController_VFlap( boost::shared_ptr<SkeletalMesh> pTargetMesh,
		                       CPseudoAircraftSimulator *pSimulator,
							   int type = TYPE_SINGLE )
		: MeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_Type(type) { m_fAnglePerYawAccel = 0.7f; }

	virtual void UpdateTransforms();

	virtual void Init( const SkeletalMesh& target_skeletal_mesh );

	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_VFLAP; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneController_AircraftBase::Serialize( ar, version );
		ar & m_fAnglePerYawAccel & m_Type;
	}

	friend class CItemDatabaseBuilder;
};


class MeshBoneController_Rotor : public MeshBoneController_AircraftBase
{
	float m_fRotationSpeed;
	float m_fAngle;
	float m_fAngleOffset;

	int m_RotationDirection;
    
	float m_fCurrentRotationAngle;

	enum dir_type
	{
		DIR_CW,		///< clockwise
		DIR_CCW,	///< counter-clockwise
		NUM_DIRS
	};

public:

	MeshBoneController_Rotor() : m_fRotationSpeed(5.0f), m_fAngle(0), m_fAngleOffset(0), m_RotationDirection(DIR_CW) {}

	MeshBoneController_Rotor( boost::shared_ptr<SkeletalMesh> pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: MeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_RotationDirection(DIR_CW) {}

	virtual void UpdateTransforms();

	virtual void Init( const SkeletalMesh& target_skeletal_mesh );

	void Update( float dt );

	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_ROTOR; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneController_AircraftBase::Serialize( ar, version );

		ar & m_fRotationSpeed & m_fAngle & m_fAngleOffset;
		ar & m_RotationDirection;
	}

	void SetRotationAngle( float angle ) { m_fCurrentRotationAngle = angle; }

	friend class CItemDatabaseBuilder;
};


class CAircraftComponentState
{
public:

	enum Name
	{
		OPEN,
		OPENING,
		CLOSED,
		CLOSING,
		NUM_STATES
	};
};



class MeshBoneController_Cover : public MeshBoneController_AircraftBase
{
public:

	float m_fOpenAngle;

	/// Used by shaft and cover components
	std::vector<CConstraintComponent> m_vecConstraint;

//	float m_fCurrentAngle;

	// TODO: Use revolute joint of the physics engine
	cdv<float> m_Angle;

//	Vector3 m_vLocalRotationAxis;
//	Matrix34 m_LocalPose;

	CAircraftComponentState::Name m_State;

	// CPhysJoint *m_pJoint

	MeshBoneController_GearUnit *m_pParent;

private:

	void Open();

	void Close();

	void UpdatedFromRequestedState( CAircraftComponentState::Name requested_state );

public:

	MeshBoneController_Cover()
		:
	m_fOpenAngle(0),
	m_pParent(NULL)
	{
		m_Angle.current = 0;
		m_Angle.target = 0;
		m_Angle.vel = 1.0f;
	}

	void Init( const SkeletalMesh& target_skeletal_mesh );

	void Update( float dt );

	void UpdateTransforms();

	bool SatisfyConstraints();

	float GetCurrentAngle() const { return m_Angle.current; }

	void SetParent( MeshBoneController_GearUnit *pParent ) { m_pParent = pParent; }

	const std::string GetName();

	void LoadFromXMLNode( XMLNode& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_COVER; }

	void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneController_AircraftBase::Serialize( ar, version );

		ar & m_fOpenAngle;
	}
};


class MeshBoneController_Tire : public MeshBoneController_AircraftBase
{
public:
};


class MeshBoneController_Shaft: public MeshBoneController_Cover
{
	boost::shared_ptr<MeshBoneController_Tire> m_pTire;
public:

	MeshBoneController_Shaft() {}

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_SHAFT; }
};


class MeshBoneController_GearUnit : public MeshBoneController_AircraftBase
{
	/// first component is always the main shaft with tire
//	std::vector<MeshBoneController_Cover> m_vecComponent;
	std::vector< boost::shared_ptr<MeshBoneController_Cover> > m_vecpComponent;

	CAircraftComponentState::Name m_State;

	/// requested state (open or closed)
	CAircraftComponentState::Name m_RequestedState;

	//
	// holds default pose of each component
	//

//	Matrix34 m_TirePose;
//	Matrix34 m_ShaftPose;

	/// covers of the gearbox that moves separately from the shaft and the tire
//	std::vector<Matrix34> m_vecCoverPose;

//	float m_fShaftClosedAngle; // always 0
//	float m_fShaftOpenAngle;

	//
	// holds states
	//

//	double m_OpenStartTime;
//	double m_CloseStartTime;

//	cdv<float> m_ShaftAngle;
//	float m_fTireAngle;
//	Vector3 m_vRotationAxis;
//	std::vector< cdv<float> > m_vecfCoverAngle;

//	CBoneControlParam& Shaft() { return m_vecBoneControlParam[0]; }
//	CBoneControlParam& Tire() { return m_vecBoneControlParam[1]; }
//	CBoneControlParam& Tire() { return 2 <= m_vecBoneControlParam.size() ? m_vecBoneControlParam[1] : CBoneControlParam::NullObject(); }

public:

	MeshBoneController_GearUnit() {}

	MeshBoneController_GearUnit( boost::shared_ptr<SkeletalMesh> pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: MeshBoneController_AircraftBase( pTargetMesh, pSimulator ) {}

	virtual void UpdateTransforms();

	virtual void SetTargetMesh( boost::shared_ptr<SkeletalMesh> pTargetMesh );

	void Init( const SkeletalMesh& target_skeletal_mesh );

	void Update( float dt );

	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_GEAR_UNIT; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	CAircraftComponentState::Name GetRequestedState() { return m_RequestedState; }

	void Open();

	void Close();

	boost::shared_ptr<MeshBoneController_Cover> GetComponent( const std::string& component_name );

	friend class CItemDatabaseBuilder;
};


class MeshBoneController_CockpitCanopy : public MeshBoneController_AircraftBase
{

public:

	MeshBoneController_CockpitCanopy( boost::shared_ptr<SkeletalMesh> pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: MeshBoneController_AircraftBase( pTargetMesh, pSimulator ) {}

	virtual void UpdateTransforms() {}

//	virtual void LoadFromXMLNode( XMLNode& reader );

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		MeshBoneController_AircraftBase::Serialize( ar, version );
//		ar & m_RotationDirection;
	}

	friend class CItemDatabaseBuilder;
};

} // namespace amorphous



#endif		/*  __MeshBoneController_Aircraft_H__  */
