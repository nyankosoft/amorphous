#include "Magazine.hpp"
#include "Cartridge.hpp"
#include "amorphous/XML/XMLNode.hpp"


namespace amorphous
{

using namespace std;


int Magazine::LoadCartridges( std::shared_ptr<Cartridge>& pCartridge, uint num_max_rounds_to_load )
{
	if( !pCartridge )
		return 0;

	if( GetCaliber() != pCartridge->GetCaliber() )
		return 0;

	int spare_capacity = m_Capacity - (int)m_pLoadedCartridges.size();

	int num_rounds_to_load = take_min( spare_capacity, (int)num_max_rounds_to_load );

	int num_available = pCartridge->IncreaseLoadedQuantity( num_rounds_to_load );
	for( int i=0; i<num_available; i++ )
		m_pLoadedCartridges.push( pCartridge );

	return num_available;
}


void Magazine::Serialize( IArchive& ar, const unsigned int version )
{
	GameItem::Serialize( ar, version );

	ar & (uint&)m_Caliber;
	ar & m_Capacity;
}


void Magazine::LoadFromXMLNode( XMLNode& reader )
{
	GameItem::LoadFromXMLNode( reader );

	m_Caliber = GetCaliberFromName( reader.GetChild( "Caliber" ).GetTextContent().c_str() );

	reader.GetChildElementTextContent( "Capacity",      m_Capacity );
}


} // namespace amorphous
