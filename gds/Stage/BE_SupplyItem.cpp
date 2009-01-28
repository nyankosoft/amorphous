
#include "BE_SupplyItem.h"

#include "GameMessage.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "trace.h"
#include "Stage.h"
#include "Serialization_BaseEntityHandle.h"

#include "../Graphics/Direct3D9.h"
#include "../Graphics/D3DXMeshObject.h"
#include "../Stage/ScreenEffectManager.h"

#include "../Sound/SoundManager.h"

#include "Sound/Serialization_SoundHandle.h"
#include "Sound/SoundManager.h"


CBE_SupplyItem::CBE_SupplyItem()
{
	m_BoundingVolumeType = BVTYPE_AABB;

	m_fRotationSpeed = 0;
	m_iEffect = GM_HEALING;
	m_fAmount = 0;

	m_bLighting = true;

	RaiseEntityFlag( BETYPE_ITEM );

}


void CBE_SupplyItem::Init()
{
	Init3DModel();

	LoadBaseEntity( m_PseudoGlare );
}


void CBE_SupplyItem::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	// set orthogonal axes
	Vector3 vDir = pCopyEnt->GetDirection();
	pCopyEnt->SetDirection_Right( Vector3( vDir.z, 0, -vDir.x ) );
	pCopyEnt->SetDirection_Up( Vector3(0,1,0) );

	if( 0 < strlen(m_PseudoGlare.GetBaseEntityName()) )
	{
		CCopyEntityDesc glare;
		glare.pBaseEntityHandle = &m_PseudoGlare;
		glare.SetWorldPose( pCopyEnt->GetWorldPose() );
		glare.pParent    = pCopyEnt;
		m_pStage->CreateEntity( glare );
	}
}


void CBE_SupplyItem::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
	if( pCopyEnt_Other )
	{
		SGameMessage msg;
		msg.pSenderEntity = pCopyEnt_Self;

		// set the kind of item as basic information
		msg.iEffect = this->m_iEffect;

		// set the name of the item as additional information so that 'pCopyEnt_Other' can specify what this item is
		msg.pcStrParam = const_cast<char *> (this->m_strItemName.c_str());

		// amount of supply delivered to 'pCopyEnt_Other'
		msg.fParam1 = this->m_fAmount;

		// deliver supply to 'pCopyEnt_Other'
		SendGameMessageTo( msg, pCopyEnt_Other );
	}
}


void CBE_SupplyItem::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_EFFECTACCEPTED:
		SoundManager().PlayAt( m_Sound, pCopyEnt_Self->Position() );

		// terminate child entity
		CCopyEntity *pChild;
		pChild = pCopyEnt_Self->GetChild(0);
		if( pChild )
			m_pStage->TerminateEntity( pChild );

		m_pStage->TerminateEntity( pCopyEnt_Self );

/**		if( pCopyEnt_Self->pChild )
			pCopyEnt_Self->pChild-Terminate();**/

		switch( this->m_iEffect )
		{	// use screen effect according to the effect of the current item
		case GM_HEALING:
			m_pStage->GetScreenEffectManager()->FadeInFrom( 0x504060FF, 1.0f, AlphaBlend::One );	// flash with blue 
			break;
		}

		break;
	}
}


void CBE_SupplyItem::Draw(CCopyEntity* pCopyEnt)
{
	Draw3DModel(pCopyEnt);
	return;
}


bool CBE_SupplyItem::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string effect, sound_name, str;

	if( scanner.TryScanLine( "EFFECT", effect ) )
	{
		if( effect == "GM_HEALING" )
			m_iEffect = GM_HEALING;
		else if( effect == "GM_AMMOSUPPLY" )
			m_iEffect = GM_AMMOSUPPLY;
		else if( effect == "GM_DOORKEYITEM" )
			m_iEffect = GM_DOORKEYITEM;
		else
			m_iEffect = 0;
		return true;
	}

	if( scanner.TryScanLine( "ITEMNAME", m_strItemName ) ) return true;
	if( scanner.TryScanLine( "AMOUNT", m_fAmount ) ) return true;
	if( scanner.TryScanLine( "ROT_SPEED", m_fRotationSpeed ) ) return true;

	if( scanner.TryScanLine( "SOUND", sound_name ) )
	{
		if( sound_name == "NO_SOUND" )
			sound_name = "";

		m_Sound.SetResourceName( sound_name );
		return true;
	}

	if( scanner.TryScanLine( "PSEUDO_GLARE", str ) )
	{
		m_PseudoGlare.SetBaseEntityName( str.c_str() );
		return true;
	}

	return false;
}


void CBE_SupplyItem::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_strItemName;
	ar & m_iEffect;
	ar & m_fAmount;
	ar & m_fRotationSpeed;
	ar & m_Sound;;
}
