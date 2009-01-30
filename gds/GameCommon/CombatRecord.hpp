#ifndef __COMBATRECORD_H__
#define __COMBATRECORD_H__


#include <string>
#include <vector>

#include "3DMath/Vector3.hpp"


// TODO:
// - identifying the entity that destroyed a terget stored in KillReport
// - script module
// ar.IsDestroyed( "" );


/**
 used in a mission to quickly record entities destroyed by the player
 sorted later when the mission is done
*/
class KillReport
{
public:
	std::string base_name;
	std::string entity_name;
	std::string terminator;

	int score;

	Vector3 vWorldPos;

	unsigned long time;

	int type;	///< surface / air

	int kill_type;	///< gun, missile, crash, etc.

public:

	KillReport() : score(0), vWorldPos(Vector3(0,0,0)), time(0), type(0), kill_type(0) {}

/*	KillReport(
		const string& _base_name,
		const string& _entity_name,
		int s,
		Vector3 pos,
		unsigned long _time,
		int _type,
		);*/

	enum KilledTargetType
	{
		TT_GROUND,
		TT_AIR,
		NUM_TARGET_TYPES
	};

};


class CombatRecord
{
public:
	std::vector<KillReport> m_vecKillReport;
};


#endif /* __COMBATRECORD_H__ */
