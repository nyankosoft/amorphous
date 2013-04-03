#ifndef  __PhysClothMeshDesc_HPP__
#define  __PhysClothMeshDesc_HPP__


#include <vector>
#include "TriangleMeshDesc.hpp"
#include "fwd.hpp"


namespace amorphous
{


namespace physics
{


class CClothMeshDesc : public CTriangleMeshDesc
{
public:

	CClothMeshDesc()	{}

	virtual ~CClothMeshDesc() {}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		CTriangleMeshDesc::Serialize( ar, version );
	}

//	unsigned int GetArchiveObjectID() const { return ; }
};


} // namespace physics

} // amorphous



#endif  /*  __PhysClothMeshDesc_HPP__  */
