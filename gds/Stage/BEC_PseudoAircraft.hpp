#ifndef  __BEC_AircraftNozzleFlames_H__
#define  __BEC_AircraftNozzleFlames_H__

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "BaseEntityHandle.hpp"

class CCopyEntity;
class CGI_Aircraft;
class CStage;


class CBEC_AircraftNozzleFlames
{
public:

	CBaseEntityHandle m_NozzleFlame;

	CBEC_AircraftNozzleFlames();

	void CreateNozzleFlames( CCopyEntity* pCopyEnt,
//		                     int child_entity_offset,
		                     CGI_Aircraft* pAircraft,
							 CStage* pStage );

	/// \param child_entity_offset starting index of the nozzle flame entity
	/// CCopyEntity::Act() of muzzle flame entities are called
	void UpdateNozzleFlames( CCopyEntity* pCopyEnt,
		                     int child_entity_offset,
		                     CGI_Aircraft* pAircraft );

	void ShowNozzleFlames( CCopyEntity* pCopyEnt,
		                   int child_entity_offset,
					       CGI_Aircraft* pAircraft );

	void HideNozzleFlames( CCopyEntity* pCopyEnt,
		                   int child_entity_offset,
					       CGI_Aircraft* pAircraft );

};


#endif /* __BEC_AircraftNozzleFlames_H__ */