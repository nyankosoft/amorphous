#include "Cartridge.hpp"
#include "XML/XMLNodeReader.hpp"

using namespace std;


void CCartridge::Serialize( IArchive& ar, const unsigned int version )
{
	CGI_Ammunition::Serialize( ar, version );

	ar & (uint&)m_Caliber;
	ar & m_NumPellets;

}


void CCartridge::LoadFromXMLNode( CXMLNodeReader& reader )
{
	CGI_Ammunition::LoadFromXMLNode( reader );

	m_Caliber = GetCaliberFromName( reader.GetChild( "Caliber" ).GetTextContent().c_str() );

	reader.GetChildElementTextContent( "NumPellets", m_NumPellets );
}
