#ifndef __BINARYNODE_H__
#define __BINARYNODE_H__


#include "amorphous/3DMath/AABB3.hpp"


namespace amorphous
{


#define CONTENTS_SOLID	-1
#define CONTENTS_EMPTY	-2


struct SNode_f
{
	short sPlaneIndex;

	short sCellIndex;

	short sFrontChild;

	short sBackChild;

	AABB3 aabb;
};


} // namespace amorphous



#endif  /*  __BINARYNODE_H__  */