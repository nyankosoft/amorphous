#include "MeshBoneControllerFactory.hpp"
#include "MeshBoneController_Aircraft.hpp"
#include "Support/Log/DefaultLog.hpp"


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
		case CMeshBoneControllerBase::ID_AIRCRAFT_FLAP:      return new CMeshBoneController_Flap;
		case CMeshBoneControllerBase::ID_AIRCRAFT_VFLAP:     return new CMeshBoneController_VFlap;
		case CMeshBoneControllerBase::ID_AIRCRAFT_ROTOR:     return new CMeshBoneController_Rotor;
		case CMeshBoneControllerBase::ID_AIRCRAFT_COVER:     return new CMeshBoneController_Cover;
		case CMeshBoneControllerBase::ID_AIRCRAFT_SHAFT:     return new CMeshBoneController_Shaft;
		case CMeshBoneControllerBase::ID_AIRCRAFT_GEAR_UNIT: return new CMeshBoneController_GearUnit;
//		case MeshBoneControllerBase::ID_AIRCRAFT_:           return new ;

		default:
			g_Log.Print( "CMeshBoneControllerFactory::CreateObject() - invalid base id or mesh controller" );
			return NULL;
	}
}
