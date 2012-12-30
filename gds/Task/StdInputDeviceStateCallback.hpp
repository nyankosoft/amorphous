#ifndef  __StdInputDeviceStateCallback_HPP__
#define  __StdInputDeviceStateCallback_HPP__

#include <boost/shared_ptr.hpp>
#include "Input/DIInputDeviceMonitor.hpp"
#include "Graphics/GraphicsEffectManager.hpp"


namespace amorphous
{



// Set by the system at application init phase


// TODO:
// - Add code to initialize CGameTask::ms_pAnimatedGraphicsManager by the system at app init phase

class CStdInputDeviceStateCallback : public CInputDeviceStateCallback
{
	CAnimatedGraphicsManager *m_pEffectMgr;

	boost::shared_ptr<CCombinedRectElement> m_pRect;
	boost::shared_ptr<CTextElement> m_pText;

	CGraphicsEffectHandle m_TextFadein;
	CGraphicsEffectHandle m_RectFadein;
	CGraphicsEffectHandle m_TextFadeout;
	CGraphicsEffectHandle m_RectFadeout;

public:

	CStdInputDeviceStateCallback(CAnimatedGraphicsManager *pMgr)
		:
	m_pEffectMgr(pMgr)
	{
	}

	~CStdInputDeviceStateCallback()
	{
		using namespace boost;

		if( !m_pEffectMgr )
			return;

		shared_ptr<CGraphicsElementManager> pElementMgr = m_pEffectMgr->GetGraphicsElementManager();

		shared_ptr<CGraphicsElement> pElems[] = { m_pRect, m_pText };
		pElementMgr->RemoveElement( pElems[0] );
		pElementMgr->RemoveElement( pElems[1] );
	}

	void SetFadeInAndOut()
	{
		if( !m_pEffectMgr )
			return;

		m_pEffectMgr->CancelEffect( m_RectFadein );
		m_pEffectMgr->CancelEffect( m_TextFadein );
		m_pEffectMgr->CancelEffect( m_RectFadeout );
		m_pEffectMgr->CancelEffect( m_TextFadeout );

		m_pEffectMgr->SetTimeOffset();

		if( m_pRect )
		{
			m_RectFadein  = m_pEffectMgr->ChangeAlphaTo( m_pRect, 0.0, 0.2, 0, 1.0f, 0 ); // fade in
			m_RectFadeout = m_pEffectMgr->ChangeAlphaTo( m_pRect, 3.0, 3.2, 0, 0.0f, 0 ); // fade out
		}

		if( m_pText )
		{
			m_TextFadein  = m_pEffectMgr->ChangeAlphaTo( m_pText, 0.0, 0.2, 0, 1.0f, 0 ); // fade in
			m_TextFadeout = m_pEffectMgr->ChangeAlphaTo( m_pText, 3.0, 3.2, 0, 0.0f, 0 ); // fade out

		}

	}

	void OnInputDeviceDetected()
	{
		SetFadeInAndOut();

		if( m_pText )
			m_pText->SetText( "An input device has been detected." );
	}

	void OnInputDeviceInitialized()
	{
		SetFadeInAndOut();

		if( m_pText )
			m_pText->SetText( "An input device has been successfuly initialized." );
	}

	void OnInputDeviceInitFailed()
	{
		SetFadeInAndOut();

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

		// Hide the elements - show only when input device events are received.
		m_pRect->SetAlpha( 0, 0.0f );
		m_pText->SetAlpha( 0, 0.0f );
	}
};


} // amorphous



#endif /* __StdInputDeviceStateCallback_HPP__ */
