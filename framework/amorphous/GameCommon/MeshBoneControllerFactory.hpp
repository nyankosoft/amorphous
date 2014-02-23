#ifndef  __MeshBoneControllerFactory_H__
#define  __MeshBoneControllerFactory_H__


#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/ArchiveObjectFactory.hpp"


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
