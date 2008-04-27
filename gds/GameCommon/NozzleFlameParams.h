#ifndef __NozzleFlameParams_H__
#define __NozzleFlameParams_H__


#include "Support/TextFileScanner.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

/*
class AircraftRecord : public IArchiveObjectBase
{
public:

	AircraftRecord() { memset(this,0,sizeof(AircraftRecord)); }

	int NumSorties;
	int NumDestroyedAirTargets;
	int NumDestroyedGroundTargets;


//	int NumFiredMissiles;
//	int NumDroppedBombs;

	int GetNumDestroyedTargets() { return NumDestroyedAirTargets + NumDestroyedGroundTargets; }
};


float GetAircraftUsageRatio()
{
	CGI_Aircraft* pAircraft = ;
	return (float)(pAircraft->GetRecord()->NumSorties) / (float);
}
*/


/**
 * NozzleFlameParams
 *
*/
class NozzleFlameParams : public IArchiveObjectBase
{
public:

	Matrix34 LocalPose;
	Matrix34 CurrentPose;

	int type;

	enum shape_type
	{
		TYPE_ROUND,
		TYPE_RECTANGULAR,
		NUM_TYPES
	};

	float start_pos;
	float length;
	float width, height;
	float radius;
//	FloatRGBAColor color;

public:

	inline NozzleFlameParams();
	virtual ~NozzleFlameParams() {}

	//	unsigned int GetArchiveObjectID() const { return ; }

	inline void Serialize( IArchive& ar, const unsigned int version );

	inline bool LoadFromFile( CTextFileScanner& scanner );
};


inline NozzleFlameParams::NozzleFlameParams()
{
	LocalPose = Matrix34Identity();
	CurrentPose = Matrix34Identity();

	type = TYPE_ROUND;

	start_pos = 0.0f;
	length = 2.0f;
	width = height = 1.0f;
	radius = 1.0f;
//	color = SFloatRGBAColor(1,0,0,1);	// red
}


inline void NozzleFlameParams::Serialize( IArchive& ar, const unsigned int version )
{
    ar & LocalPose;
    ar & CurrentPose;

	ar & type;
	ar & start_pos;
	ar & length;
	ar & width & height;
	ar & radius;
//	ar & color;
}


inline bool NozzleFlameParams::LoadFromFile( CTextFileScanner& scanner )
{
//	int index;
	float diameter, heading;

	if( scanner.TryScanLine( "nozzle_flame_pos",	LocalPose.vPosition ) )return true;

	if( scanner.TryScanLine( "nozzle_flame_heading", heading ) )
	{
		LocalPose.matOrient = Matrix33RotationY(heading);
		return true;
	}

	if( scanner.TryScanLine( "nozzle_flame_sld",	start_pos, length, diameter ) )
	{
		type = TYPE_ROUND;
		radius = diameter * 0.5f;
		return true;
	}

	if( scanner.TryScanLine( "nozzle_flame_slwh",	start_pos, length, width, height ) )
	{
		type = TYPE_RECTANGULAR;
		return true;
	}

	return false;
}


#endif  /*  __NozzleFlameParams_H__  */
