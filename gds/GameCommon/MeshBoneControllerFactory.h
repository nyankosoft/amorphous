#ifndef  __MeshBoneControllerFactory_H__
#define  __MeshBoneControllerFactory_H__


#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CMeshBoneControllerFactory : public IArchiveObjectFactory
{

public:

	CMeshBoneControllerFactory();

	virtual ~CMeshBoneControllerFactory();

	IArchiveObjectBase *CreateObject( const unsigned int id );

};


#endif		/*  __MeshBoneControllerFactory_H__  */
