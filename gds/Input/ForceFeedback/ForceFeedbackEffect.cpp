#include "ForceFeedbackEffect.hpp"
#include "Input/InputDevice.hpp"


using namespace std;
using namespace boost;


#include  "DIForceFeedbackEffectImpl.hpp"


class CDIForceFeedbackEffectImplList
{
	std::vector< boost::shared_ptr<CDIForceFeedbackEffectImpl> > m_vecpList;

public:

	CDIForceFeedbackEffectImplList() {}

	void Register( boost::shared_ptr<CDIForceFeedbackEffectImpl> pDIImpl )
	{
		m_vecpList.push_back( pDIImpl );
	}

//	void Unregister( boost::shared_ptr<CDIForceFeedbackEffectImpl> pDIImpl )
	void Unregister( boost::shared_ptr<CForceFeedbackEffectImpl> pImpl )
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
};


CDIForceFeedbackEffectImplList& DIForceFeedbackEffectImplList()
{
	static CDIForceFeedbackEffectImplList s_obj;
	return s_obj;
}


CForceFeedbackEffect::CForceFeedbackEffect()
{
	shared_ptr<CDIForceFeedbackEffectImpl> pDIFFImpl( new CDIForceFeedbackEffectImpl );

	// register the FF effect impl to the list
	DIForceFeedbackEffectImplList().Register( pDIFFImpl );

	m_pImpl = pDIFFImpl;
}


CForceFeedbackEffect::~CForceFeedbackEffect()
{
	// ForceFeedbackEffectImplManager( m_pImpl );
	DIForceFeedbackEffectImplList().Unregister( m_pImpl );
}
