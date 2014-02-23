#ifndef  __STAGEENTRYPOINT_H__
#define  __STAGEENTRYPOINT_H__

#include <string>
using namespace std;

#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix33.hpp"


namespace amorphous
{


class StageEntryPoint
{
	string m_strName;

	Vector3 m_vPosition;
	Matrix33 m_matDefaultOrient;

public:
	StageEntryPoint();
	~StageEntryPoint() {}

	string& GetName() { return m_strName; }
	void SetName() { }

	Vector3& GetPosition() { return m_vPosition; }
	Matrix33 *GetDefaultOrientation();

};


} // namespace amorphous



#endif		/*  __STAGEENTRYPOINT_H__  */