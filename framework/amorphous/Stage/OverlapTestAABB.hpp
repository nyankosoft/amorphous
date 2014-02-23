#ifndef  __amorphous_OverlapTestAABB_HPP__
#define  __amorphous_OverlapTestAABB_HPP__


#include <vector>
#include "amorphous/3DMath/AABB3.hpp"
#include "fwd.hpp"


namespace amorphous
{


class OverlapTestAABB
{
public:

	OverlapTestAABB( const AABB3& _aabb, std::vector<CCopyEntity *> *buffer )
		: aabb(_aabb), pvecBuffer(buffer), TargetEntityTypeID(-1) { GroupIndex = 0; }

	OverlapTestAABB( const AABB3& _aabb, std::vector<CCopyEntity *> *buffer, short group_index )
		: aabb(_aabb), pvecBuffer(buffer), GroupIndex(group_index), TargetEntityTypeID(-1) {}

	AABB3 aabb;	///< [in] bounding box to test
	std::vector<CCopyEntity *> *pvecBuffer;	///< [out] overlapping entities
	
	short GroupIndex;;	///< [in] entity group of the src aabb

	/// Set this value to target entities with the specific type
	/// Leave this to -1 to target all types of entities
	/// - default: -1
	int TargetEntityTypeID;
};

} // namespace amorphous



#endif  /*  __amorphous_OverlapTestAABB_HPP__  */
