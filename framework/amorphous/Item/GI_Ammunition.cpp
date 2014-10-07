#include "GI_Ammunition.hpp"
#include "Stage/Serialization_BaseEntityHandle.hpp"
#include "XML/XMLNode.hpp"


namespace amorphous
{

using namespace std;


//======================================================================================
// CGI_Ammunition
//======================================================================================

CGI_Ammunition::CGI_Ammunition()
:
m_NumLoaded(0),
m_fMuzzleSpeed(100.0f),
m_fPower(1.0f),
m_fRange(100000.0f)
{
	m_MuzzleFlashBaseEntity.SetBaseEntityName( "mflsh" );
	m_TypeFlag |= (TYPE_AMMO);
}


void CGI_Ammunition::SetData( char* pcAmmoType, float fMuzzleSpeed, float fPower,
						      int iNumPellets, char* pcBaseEntityName)
{
	m_strAmmoType  = pcAmmoType;
	m_fMuzzleSpeed = fMuzzleSpeed;
	m_fPower       = fPower;
//	m_iNumPellets  = iNumPellets;
	m_AmmoBaseEntity.SetBaseEntityName( pcBaseEntityName );
}


void CGI_Ammunition::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

	ar & m_strAmmoType;

	ar & m_fMuzzleSpeed;
	ar & m_fPower;

	ar & m_fRange;

	ar & m_AmmoBaseEntity;
	ar & m_MuzzleFlashBaseEntity;
}


void CGI_Ammunition::OnOwnerLeftStage()
{
	m_AmmoBaseEntity.SetBaseEntityPointer( NULL );
	m_AmmoBaseEntity.SetState( BaseEntityHandle::STATE_UNINITIALIZED );

	m_MuzzleFlashBaseEntity.SetBaseEntityPointer( NULL );
	m_MuzzleFlashBaseEntity.SetState( BaseEntityHandle::STATE_UNINITIALIZED );
}


void CGI_Ammunition::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	reader.GetChildElementTextContent( "AmmoType",              m_strAmmoType );
	reader.GetChildElementTextContent( "MuzzleSpeed",           m_fMuzzleSpeed );
	reader.GetChildElementTextContent( "Power",                 m_fPower );
	reader.GetChildElementTextContent( "Range",                 m_fRange );

	string ammo_base_entity_name;
	reader.GetChildElementTextContent( "AmmoBaseEntity",        ammo_base_entity_name );
	if( 0 < ammo_base_entity_name.length() )
		m_AmmoBaseEntity.SetBaseEntityName( ammo_base_entity_name.c_str() );

	string muzzle_flash_base_entity_name;
	reader.GetChildElementTextContent( "MuzzleFlashBaseEntity", muzzle_flash_base_entity_name );
	m_MuzzleFlashBaseEntity.SetBaseEntityName( muzzle_flash_base_entity_name.c_str() );
}


} // namespace amorphous
