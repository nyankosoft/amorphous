#include "ForceFeedbackEffect.hpp"
#include "Input/InputDevice.hpp"
#include  "DIForceFeedbackEffectImpl.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


class CDIForceFeedbackEffectImplList
{
	std::vector< std::shared_ptr<CDIForceFeedbackEffectImpl> > m_vecpList;

public:

	CDIForceFeedbackEffectImplList() {}

	void Register( std::shared_ptr<CDIForceFeedbackEffectImpl> pDIImpl )
	{
		m_vecpList.push_back( pDIImpl );
	}

//	void Unregister( std::shared_ptr<CDIForceFeedbackEffectImpl> pDIImpl )
	void Unregister( std::shared_ptr<CForceFeedbackEffectImpl> pImpl )
	{
		for( size_t i=0; i<m_vecpList.size(); i++ )
		{
			if( m_vecpList[i]->GetID() == pImpl->GetID() )
			{
				m_vecpList.erase( m_vecpList.begin() + i );
				return;
			}
		}
	}

	void OnInputDevicePlugged()
	{
		for( size_t i=0; i<m_vecpList.size(); i++ )
		{
			m_vecpList[i]->OnInputDevicePlugged();
		}
	}
};



CDIForceFeedbackEffectImplList& DIForceFeedbackEffectImplList()
{
	static CDIForceFeedbackEffectImplList s_obj;
	return s_obj;
}


// draft
void OnInputDevicePlugged()
{
	DIForceFeedbackEffectImplList().OnInputDevicePlugged();
}


CForceFeedbackEffect::CForceFeedbackEffect()
{
	std::shared_ptr<CDIForceFeedbackEffectImpl> pDIFFImpl( new CDIForceFeedbackEffectImpl );

	// register the FF effect impl to the list
	DIForceFeedbackEffectImplList().Register( pDIFFImpl );

	m_pImpl = pDIFFImpl;
}


CForceFeedbackEffect::~CForceFeedbackEffect()
{
	// ForceFeedbackEffectImplManager( m_pImpl );
	DIForceFeedbackEffectImplList().Unregister( m_pImpl );
}


} // namespace amorphous
