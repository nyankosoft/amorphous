#ifndef __Cartridge_HPP__
#define __Cartridge_HPP__


#include "GI_Ammunition.hpp"
#include "../GameCommon/Caliber.hpp"


namespace amorphous
{


class Cartridge : public CGI_Ammunition
{
public:

	Caliber::Name m_Caliber;

	uint m_NumPellets;		///< for shotgun shells

public:

	Cartridge()
		:
	m_Caliber(Caliber::OTHER),
	m_NumPellets(1)
	{
		m_AmmoBaseEntity.SetBaseEntityName( "__DefaultBullet__" );
	}

	Caliber::Name GetCaliber() const { return m_Caliber; }

	inline uint GetNumPellets() const { return m_NumPellets; }

	unsigned int GetArchiveObjectID() const { return ID_CARTRIDGE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );
};


} // namespace amorphous



#endif /* __Cartridge_HPP__ */
