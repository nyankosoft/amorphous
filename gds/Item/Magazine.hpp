#ifndef __Magazine_HPP__
#define __Magazine_HPP__


#include <stack>
#include "../GameCommon/Caliber.hpp"
#include "GameItem.hpp"


class CCartridge;


class CMagazine : public CGameItem
{
	Caliber::Name m_Caliber;
	uint m_Capacity;

	std::stack< boost::shared_ptr<CCartridge> > m_pLoadedCartridges;

public:

	CMagazine()
		:
	m_Caliber(Caliber::OTHER),
	m_Capacity(0)
	{}

	~CMagazine() {}

	Caliber::Name GetCaliber() const { return m_Caliber; }

	bool IsEmpty() const { return m_pLoadedCartridges.empty(); }

	int LoadCartridges( boost::shared_ptr<CCartridge>& pCartridge, uint num_max_rounds_to_load );

	boost::shared_ptr<CCartridge> TakeNextCartridge()
	{
		if( IsEmpty() )
			return boost::shared_ptr<CCartridge>();

		boost::shared_ptr<CCartridge> pNextCartridge = m_pLoadedCartridges.top();
		m_pLoadedCartridges.pop();
		return pNextCartridge;
	}

	unsigned int GetArchiveObjectID() const { return ID_MAGAZINE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );
};



#endif /* __Magazine_HPP__ */
