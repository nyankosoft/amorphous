#include "ArmedCharacterOperations.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/Stage/Stage.hpp"
#include "amorphous/Item/ItemEntity.hpp"

using namespace std;


void CArmedCharacterOperations::InitMotionNodesForFirearmOperations( SkeletalCharacter& skeletal_character )
{
	vector< shared_ptr<CCharacterMotionNodeAlgorithm> >& motion_node_algorithms
		= skeletal_character.MotionNodeAlgorithms();

	for( size_t i=0; i<motion_node_algorithms.size(); i++ )
	{
		if( !motion_node_algorithms[i] )
			continue;

		const string& node_name = motion_node_algorithms[i]->GetNodeName();
		if( node_name == "fwd"
		 || node_name == "standing" )
		{
//			motion_node_algorithms[i]->AddActionCodeToMotionNodeMap( ACTION_ATK_RAISEWEAPON, ITYPE_KEY_PRESSED, "standing_and_aiming_with_handgun" );
			motion_node_algorithms[i]->AddActionCodeToMotionNodeMap( ACTION_CAMERA_ALIGN, ITYPE_KEY_PRESSED, "standing_and_aiming_with_handgun" );
		}
	}

	shared_ptr<CAimingMotionNode> pAimingMotionNode( new CAimingMotionNode( GetWeakPtr().lock() ) );
	pAimingMotionNode->AddActionCodeToMotionNodeMap( ACTION_CAMERA_ALIGN, ITYPE_KEY_RELEASED, "standing" );
	skeletal_character.SetMotionNodeAlgorithm( "standing_and_aiming_with_handgun", pAimingMotionNode );
	motion_node_algorithms.push_back( pAimingMotionNode );
} 


void CArmedCharacterOperations::Init( SkeletalCharacter& skeletal_character )
{
	using namespace std;

	vector< std::shared_ptr<Firearm> > pFirearms;
	GetItems( skeletal_character.GetOwnedItems(), pFirearms );

	vector< std::shared_ptr<Magazine> > pMags;
	GetItems( skeletal_character.GetOwnedItems(), pMags );

	vector< std::shared_ptr<Cartridge> > pCartridges;
	GetItems( skeletal_character.GetOwnedItems(), pCartridges );

	m_FirearmUnits.resize( 0 );

	for( size_t i=0; i<pFirearms.size(); i++ )
	{
		if( !pFirearms[i] )
			continue;

		// Find a magazine with the highest capacity available
		int mag_index = -1;
		for( int j=0; j<(int)pMags.size(); j++ )
		{
			if( !pMags[j] )
				continue;

			if( pFirearms[i]->IsMagazineCompliant( pMags[j] )
			 && (mag_index==-1 || pMags[mag_index]->GetCapacity() < pMags[j]->GetCapacity() ) )
			{
				mag_index = j;
			}
		}

		if( mag_index == -1 )
		{
			LOG_PRINT_WARNING( "No magazine was found for '" + pFirearms[i]->GetName() + "'." );
			continue;
		}

		std::shared_ptr<Cartridge> pCartridge;
		for( int j=0; j<(int)pCartridges.size(); j++ )
		{
			if( pCartridges[j]->GetCaliber() == pMags[mag_index]->GetCaliber() )
			{
				pCartridge = pCartridges[j];
				break;
			}
		}

		if( !pCartridge )
		{
			LOG_PRINT_WARNING( "No cartridge was found for '" + pFirearms[i]->GetName() + "'." );
			continue;
		}

		m_FirearmUnits.push_back( FirearmUnit() );

		m_FirearmUnits.back().m_pFirearm   = pFirearms[i];
		m_FirearmUnits.back().m_pMagazine  = pMags[mag_index];
		m_FirearmUnits.back().m_pCartridge = pCartridge;

		// magazine cannot be shared between different firearms
		if( 0 <= mag_index && mag_index < (int)pMags.size() )
			pMags.erase( pMags.begin() + (size_t)mag_index );
	}

	InitMotionNodesForFirearmOperations( skeletal_character );

//	for( int i=0; i<skeletal_character.m_pProperty.size(); i++ )
//	{
//		std::shared_ptr<GameItem> pItem = skeletal_character.m_pProperty[i];
//		if( !pItem )
//			continue;

//		std::shared_ptr<Firearm> pFirearm
//			= boost::dynamic_pointer_cast<Firearm,GameItem>( pItem );

//		if( !pFirearm )
//			continue;

//		m_FirearmUnits.push_back( FirearmUnit() );
//		m_FirearmUnits.back().m_pFirearm = pFirearm;
//	}

	LoadMeshes();
}


//void CArmedCharacterOperations::UpdateSelectedFirearm()
//{
//	UpdateSelectedFirearm( m_SelectedFirearmUnitIndex );
//}


void CArmedCharacterOperations::LoadMeshes()
{
	for( size_t i=0; i<m_FirearmUnits.size(); i++ )
	{
		FirearmUnit& unit = m_FirearmUnits[i];
		
		unit.m_pFirearm->LoadMeshObject();
		unit.m_pMagazine->LoadMeshObject();
		unit.m_pCartridge->LoadMeshObject();
	}
}


void CArmedCharacterOperations::UpdateFirearm( int unit_index )
{
	if( unit_index < 0 || (int)m_FirearmUnits.size() <= unit_index )
		return;

	FirearmUnit& selected_unit = m_FirearmUnits[unit_index];

	if( !selected_unit.m_pFirearm )
		return;

	Firearm& selected_firearm = *(selected_unit.m_pFirearm);

/*	switch( selected_firearm.GetFirearmState() )
	{
	case Firearm::FS_SLIDE_FORWARD:
		if( selected_firearm.IsCartridgeChambered() )
		{
		}
		else
		{
//			StartCartridgeFeedingMotion();
			selected_firearm.FeedNextCartridge();
		}
		break;

	case Firearm::FS_SLIDE_MOVING_FORWARD:
		break;

	case Firearm::FS_SLIDE_OPEN:
		// Do nothing
		break;

	default:
		break;
	}*/

	if( selected_firearm.GetMagazine() )
	{
		if( selected_firearm.IsCartridgeChambered() )
		{
			// ready to fire - no need to do anything
		}
		else
		{
			if( selected_firearm.GetMagazine()->IsEmpty() )
			{
				// Reload();
				// Start the mag change motion.
				// When the mag change motion is finished, a new mag is set to the selected_firearm.
				shared_ptr<Magazine> pMag = selected_firearm.DropMagazine();
				pMag->LoadCartridges( selected_unit.m_pCartridge, 0xFFFF );
				selected_firearm.ChangeMagazine( pMag );

				if( selected_firearm.IsSlideStopEngaged() )
					selected_firearm.DisengageSlideStop();
			}
			else
			{
				selected_firearm.PullSlide();
			}
		}
	}
	else
	{
		// Insert a magazine to the firearm

//		GetAnyLoadedMagazine();
		if( !selected_unit.m_pMagazine )
			return;

		selected_unit.m_pMagazine->LoadCartridges( selected_unit.m_pCartridge, 0xFFFF );

		selected_firearm.ChangeMagazine( selected_unit.m_pMagazine );

		if( !selected_firearm.IsCartridgeChambered() )
		{
			if( selected_firearm.IsSlideAtItsMostRearwardPosition() )
			{
				if( selected_firearm.IsSlideStopEngaged() )
				{
					selected_firearm.DisengageSlideStop();
				}

				if( selected_firearm.IsSlideHeld() )
				{
					selected_firearm.ReleaseSlide();
				}
			}
			else
				selected_firearm.PullSlide();
		}
	}

	if( selected_firearm.IsSlideOpen() )
	{
//		selected_firearm.CloseSlide();
	}
}


std::shared_ptr<ItemEntity> CArmedCharacterOperations::GetSkeletalCharacterEntity()
{
	std::shared_ptr<SkeletalCharacter> pCharacter = GetSkeletalCharacter().lock();
	if( !pCharacter )
		return shared_ptr<ItemEntity>();

	EntityHandle<ItemEntity> character = pCharacter->GetItemEntity();

	return character.Get();
}


std::shared_ptr<CStage> CArmedCharacterOperations::GetStage()
{
	shared_ptr<ItemEntity> pCharacter = GetSkeletalCharacterEntity();

	if( !pCharacter )
		return shared_ptr<CStage>();

	return pCharacter->GetStage()->GetWeakPtr().lock();
}


EntityHandle<ItemEntity> CArmedCharacterOperations::CreateFirearmEntity( uint unit_index )
{
	if( (uint)m_FirearmUnits.size() <= unit_index )
		return EntityHandle<ItemEntity>();

	FirearmUnit& unit = m_FirearmUnits[unit_index];
	if( !unit.m_pFirearm )
		return EntityHandle<ItemEntity>();

	EntityHandle<ItemEntity> firearm = unit.m_pFirearm->GetItemEntity();
	if( firearm.Get() )
		return EntityHandle<ItemEntity>(); // the entity for the firearm item already exists

	shared_ptr<CStage> pStage = GetStage();

	CItemStageUtility stg_util( pStage );

	return stg_util.CreateItemEntity( unit.m_pFirearm, Vector3(0,0,0) );
}


void CArmedCharacterOperations::CreateEntityForSelectedFirearm( uint selection_index )
{
	if( selection_index < 0 || (uint)m_SelectionIndices.size() <= selection_index )
		return;

	if( m_SelectionIndices[selection_index] == -1 )
		return; // not selected for selection_index

//	CreateEntityForFirearm( (uint)m_SelectionIndices[selection_index] );
}

/*
void CArmedCharacterOperations::CreateEntityForFirearm( uint index )
{
	if( (uint)m_FirearmUnits.size() <= index )
		return;

	FirearmUnit& selected_unit = m_FirearmUnits[m_SelectedFirearmUnitIndex];

	if( !selected_unit.m_pFirearm )
		return;

	if( selected_unit.m_pFirearm->GetItemEntity().Get() )
	{
		// Already has an item entity
		return;
		// or, terminate this item entity and create a new one
	}

	CItemStageUtility stg_util( m_pStage );
	EntityHandle<ItemEntity> entity = stg_util.CreateItemEntity( selected_unit.m_pFirearm, actor_desc ); // create an entity for the item
}*/


void CArmedCharacterOperations::FireSelectedFirearm( uint selection_index )
{
	if( (uint)m_SelectionIndices.size() <= selection_index )
		return;

	int unit_index = m_SelectionIndices[selection_index];
	if( unit_index < 0 || (int)m_FirearmUnits.size() <= unit_index )
		return;

	if( m_FirearmUnits[unit_index].m_pFirearm )
		m_FirearmUnits[unit_index].m_pFirearm->Fire();
}


void UpdateFirearmUnit( CArmedCharacterOperations::FirearmUnit& firearm_unit, float dt, ItemEntity& character_entity )
{
	Matrix34 character_world_pose = character_entity.GetWorldPose();

	if( !firearm_unit.m_pFirearm )
		return;

	shared_ptr<ItemEntity> pFirearmEntity = firearm_unit.m_pFirearm->GetItemEntity().Get();

	if( !pFirearmEntity )
		return;
	
	Matrix34 firearm_world_pose;
	firearm_world_pose.matOrient = character_world_pose.matOrient;
	firearm_world_pose.vPosition
		= character_world_pose.vPosition
		+ character_world_pose.matOrient.GetColumn(2)
		+ Vector3(0,1,0);

	pFirearmEntity->SetWorldPose( firearm_world_pose );

	firearm_unit.m_pFirearm->Update( dt );
}


void CArmedCharacterOperations::Update( float dt )
{
	if( m_FirearmUnits.empty() )
		return;

	shared_ptr<ItemEntity> pCharacter = GetSkeletalCharacterEntity();
	if( !pCharacter )
		return;

	UpdateFirearm( 0 );

	UpdateFirearmUnit( m_FirearmUnits[0], dt, *pCharacter );

//	UpdateSelectedFirearm( 0 );
}


bool CArmedCharacterOperations::HandleInput( const InputData& input_data, int action_code )
{
	if( action_code == ACTION_ATK_FIRE )
		int break_here = 1;

	if( action_code == ACTION_ATK_FIRE && !m_IsAiming )
	{
		return false;
	}

	int selection_index = 0;
	if( (uint)m_SelectionIndices.size() <= selection_index )
		return false;

	int unit_index = m_SelectionIndices[selection_index];
	if( unit_index < 0 || (int)m_FirearmUnits.size() <= unit_index )
		return false;

	if( m_FirearmUnits[unit_index].m_pFirearm )
		return m_FirearmUnits[unit_index].m_pFirearm->HandleInput( action_code, input_data.iType, input_data.fParam1 );
	else
		return false;
/*
	switch( action_code )
	{
	case ACTION_ATK_FIRE:
		if( input_data.iType == ITYPE_KEY_PRESSED )
		{
			FireSelectedFirearm( 0 );
		}
		break;

	case ACTION_ATK_FIRE0:
	case ACTION_ATK_FIRE1:
	case ACTION_ATK_FIRE2:
	case ACTION_ATK_FIRE3:
		if( input_data.iType == ITYPE_KEY_PRESSED )
		{
			FireSelectedFirearm( action_code - ACTION_ATK_FIRE0 );
		}
		break;

	default:
		break;
	}*/
}


void CAimingMotionNode::Update( float dt )
{
}


bool CAimingMotionNode::HandleInput( const InputData& input, int action_code )
{
	bool res = HandleInputForTransition( input, action_code );
	if( res )
		return true;

	bool handled = false;
	if( m_pArmedCharacterOperations )
	{
//		handled = m_pArmedCharacterOperations->HandleInput( input, action_code );
//		if( handled )
//		{
//			RequestTransition( "standing" );
//		}
	}

	return handled;
}


void CAimingMotionNode::EnterState()
{
	if( m_pArmedCharacterOperations )
		m_pArmedCharacterOperations->SetAiming( true );
}


void CAimingMotionNode::ExitState()
{
	if( m_pArmedCharacterOperations )
		m_pArmedCharacterOperations->SetAiming( false );
}


class CFiringMotionNode
{
public:
};


class CFiredToAimingMotionNode
{
public:
};


class CReloadingMotionNode
{
public:
};
