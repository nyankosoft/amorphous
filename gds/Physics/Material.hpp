#ifndef  __PhysMaterial_H__
#define  __PhysMaterial_H__

#include "../3DMath/precision.h"


namespace physics
{


class CMaterial
{
public:
/*
	enum Flag
	{
		Anisotropic           = ( 1 << 0 ),
		DisableFriction       = ( 1 << 1 ),
		DisableStrongFriction = ( 1 << 2 ),
	};
*/
	CMaterial() {}

	virtual ~CMaterial() {}

//	virtual void SetDefault() {}

	/// materials ids are set to shape descs
	virtual int GetMaterialID() = 0;

	/// Sets the coefficient of dynamic friction. 
	virtual void SetDynamicFriction (Scalar coef)= 0;
	 
	/// Retrieves the DynamicFriction value. 
	virtual Scalar  GetDynamicFriction () const = 0;
	 
	/// Sets the coefficient of static friction. 
	virtual void  SetStaticFriction (Scalar coef)= 0;
	 
	/// Retrieves the coefficient of static friction. 
	virtual Scalar  GetStaticFriction () const = 0;
	 
	/// Sets the coefficient of restitution. 
	virtual void  SetRestitution (Scalar rest)= 0;
	 
	/// Retrieves the coefficient of restitution. 
	virtual Scalar  GetRestitution () const = 0;
	 
	/// Sets the dynamic friction coefficient along the secondary (V) axis. 
//	virtual void  SetDynamicFrictionV (Scalar coef)=0 

	/// Retrieves the dynamic friction coefficient for the V direction. 
//	virtual Scalar  GetDynamicFrictionV () const =0 

	/// Sets the static friction coefficient along the secondary (V) axis. 
//	virtual void  SetStaticFrictionV (Scalar coef)=0 

	/// Retrieves the static friction coefficient for the V direction. 
//	virtual Scalar  GetStaticFrictionV () const =0 

	/// Sets the shape space direction (unit vector) of anisotropy. 
//	virtual void  SetDirOfAnisotropy (const Vector3 &vec)=0 

	/// Retrieves the direction of anisotropy value. 
//	virtual Vector3  GetDirOfAnisotropy () const =0 

	/// Sets the flags, a combination of the bits defined by the enum NxMaterialFlag . 
//	virtual void  SetFlags (U32 flags)=0 

	/// Retrieves the flags. See NxMaterialFlag. 
//	virtual U32  GetFlags () const =0 

	/// Sets the friction combine mode. 
//	virtual void  SetFrictionCombineMode (NxCombineMode combMode)=0 

	/// Retrieves the friction combine mode. 
//	virtual NxCombineMode  GetFrictionCombineMode () const =0 

	/// Sets the restitution combine mode. 
//	virtual void  SetRestitutionCombineMode (NxCombineMode combMode)=0 

	/// Retrieves the restitution combine mode. 
//	virtual NxCombineMode  GetRestitutionCombineMode () const =0 

};


} // namespace physics



#endif		/*  __PhysMaterial_H__  */
