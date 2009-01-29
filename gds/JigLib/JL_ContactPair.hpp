#ifndef  __JL_CONTACTPAIR_H__
#define  __JL_CONTACTPAIR_H__


#include "3DMath/Vector3.h"

class CJL_PhysicsActor;


class CJL_ContactPointInfo
{
public:

	Vector3 vImpulse;

	/// contact position in world space
	Vector3 vPosition;
};


class CJL_ContactPair
{
public:

	CJL_PhysicsActor *pActor[2];

	TCFixedVector< CJL_ContactPointInfo, 8 > vecPointInfo;

	Vector3 vNormal;

public:

	CJL_ContactPair() {}

	~CJL_ContactPair() {}

};


class CJL_ContactPairManager
{
	vector<CJL_ContactPair> m_vecContactPair;

public:

	int GetNumContactPairs() const { m_vecContactPair.size(); }

	CJL_ContactPair& GetContactPair( int index ) { return m_vecContactPair[index]; }
}


#endif		/*  __JL_CONTACTPAIR_H__  */