#ifndef __Magazine_HPP__
#define __Magazine_HPP__


#include <stack>
#include "amorphous/GameCommon/Caliber.hpp"
#include "GameItem.hpp"


namespace amorphous
{


class Cartridge;


class Magazine : public GameItem
{
	Caliber::Name m_Caliber;
	uint m_Capacity;

	std::stack< std::shared_ptr<Cartridge> > m_pLoadedCartridges;

	bool m_IsInserted; ///< true if the magazine is currently in the gun

public:

	Magazine()
		:
	m_Caliber(Caliber::OTHER),
	m_Capacity(0),
	m_IsInserted(false)
	{}

	~Magazine() {}

	Caliber::Name GetCaliber() const { return m_Caliber; }

	uint GetCapacity() const { return m_Capacity; }

	uint GetNumLoadedCartridges() const { return (uint)m_pLoadedCartridges.size(); }

	bool IsEmpty() const { return m_pLoadedCartridges.empty(); }

	int LoadCartridges( std::shared_ptr<Cartridge>& pCartridge, uint num_max_rounds_to_load );

	std::shared_ptr<Cartridge> TakeNextCartridge()
	{
		if( IsEmpty() )
			return std::shared_ptr<Cartridge>();

		std::shared_ptr<Cartridge> pNextCartridge = m_pLoadedCartridges.top();
		m_pLoadedCartridges.pop();
		return pNextCartridge;
	}

	unsigned int GetArchiveObjectID() const { return ID_MAGAZINE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( XMLNode& reader );

	void SetInserted( bool inserted ) { m_IsInserted = inserted; }

	bool IsInserted() const { return m_IsInserted; }
};


} // namespace amorphous



#endif /* __Magazine_HPP__ */
