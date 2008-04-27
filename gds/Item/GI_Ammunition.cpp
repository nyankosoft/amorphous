
#include "GI_Ammunition.h"
#include "Stage/Serialization_BaseEntityHandle.h"


//======================================================================================
// CGI_Ammunition
//======================================================================================

void CGI_Ammunition::SetData( char* pcAmmoType, float fMuzzleSpeed, float fPower,
						      int iNumPellets, char* pcBaseEntityName)
{
	strcpy(m_acAmmoType, pcAmmoType);
	m_fMuzzleSpeed = fMuzzleSpeed;
	m_fPower       = fPower;
	m_iNumPellets  = iNumPellets;
	m_AmmoBaseEntity.SetBaseEntityName( pcBaseEntityName );
}


void CGI_Ammunition::OnOwnerLeftStage()
{
	m_AmmoBaseEntity.SetBaseEntityPointer( NULL );
	m_AmmoBaseEntity.SetState( CBaseEntityHandle::STATE_UNINITIALIZED );

	m_MuzzleFlashBaseEntity.SetBaseEntityPointer( NULL );
	m_MuzzleFlashBaseEntity.SetState( CBaseEntityHandle::STATE_UNINITIALIZED );
}