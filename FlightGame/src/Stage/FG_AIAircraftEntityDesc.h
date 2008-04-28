#ifndef  __FG_AIAircraftEntityDesc_H__
#define  __FG_AIAircraftEntityDesc_H__


#include <string>

#include "Stage/fwd.h"
#include "Stage/CopyEntityDesc.h"
#include "Stage/EntityGroupHandle.h"


class CFG_EntityTypeID
{
public:

	enum eTypeID
	{
		AI_AIRCRAFT = CCopyEntityTypeID::USE_ID_OFFSET,
		AI_VEHICLE,
		AI_SHIP,
		NUM_FG_ENTITY_TYPE_IDS
	};
};


class CFG_AIAircraftEntityDesc : public CCopyEntityDesc
{
public:

	inline CFG_AIAircraftEntityDesc()
	{
		SetDefault();
	}

	inline void SetDefault()
	{
		TypeID = CFG_EntityTypeID::AI_AIRCRAFT;
	}

	virtual ~CFG_AIAircraftEntityDesc() {}

};



#endif		/*  __FG_AIAircraftEntityDesc_H__  */
