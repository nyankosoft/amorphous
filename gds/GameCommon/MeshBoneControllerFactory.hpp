#ifndef  __MeshBoneControllerFactory_H__
#define  __MeshBoneControllerFactory_H__


#include "gds/Support/Serialization/Serialization.hpp"
#include "gds/Support/Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace serialization;


class MeshBoneControllerFactory : public IArchiveObjectFactory
{

public:

	MeshBoneControllerFactory();

	virtual ~MeshBoneControllerFactory();

	IArchiveObjectBase *CreateObject( const unsigned int id );

};

} // namespace amorphous



#endif		/*  __MeshBoneControllerFactory_H__  */
