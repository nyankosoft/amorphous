
#ifndef __CJL_ShapeDescFACTORY_H__
#define __CJL_ShapeDescFACTORY_H__


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


//===========================================================================
// CJL_ShapeDescFactory
//===========================================================================

class CJL_ShapeDescFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject( const unsigned int id );
};


#endif  /* __CJL_ShapeDescFACTORY_H__   */