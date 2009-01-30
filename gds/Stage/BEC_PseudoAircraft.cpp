

#include "BEC_PseudoAircraft.hpp"
#include "CopyEntityDesc.hpp"
#include "CopyEntity.hpp"
#include "BaseEntity.hpp"
#include "Stage.hpp"
#include "Item/GI_Aircraft.hpp"
#include "Stage.hpp"


CBEC_AircraftNozzleFlames::CBEC_AircraftNozzleFlames()
{
	m_NozzleFlame.SetBaseEntityName( "nozzle_flame" );
}


void CBEC_AircraftNozzleFlames::CreateNozzleFlames( CCopyEntity* pCopyEnt,
//												    int child_entity_offset,
		                                            CGI_Aircraft* pAircraft,
                                                    CStage* pStage )
{
//	CBaseEntityHandle base_entity;
//	base_entity.SetBaseEntityName( "nozzle_flame" );

	CCopyEntityDesc desc;
	desc.pParent = pCopyEnt;
//	desc.pBaseEntityHandle = &base_entity;
	desc.pBaseEntityHandle = &m_NozzleFlame;

/*	desc.f1 = 0.0f;	//  nozzle flame local start pos
	const Matrix33& matBaseOrient = pCopyEnt->GetWorldPose().matOrient;
	Matrix33 matOrient;
	matOrient = matBaseOrient * Matrix33RotationAxis( 3.141592f, matBaseOrient.GetColumn(1) );
	desc.SetWorldOrient( matOrient );
	desc.vPosition = pCopyEnt->Position()
		+ pCopyEnt->GetDirection() * (-14.0f);
*/
	vector<NozzleFlameParams>& NozzleFlameParams = pAircraft->GetNozzleFlameParams();
	size_t i, num_nozzles = NozzleFlameParams.size();
	for( i=0; i<num_nozzles; i++ )
	{
		desc.SetWorldPose( pCopyEnt->GetWorldPose() * NozzleFlameParams[i].LocalPose );
		desc.f1 = NozzleFlameParams[i].start_pos;
		desc.f3 = NozzleFlameParams[i].length;
		desc.f4 = NozzleFlameParams[i].radius;

		pStage->CreateEntity( desc );
	}
}


void CBEC_AircraftNozzleFlames::UpdateNozzleFlames( CCopyEntity* pCopyEnt,
		                                            int child_entity_offset,
		                                            CGI_Aircraft* pAircraft )
{
	size_t i, num_nozzles = pAircraft->GetNozzleFlameParams().size();
	for( i=0; i<num_nozzles; i++ )
	{
		int child_index = child_entity_offset + (int)i;
		CCopyEntity *pNozzleFlame = pCopyEnt->m_aChild[child_index].GetRawPtr();

        if( !IsValidEntity( pNozzleFlame ) )
			continue;

		pNozzleFlame->SetWorldPose( pCopyEnt->GetWorldPose() * pAircraft->GetNozzleFlameParams()[i].LocalPose );

		// update nozzle flame entity
		pNozzleFlame->Act();
	}
}


void CBEC_AircraftNozzleFlames::ShowNozzleFlames( CCopyEntity* pCopyEnt,
		                                          int child_entity_offset,
					                              CGI_Aircraft* pAircraft )
{
	size_t i, num_nozzles = pAircraft->GetNozzleFlameParams().size();
	for( i=0; i<num_nozzles; i++ )
	{
		CCopyEntity *pNozzleFlame = pCopyEnt->m_aChild[child_entity_offset + i].GetRawPtr();
		if( !IsValidEntity( pNozzleFlame ) )
			continue;

		pNozzleFlame->EntityFlag |= BETYPE_VISIBLE;
	}
}


void CBEC_AircraftNozzleFlames::HideNozzleFlames( CCopyEntity* pCopyEnt,
		                                          int child_entity_offset,
					                              CGI_Aircraft* pAircraft )
{
	size_t i, num_nozzles = pAircraft->GetNozzleFlameParams().size();
	for( i=0; i<num_nozzles; i++ )
	{
		CCopyEntity *pNozzleFlame = pCopyEnt->m_aChild[child_entity_offset + i].GetRawPtr();
		if( !IsValidEntity( pNozzleFlame ) )
			continue;

		pNozzleFlame->EntityFlag &= ~BETYPE_VISIBLE;	// don't render muzzle flash
	}
}
