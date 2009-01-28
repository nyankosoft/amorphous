#ifndef  __MeshBoneController_Aircraft_H__
#define  __MeshBoneController_Aircraft_H__


#include "MeshBoneControllerBase.h"


/**
 * init steps for CMeshBoneController_Aircraft derived classes
 * 1. create an instance: pController = new CMeshBoneController_Flap()
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

class CMeshBoneController_AircraftBase : public CMeshBoneControllerBase
{
protected:

	CPseudoAircraftSimulator *m_pSimulator;

public:

	CMeshBoneController_AircraftBase() : CMeshBoneControllerBase(NULL), m_pSimulator(NULL) {}

	CMeshBoneController_AircraftBase( CD3DXSMeshObject *pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: CMeshBoneControllerBase( pTargetMesh ), m_pSimulator( pSimulator ) {}

	virtual ~CMeshBoneController_AircraftBase() {}

	void SetPseudoAircraftSimulator( CPseudoAircraftSimulator *pSimulator ) { m_pSimulator = pSimulator; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneControllerBase::Serialize( ar, version );
	}

	friend class CItemDatabaseBuilder;
};

/**
 * controls a pair of flaps
 * always holds a pair of right & left flaps. does not hold single right / left flap
 */
class CMeshBoneController_Flap : public CMeshBoneController_AircraftBase
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

	CMeshBoneController_Flap() : m_fAnglePerPitchAccel(0.4f), m_fAnglePerRollAccel(0.4f) {}

	CMeshBoneController_Flap( CD3DXSMeshObject *pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: CMeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_fAnglePerPitchAccel(0.4f), m_fAnglePerRollAccel(0.4f) {}

	virtual void UpdateTransforms();

	virtual void Init();

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_FLAP; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneController_AircraftBase::Serialize( ar, version );
		ar & m_fAnglePerPitchAccel & m_fAnglePerRollAccel;
	}

	friend class CItemDatabaseBuilder;
};


class CMeshBoneController_VFlap : public CMeshBoneController_AircraftBase
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

	CMeshBoneController_VFlap() : m_fAnglePerYawAccel(0.5f), m_Type(TYPE_SINGLE) {}

	CMeshBoneController_VFlap( CD3DXSMeshObject *pTargetMesh,
		                       CPseudoAircraftSimulator *pSimulator,
							   int type = TYPE_SINGLE )
		: CMeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_Type(type) { m_fAnglePerYawAccel = 0.7f; }

	virtual void UpdateTransforms();

	virtual void Init();

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_VFLAP; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneController_AircraftBase::Serialize( ar, version );
		ar & m_fAnglePerYawAccel & m_Type;
	}

	friend class CItemDatabaseBuilder;
};


class CMeshBoneController_Rotor : public CMeshBoneController_AircraftBase
{
//	CPseudoAircraftSimulator *m_pSimulator;

	int m_RotationDirection;
    
	float m_fCurrentRotationAngle;

	enum dir_type
	{
		DIR_CW,		///< clockwise
		DIR_CCW,	///< counter-clockwise
		NUM_DIRS
	};

public:

	CMeshBoneController_Rotor() : m_RotationDirection(DIR_CW) {}

	CMeshBoneController_Rotor( CD3DXSMeshObject *pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: CMeshBoneController_AircraftBase( pTargetMesh, pSimulator ), m_RotationDirection(DIR_CW) {}

	virtual void UpdateTransforms();

	virtual void Init();

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual unsigned int GetArchiveObjectID() const { return ID_AIRCRAFT_ROTOR; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneController_AircraftBase::Serialize( ar, version );
		ar & m_RotationDirection;
	}

	void SetRotationAngle( float angle ) { m_fCurrentRotationAngle = angle; }

	friend class CItemDatabaseBuilder;
};


class CMeshBoneController_Gear : public CMeshBoneController_AircraftBase
{

public:

	CMeshBoneController_Gear() {}

	CMeshBoneController_Gear( CD3DXSMeshObject *pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: CMeshBoneController_AircraftBase( pTargetMesh, pSimulator ) {}

	virtual void UpdateTransforms() {}

//	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneController_AircraftBase::Serialize( ar, version );
//		ar & m_RotationDirection;
	}

	friend class CItemDatabaseBuilder;
};


class CMeshBoneController_CockpitCanopy : public CMeshBoneController_AircraftBase
{

public:

	CMeshBoneController_CockpitCanopy( CD3DXSMeshObject *pTargetMesh, CPseudoAircraftSimulator *pSimulator )
		: CMeshBoneController_AircraftBase( pTargetMesh, pSimulator ) {}

	virtual void UpdateTransforms() {}

//	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CMeshBoneController_AircraftBase::Serialize( ar, version );
//		ar & m_RotationDirection;
	}

	friend class CItemDatabaseBuilder;
};


#endif		/*  __MeshBoneController_Aircraft_H__  */
