#ifndef  __OverlapTestAABB__H__
#define  __OverlapTestAABB__H__

#include "3DMath/aabb3.hpp"
#include <vector>

class CCopyEntity;


class COverlapTestAABB
{
public:

	COverlapTestAABB( const AABB3& _aabb, std::vector<CCopyEntity *> *buffer )
		: aabb(_aabb), pvecBuffer(buffer) { GroupIndex = 0; }

	COverlapTestAABB( const AABB3& _aabb, std::vector<CCopyEntity *> *buffer, short group_index )
		: aabb(_aabb), pvecBuffer(buffer), GroupIndex(group_index) {}

	AABB3 aabb;	///< [in] bounding box to test
	std::vector<CCopyEntity *> *pvecBuffer;	///< [out] overlapping entities
	
	short GroupIndex;;	///< [in] entity group of the src aabb
};


#endif  /*  __OverlapTestAABB__H__  */