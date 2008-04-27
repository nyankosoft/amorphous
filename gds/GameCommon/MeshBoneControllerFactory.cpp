
#include "MeshBoneControllerFactory.h"

#include "MeshBoneController_Aircraft.h"

#include "Support/Log/DefaultLog.h"

//#include "Support/memory_helpers.h"



CMeshBoneControllerFactory::CMeshBoneControllerFactory()
{
}


CMeshBoneControllerFactory::~CMeshBoneControllerFactory()
{
}


IArchiveObjectBase *CMeshBoneControllerFactory::CreateObject( const unsigned int id )
{
	switch(id)
	{
		case CMeshBoneControllerBase::ID_AIRCRAFT_FLAP:		return new CMeshBoneController_Flap;
		case CMeshBoneControllerBase::ID_AIRCRAFT_VFLAP:	return new CMeshBoneController_VFlap;
		case CMeshBoneControllerBase::ID_AIRCRAFT_ROTOR:	return new CMeshBoneController_Rotor;
//		case MeshBoneControllerBase::ID_AIRCRAFT_:		return new ;

		default:
			g_Log.Print( "CMeshBoneControllerFactory::CreateObject() - invalid base id or mesh controller" );
			return NULL;
	}
}
