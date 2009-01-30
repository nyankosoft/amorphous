
#include "StageEntryPoint.hpp"


CStageEntryPoint::CStageEntryPoint()
{
	m_vPosition = Vector3(0,0,0);
	memset( &m_matDefaultOrient, 0, sizeof(Matrix33) );
}

Matrix33 *CStageEntryPoint::GetDefaultOrientation()
{
	if( m_matDefaultOrient.GetColumn(0) == Vector3(0,0,0) &&
		m_matDefaultOrient.GetColumn(1) == Vector3(0,0,0) &&
		m_matDefaultOrient.GetColumn(2) == Vector3(0,0,0) )
		return NULL;	// no default orientation is set
	else
		return &m_matDefaultOrient;
}
