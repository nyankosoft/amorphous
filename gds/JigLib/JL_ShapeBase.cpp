
#include "JL_ShapeBase.h"


CJL_ShapeBase::CJL_ShapeBase()
{
	m_sShape = JL_SHAPE_INVALID;
	m_sMaterialIndex = -1;

	m_LocalPose.Default();
//	m_WorldPose.Default();

	m_pPhysicsActor = NULL;
}


CJL_ShapeBase::~CJL_ShapeBase()
{
}


void CJL_ShapeBase::SetShape( CJL_ShapeDesc& rDesc )
{
	m_sShape = rDesc.sShape;
	m_LocalPose.vPosition = rDesc.vLocalPos;
	m_LocalPose.matOrient = rDesc.matLocalOrient;
	m_sMaterialIndex = rDesc.sMaterialIndex;
}