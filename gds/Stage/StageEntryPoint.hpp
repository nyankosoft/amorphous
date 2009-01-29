#ifndef  __STAGEENTRYPOINT_H__
#define  __STAGEENTRYPOINT_H__

#include <string>
using namespace std;

#include "3DMath/Vector3.h"
#include "3DMath/Matrix33.h"


class CStageEntryPoint
{
	string m_strName;

	Vector3 m_vPosition;
	Matrix33 m_matDefaultOrient;

public:
	CStageEntryPoint();
	~CStageEntryPoint() {}

	string& GetName() { return m_strName; }
	void SetName() { }

	Vector3& GetPosition() { return m_vPosition; }
	Matrix33 *GetDefaultOrientation();

};



#endif		/*  __STAGEENTRYPOINT_H__  */