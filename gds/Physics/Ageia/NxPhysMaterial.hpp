#ifndef  __NxPhysMaterial_H__
#define  __NxPhysMaterial_H__

#include "3DMath/precision.h"
#include "../Material.hpp"
#include "NxPhysics.h"


namespace physics
{


class CNxPhysMaterial : public CMaterial
{
	NxMaterial *m_pNxMaterial;

	/// owner scene (borrowed reference)
	NxScene *m_pScene;

public:

	CNxPhysMaterial(NxMaterial *pNxMaterial, NxScene *pScene)
		:
	m_pNxMaterial(pNxMaterial),
	m_pScene(pScene)
	{}

	virtual ~CNxPhysMaterial() { m_pScene->releaseMaterial( *m_pNxMaterial ); }

	/// materials ids are set to shape descs
	virtual int GetMaterialID() { return (int)m_pNxMaterial->getMaterialIndex(); }

	/// Sets the coefficient of dynamic friction. 
	virtual void SetDynamicFriction (Scalar coef) { m_pNxMaterial->setDynamicFriction( coef ); } 

	/// Retrieves the DynamicFriction value. 
	virtual Scalar  GetDynamicFriction () const { return m_pNxMaterial->getDynamicFriction(); } 

	/// Sets the coefficient of static friction. 
	virtual void  SetStaticFriction (Scalar coef) { m_pNxMaterial->setStaticFriction( coef ); } 

	/// Retrieves the coefficient of static friction. 
	virtual Scalar  GetStaticFriction () const { return m_pNxMaterial->getStaticFriction(); } 

	/// Sets the coefficient of restitution. 
	virtual void  SetRestitution (Scalar rest){ m_pNxMaterial->setRestitution( rest ); } 

	/// Retrieves the coefficient of restitution. 
	virtual Scalar  GetRestitution () const { return m_pNxMaterial->getRestitution(); } 

};


} // namespace physics



#endif		/*  __NxPhysMaterial_H__  */
