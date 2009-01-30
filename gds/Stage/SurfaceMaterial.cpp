
#include "SurfaceMaterial.hpp"

#include "Serialization_BaseEntityHandle.hpp"
#include "Sound/Serialization_SoundHandle.hpp"
#include "Support/TextFileScanner.hpp"
#include "Support/Serialization/SerializationEx.hpp"



//===========================================================================
// CPhysicsMaterial
//===========================================================================

void CPhysicsMaterial::Serialize( IArchive& ar, const unsigned int version )
{
	ar & fStaticFriction;
	ar & fDynamicFriction;
	ar & fElasticity;
}



//===========================================================================
// CSurfaceMaterial
//===========================================================================


CSurfaceMaterial CSurfaceMaterial::sm_Null = CSurfaceMaterial( false );


CSurfaceMaterial::CSurfaceMaterial()
:
m_fPenetrationResistance(0),
m_bValid(true)
{
}


CSurfaceMaterial::CSurfaceMaterial( bool valid )
:
m_fPenetrationResistance(0),
m_bValid(valid)
{
}


CSurfaceMaterial::~CSurfaceMaterial()
{
}


void CSurfaceMaterial::LoadReactionEntities( CTextFileScanner& scanner )
{
	string str;

	for( ; !scanner.TryScanLine( "[Entity:End]", str ); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "OnBulletHit", str ) )
			m_vecReactionEntity[REACTION_TO_BULLET_HIT].push_back( CBaseEntityHandle(str.c_str()) );
		else if( scanner.TryScanLine( "OnBulletOut", str ) )
			m_vecReactionEntity[REACTION_TO_BULLET_OUT].push_back( CBaseEntityHandle(str.c_str()) );
		else if( scanner.TryScanLine( "OnExplosion_S", str ) )
			m_vecReactionEntity[REACTION_TO_EXPLOSION_S].push_back( CBaseEntityHandle(str.c_str()) );
		else if( scanner.TryScanLine( "OnExplosion_M", str ) )
			m_vecReactionEntity[REACTION_TO_EXPLOSION_M].push_back( CBaseEntityHandle(str.c_str()) );
		else if( scanner.TryScanLine( "OnExplosion_L", str ) )
			m_vecReactionEntity[REACTION_TO_EXPLOSION_L].push_back( CBaseEntityHandle(str.c_str()) );
	}
}


void CSurfaceMaterial::LoadReactionSounds( CTextFileScanner& scanner )
{
	string str;

	for( ; !scanner.TryScanLine( "[Sound:End]", str ); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "OnBulletHit", str ) )
			m_vecSound[REACTION_TO_BULLET_HIT].push_back( CSoundHandle(str) );
		else if( scanner.TryScanLine( "OnBulletOut", str ) )
			m_vecSound[REACTION_TO_BULLET_OUT].push_back( CSoundHandle(str) );
		else if( scanner.TryScanLine( "OnExplosion_S", str ) )
			m_vecSound[REACTION_TO_EXPLOSION_S].push_back( CSoundHandle(str) );
		else if( scanner.TryScanLine( "OnExplosion_M", str ) )
			m_vecSound[REACTION_TO_EXPLOSION_M].push_back( CSoundHandle(str) );
		else if( scanner.TryScanLine( "OnExplosion_L", str ) )
			m_vecSound[REACTION_TO_EXPLOSION_L].push_back( CSoundHandle(str) );
	}
}


void CSurfaceMaterial::LoadFromTextFile( CTextFileScanner& scanner )
{
	string str;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "[Material:End]", str ) )
			break;

		if( scanner.TryScanLine( "Name", m_strName ) ) continue;

		else if( scanner.TryScanLine( "PenetrationResistance", m_fPenetrationResistance ) )     continue;
		else if( scanner.TryScanLine( "StaticFriction",        m_PhysicsMaterial.fStaticFriction ) )   continue;
		else if( scanner.TryScanLine( "DynamicFriction",       m_PhysicsMaterial.fDynamicFriction ) ) continue;
		else if( scanner.TryScanLine( "Elasticity",            m_PhysicsMaterial.fElasticity ) )            continue;

		else if( scanner.TryScanLine( "[Entity:Start]", str ) )
		{
			LoadReactionEntities( scanner );
		}

		else if( scanner.TryScanLine( "[Sound:Start]", str ) )
		{
			LoadReactionSounds( scanner );
		}
/*
		else if( scanner.TryScanLine( "EntityOnHit", str ) )
		{
			m_vecReactionEntity[REACTION_TO_BULLET_HIT].push_back( CBaseEntityHandle() );
			m_vecReactionEntity[REACTION_TO_BULLET_HIT].back().SetBaseEntityName( str.c_str() );
		}

		else if( scanner.TryScanLine( "EntityOnOut", str ) )
		{
			m_vecReactionEntity[REACTION_TO_BULLET_OUT].push_back( CBaseEntityHandle() );
			m_vecReactionEntity[REACTION_TO_BULLET_OUT].back().SetBaseEntityName( str );
		}


		else if( scanner.TryScanLine( "SoundOnBulletHit", str ) )
			m_vecSound[REACTION_TO_BULLET_HIT].push_back( CSoundHandle( str ) );

		else if( scanner.TryScanLine( "SoundOnHit", str ) )
			m_vecSound[REACTION_TO_HIT].push_back( CSoundHandle( str ) );*/
	}
}


void CSurfaceMaterial::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_strName;

	ar & m_fPenetrationResistance;

	ar & m_PhysicsMaterial;

	for( int i=0; i<NUM_REACTION_TYPES; i++ )
		ar & m_vecReactionEntity[i];

	for( int i=0; i<NUM_REACTION_TYPES; i++ )
		ar & m_vecSound[i];

	ar & m_bValid;


	// how come these things do not cause any compiler error
//	ar & m_vecReactionEntity[NUM_REACTION_TYPES];

//	ar & m_vecSound[NUM_REACTION_TYPES];
}







/*
void CSurfaceMaterial::Load( CSurfaceMaterialData& src_material )
{
	m_strName = src_material.m_strName;

	m_fPenetrationResistance = src_material.m_fPenetrationResistance;

	m_PhysicsMaterial = src_material.m_PhysicsMaterial;

	size_t i,j;
	for( i=0; i<NUM_REACTION_TYPES; i++ )
	{
		m_vecReactionEntity[i].clear();

		for( j=0; j<src_material.m_vecstrReactionEntityName[i].size(); j++ )
		{
			m_vecReactionEntity[i].push_back( CBaseEntityHandle() );
			m_vecReactionEntity[i].back().SetBaseEntityName( src_material.m_vecstrReactionEntityName[i][j].c_str() );
		}
	}

	// load sounds to play when objects hit the material
	for( i=0; i<NUM_REACTION_TYPES; i++ )
	{
		m_vecSound[i].clear();

		for( j=0; j<src_material.m_vecstrSoundName[i].size(); j++ )
		{
			m_vecSound[i].push_back( CSoundHandle() );
			m_vecSound[i].back().SetResourceName( src_material.m_vecstrSoundName[i][j].c_str() );
		}
	}
}
*/
