#include "Scene.hpp"
#include "Joint.hpp"


namespace amorphous
{

using namespace physics;


CJointImpl *CScene::GetJointImpl( CJoint& joint )
{
	return joint.m_pImpl;
}


void CScene::SetJointImpl( CJoint& joint, CJointImpl& impl )
{
	joint.m_pImpl = &impl;
}


} // namespace amorphous
