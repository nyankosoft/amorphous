#ifndef  __PhysConvexShapeDesc_HPP__
#define  __PhysConvexShapeDesc_HPP__


#include "fwd.hpp"
#include "ShapeDesc.hpp"

#include <assert.h>


namespace physics
{


class CConvexShapeDesc : public CShapeDesc
{
public:

	/// borrowed reference
	CConvexMesh *pConvexMesh;

	CConvexShapeDesc()
		:
	pConvexMesh(NULL)
	{
	}

	virtual ~CConvexShapeDesc() {}

	unsigned int GetArchiveObjectID() const { return PhysShape::Convex; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CShapeDesc::Serialize( ar, version );

//#error <How do you serialize pTriangleMesh???>
		assert( !"How do you serialize pTriangleMesh???\n" );

	}
};


} // namespace physics


#endif  /*  __PhysConvexShapeDesc_HPP__  */
