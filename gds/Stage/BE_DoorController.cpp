#include "BE_Door.hpp"
#include "BE_DoorController.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "Stage.hpp"
#include "GameMessage.hpp"

#include "Serialization_BaseEntityHandle.hpp"
#include "Sound/Serialization_SoundHandle.hpp"
#include "Support/Serialization/Serialization_FixedVector.hpp"
#include "Utilities/TextFileScannerExtensions.hpp"

using namespace std;


CBE_DoorController::SDoorComponentSet::SDoorComponentSet()
{
	matLocalPose = Matrix34Identity();
}


CBE_DoorController::CBE_DoorController()
{
	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_strKeyCode = "NO_LOCK";

//	m_acStartSound[0] = '\0';
//	m_acStopSound[0] = '\0';

}


CBE_DoorController::~CBE_DoorController()
{
}


void CBE_DoorController::Init()
{
	int i, num_components = m_vecDoorComponent.size();
	for( i=0; i<num_components; i++ )
	{
		LoadBaseEntity( m_vecDoorComponent[i].entity );
	}
}


void CBE_DoorController::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	CCopyEntityDesc door_component;
	Matrix34 matDoorWorldPose, matControllerWorldPose;

//	matControllerWorldPose.vPosition = pCopyEnt->Position();
//	matControllerWorldPose.matOrient = Matrix33( pCopyEnt->Right(), pCopyEnt->Up(), pCopyEnt->GetDirection() );
	matControllerWorldPose = pCopyEnt->GetWorldPose();

	int i, iNumComponents = m_vecDoorComponent.size();
	for( i=0; i< iNumComponents; i++ )
	{
		door_component.pBaseEntityHandle = &(m_vecDoorComponent[i].entity);

		// set the current copy entity (door controller) as the parent
		door_component.pParent    = pCopyEnt;

		// calc position & orientation
		matDoorWorldPose = matControllerWorldPose * m_vecDoorComponent[i].matLocalPose;

		door_component.WorldPose = matDoorWorldPose;

		m_pStage->CreateEntity( door_component );
	}
}


void CBE_DoorController::Act(CCopyEntity* pCopyEnt)
{
	int i, iNumComponents = pCopyEnt->GetNumChildren(); //m_vecDoorComponent.size();
	CCopyEntity *pDoorEntity;
	for( i=0; i< iNumComponents; i++ )
	{
		pDoorEntity = pCopyEnt->GetChild(i);
		if( pDoorEntity )
			pDoorEntity->Act();
	}
}


//void CBE_DoorController::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}


void CBE_DoorController::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	short& rsDoorState = pCopyEnt_Self->s1;
	int i, iNumChildren = pCopyEnt_Self->GetNumChildren();
	CCopyEntity *pDoor;

	switch( rGameMessage.effect )
	{
	case GM_DOOR_TOUCHED:

		if( m_strKeyCode == "NO_LOCK" )
		{	// door is not locked
//			if( rsDoorState == CBE_Door::DOOR_CLOSED )
//				this->SoundManager().PlayAt( m_acStartSound, pCopyEnt_Self->Position() );
//			rsDoorState = CBE_Door::DOOR_OPENING;

			for( i=0; i<iNumChildren; i++ )
			{	// change the state of every door component to OPENING
				pDoor = pCopyEnt_Self->GetChild(i);
				if( pDoor )
					pDoor->s1 = CBE_Door::DOOR_OPENING;
			}
		}
		else
		{	// the door is locked - the entity currently tring to open this door has to input valid key code
			SGameMessage msg;
			msg.effect = GM_KEYCODE_REQUEST;
			msg.pcStrParam = &m_strKeyCode.at(0);
//			msg.pcStrParam = m_strKeyCode.begin();
			msg.sender = pCopyEnt_Self->Self();
			SendGameMessageTo( msg, rGameMessage.pEntity0 );
		}
		break;
	
	case GM_KEYCODE_INPUT:
		if( rsDoorState == CBE_Door::DOOR_OPENING )
			return; // the door has already been started to open - no need to try

		if( strcmp(rGameMessage.pcStrParam, m_strKeyCode.c_str()) == 0 )
		{	// valid key-code - open the door
//			if( rsDoorState == CBE_Door::DOOR_CLOSED )
//				this->SoundManager().PlayAt( m_acStartSound, pCopyEnt_Self->Position() );
//			rsDoorState = CBE_Door::DOOR_OPENING;

			for( i=0; i<iNumChildren; i++ )
			{	// change the state of every door component to OPENING
				pDoor = pCopyEnt_Self->GetChild(i);
				if( pDoor )
					pDoor->s1 = CBE_Door::DOOR_OPENING;
			}
		}
		break;
	}
}


//void CBE_DoorController::Draw(CCopyEntity* pCopyEnt) {}


bool CBE_DoorController::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string str;

	if( scanner.TryScanLine( "KEYCODE", m_strKeyCode ) ) return true;

	// door component
	if( scanner.TryScanLine( "DOOR_ENTITY", str ) )
	{
		m_vecDoorComponent.push_back( SDoorComponentSet() );
		m_vecDoorComponent.back().entity.SetBaseEntityName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "DOOR_LOCAL_POS", m_vecDoorComponent.back().matLocalPose.vPosition ) ) return true;

//	if( scanner.TryScanLine( "DOOR_LOCAL_ORIENT" ) ) return true;

	if( scanner.TryScanLine( "START_SND", str ) )
	{
		m_OpenSound.SetResourceName( str.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "STOP_SND", str ) )
	{
		m_CloseSound.SetResourceName( str.c_str() );
		return true;
	}

	return false;
}


void CBE_DoorController::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_strKeyCode;
	ar & m_OpenSound & m_CloseSound;
	ar & m_vecDoorComponent;
}
