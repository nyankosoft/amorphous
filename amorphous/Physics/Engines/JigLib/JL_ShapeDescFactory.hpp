#ifndef __CJL_ShapeDescFACTORY_H__
#define __CJL_ShapeDescFACTORY_H__


#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace serialization;


//===========================================================================
// CJL_ShapeDescFactory
//===========================================================================

class CJL_ShapeDescFactory : public IArchiveObjectFactory
{
public:
	IArchiveObjectBase *CreateObject( const unsigned int id );
};

} // namespace amorphous



#endif  /* __CJL_ShapeDescFACTORY_H__   */
