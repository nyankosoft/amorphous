#ifndef  __MeshBoneControllerFactory_H__
#define  __MeshBoneControllerFactory_H__


#include "gds/Support/Serialization/Serialization.hpp"
#include "gds/Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;


class CMeshBoneControllerFactory : public IArchiveObjectFactory
{

public:

	CMeshBoneControllerFactory();

	virtual ~CMeshBoneControllerFactory();

	IArchiveObjectBase *CreateObject( const unsigned int id );

};


#endif		/*  __MeshBoneControllerFactory_H__  */
