#ifndef  __StdInputDeviceStateCallback_HPP__
#define  __StdInputDeviceStateCallback_HPP__

#include <boost/shared_ptr.hpp>
#include "Input/DIInputDeviceMonitor.hpp"
#include "Graphics/GraphicsEffectManager.hpp"



// Set by the system at application init phase


// TODO:
// - Add code to initialize CGameTask::ms_pAnimatedGraphicsManager by the system at app init phase

class CStdInputDeviceStateCallback : public CInputDeviceStateCallback
{
	CAnimatedGraphicsManager *m_pEffectMgr;

	boost::shared_ptr<CCombinedRectElement> m_pRect;
	boost::shared_ptr<CTextElement> m_pText;

public:

	CStdInputDeviceStateCallback(CAnimatedGraphicsManager *pMgr)
		:
	m_pEffectMgr(pMgr)
	{
	}

	void OnInputDeviceDetected()
	{
		if( m_pText )
			m_pText->SetText( "An input device has been detected." );
	}

	void OnInputDeviceInitialized()
	{
		if( m_pText )
			m_pText->SetText( "An input device has been successfuly initialized." );
	}

	void OnInputDeviceInitFailed()
	{
		if( m_pText )
			m_pText->SetText( "Initialization of an input device has failed." );
	}

	void OnInputDeviceUnplugged() {}

	void Init()
	{
		using namespace boost;

		if( !m_pEffectMgr )
			return;

		// create graphics elements for the notification bar that tells
		// the state of the input device when they are plugged, initialized, etc.
		shared_ptr<CGraphicsElementManager> pElementMgr = m_pEffectMgr->GetGraphicsElementManager();

		SRect rect = CGraphicsComponent::RectAtCenterBottom( 800, 40, 30 );
		int layer = 10;
		m_pRect
			= pElementMgr->CreateRect(
			rect,
			SFloatRGBAColor( 0.2f, 0.2f, 0.2f, 0.7f ),
			SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.9f ),
			4,
			layer
			);

		int font_id = 0;
		m_pText
			= pElementMgr->CreateText( font_id,
			"Display input device state here.",
			rect,
			CTextElement::TAL_CENTER,
			CTextElement::TAL_CENTER,
			SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.9f ),
			0, 0, // font width & height - set to 0 to use the default font size
			layer - 3
			);
	}
};



#endif /* __StdInputDeviceStateCallback_HPP__ */
