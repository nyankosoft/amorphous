#include "MeshBoneControllerFactory.hpp"
#include "MeshBoneController_Aircraft.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{


MeshBoneControllerFactory::MeshBoneControllerFactory()
{
}


MeshBoneControllerFactory::~MeshBoneControllerFactory()
{
}


IArchiveObjectBase *MeshBoneControllerFactory::CreateObject( const unsigned int id )
{
	switch(id)
	{
		case MeshBoneControllerBase::ID_AIRCRAFT_FLAP:      return new MeshBoneController_Flap;
		case MeshBoneControllerBase::ID_AIRCRAFT_VFLAP:     return new MeshBoneController_VFlap;
		case MeshBoneControllerBase::ID_AIRCRAFT_ROTOR:     return new MeshBoneController_Rotor;
		case MeshBoneControllerBase::ID_AIRCRAFT_COVER:     return new MeshBoneController_Cover;
		case MeshBoneControllerBase::ID_AIRCRAFT_SHAFT:     return new MeshBoneController_Shaft;
		case MeshBoneControllerBase::ID_AIRCRAFT_GEAR_UNIT: return new MeshBoneController_GearUnit;
//		case MeshBoneControllerBase::ID_AIRCRAFT_:           return new ;

		default:
			LOG_PRINT_ERROR( " An invalid base id or mesh controller: " + to_string(id) );
			return NULL;
	}
}


} // namespace amorphous
