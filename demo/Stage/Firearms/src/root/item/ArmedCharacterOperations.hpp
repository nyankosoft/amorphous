#ifndef __ArmedCharacterOperations_HPP__
#define __ArmedCharacterOperations_HPP__


#include "amorphous/Item/SkeletalCharacter.hpp"
#include "amorphous/Item/Cartridge.hpp"
#include "amorphous/Item/Magazine.hpp"
#include "amorphous/Item/Firearm.hpp"
using namespace amorphous;


template<class T>
inline void GetItems( const std::vector< std::shared_ptr<GameItem> >& pSrcItems,
			   std::vector< std::shared_ptr<T> >& pDestItems )
{
	for( size_t i=0; i<pSrcItems.size(); i++ )
	{
		std::shared_ptr<T> pDest = boost::dynamic_pointer_cast<T,GameItem>( pSrcItems[i] );

		if( pDest )
			pDestItems.push_back( pDest );
	}
}


class FirearmsCarryState
{
	enum Flags
	{
		CSF_HOLSTERED                          = (1 << 0),
		CSF_GRIP_HELD_BY_RIGHT_HAND            = (1 << 1),
		CSF_GRIP_HELD_BY_LEFT_HAND             = (1 << 2),
		CSF_VERTICAL_GRIP_HELD_BY_RIGHT_HAND   = (1 << 3),
		CSF_VERTICAL_GRIP_HELD_BY_LEFT_HAND    = (1 << 4),
		CSF_HANDGUARD_GRIP_HELD_BY_RIGHT_HAND  = (1 << 5),
		CSF_HANDGUARD_GRIP_HELD_BY_LEFT_HAND   = (1 << 6),
	};
};


class CArmedCharacterOperations : public SkeletalCharacterOperations
{
	boost::weak_ptr<CArmedCharacterOperations> m_pSelf;

	bool m_IsAiming;

private:

	std::shared_ptr<ItemEntity> GetSkeletalCharacterEntity();

	std::shared_ptr<CStage> GetStage();

	boost::weak_ptr<CArmedCharacterOperations> GetWeakPtr() { return m_pSelf; }


	void InitMotionNodesForFirearmOperations( SkeletalCharacter& skeletal_character );

	void LoadMeshes();

public:

	CArmedCharacterOperations()
		:
//	m_SelectedFirearmUnitIndex(-1)
	m_IsAiming(false)
	{
		m_SelectionIndices.resize( 4, -1 );
	}

	~CArmedCharacterOperations() {}

	class FirearmUnit
	{
	public:
		std::shared_ptr<Firearm> m_pFirearm;
		std::shared_ptr<Magazine> m_pMagazine;
//		std::vector< std::shared_ptr<Magazine> > m_pMagazines;
		std::shared_ptr<Cartridge> m_pCartridge;

		U32 m_CarryStateFlags;

		EntityHandle<ItemEntity> m_pFirearmEntity;
//		std::vector< EntityHandle<ItemEntity> > m_pMagazineEntities;

		std::shared_ptr<MotionPrimitive> m_pFireMotion;
		std::shared_ptr<MotionPrimitive> m_pFiredToAimingMotion;
		std::shared_ptr<MotionPrimitive> m_pReloadMotion;

		FirearmUnit()
			:
		m_CarryStateFlags(0)
		{}
	};

	// Firearms

	std::shared_ptr<Cartridge> m_pCartridges;

//	std::shared_ptr<Magazine> m_pMagazines[Caliber::NUM_CALIBERS];

//	std::shared_ptr<Firearm> m_pFirearms;

	std::vector<FirearmUnit> m_FirearmUnits;

//	std::shared_ptr<Firearm> m_pSelectedFirearm;

//	int m_SelectedFirearmUnitIndex;

	std::vector<int> m_SelectionIndices;

public:

	void Init( SkeletalCharacter& skeletal_character );

//	void SelectFirearmUnit( int index ) { m_SelectedFirearmUnitIndex = index; }
	void SelectFirearmUnit( uint selection_index, int unit_index )
	{
		if( (int)m_FirearmUnits.size() <= unit_index )
			return;

		if( m_SelectionIndices.size() <= selection_index )
			m_SelectionIndices.insert( m_SelectionIndices.begin(), selection_index - m_SelectionIndices.size() + 1, -1 );

		m_SelectionIndices[selection_index] = unit_index;
	}

	void UpdateFirearm( int unit_index );

	EntityHandle<ItemEntity> CreateFirearmEntity( uint unit_index );

	void CreateEntities( uint unit_index );

	void Update( float dt );

	bool HandleInput( const InputData& input_data, int action_code );

	void FireSelectedFirearm( uint selection_index );

	void Serialize( IArchive& ar, const unsigned int version ) {}

	void CreateEntityForSelectedFirearm( uint selection_index );

	void SetWeakPtr( boost::weak_ptr<CArmedCharacterOperations> pSelf ) { m_pSelf = pSelf; }

	void SetAiming( bool aiming ) { m_IsAiming = aiming; }
};



class CAimingMotionNode : public CCharacterMotionNodeAlgorithm
{
	std::shared_ptr<CArmedCharacterOperations> m_pArmedCharacterOperations;

public:

	CAimingMotionNode( std::shared_ptr<CArmedCharacterOperations> pArmedCharacterOperations )
		:
	m_pArmedCharacterOperations( pArmedCharacterOperations )
	{}

	void Update( float dt );

	bool HandleInput( const InputData& input, int action_code );

	void EnterState();

	void ExitState();
};


#endif /* __ArmedCharacterOperations_HPP__ */
