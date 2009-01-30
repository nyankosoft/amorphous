#ifndef __BINARYNODE_H__
#define __BINARYNODE_H__


#include "3DMath/aabb3.hpp"


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



#endif  /*  __BINARYNODE_H__  */