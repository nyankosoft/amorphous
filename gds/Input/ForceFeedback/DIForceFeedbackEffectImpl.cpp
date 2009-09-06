#include "DIForceFeedbackEffectImpl.hpp"
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace boost;


static boost::mutex sg_IDCreationMutex;


static uint GetDIFFEffectID()
{
	boost::mutex::scoped_lock( sg_IDCreationMutex );

	static uint s_Count = 0;
	s_Count += 1;

	return s_Count;
}


CDIForceFeedbackEffectImpl::CDIForceFeedbackEffectImpl()
:
m_pEffect(NULL)
{
	memset( m_aDirection, 0, sizeof(m_aDirection) );
	memset( m_adwAxis,    0, sizeof(m_adwAxis) );
	memset( &m_Effect,     0, sizeof(m_Effect) );
	m_Effect.dwSize = sizeof(DIEFFECT);

	m_ID = GetDIFFEffectID();
}


Result::Name CDIForceFeedbackEffectImpl::Init( boost::shared_ptr<CDIFFParams> pFFParams )
{
	return Init( m_Desc, pFFParams );
}


Result::Name CDIForceFeedbackEffectImpl::Init( const CForceFeedbackEffectDesc& desc, boost::shared_ptr<CDIFFParams> pFFParams )
{
	m_pFFParams = pFFParams;

    DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf = { 0 };

//	DIEFFECT dieffect;
	m_Effect.dwSize          = sizeof( DIEFFECT );
	m_Effect.dwDuration      = (DWORD)desc.duration;
//	m_Effect.dwDuration      = INFINITE;
	m_Effect.dwGain          = (DWORD)desc.gain;
//	m_Effect.dwGain          = DI_FFNOMINALMAX;
//	m_Effect.cAxes           = (DWORD)desc.num_axes
	m_Effect.cAxes           = pFFParams->m_NumFFAxes; // m_dwNumForceFeedbackAxis;
	m_Effect.dwFlags         = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	m_Effect.dwSamplePeriod  = 0;
	m_Effect.dwTriggerButton = DIEB_NOTRIGGER;
	m_Effect.dwTriggerRepeatInterval = 0;
	m_Effect.rgdwAxes        = rgdwAxes;
	m_Effect.rglDirection    = rglDirection;
	m_Effect.lpEnvelope      = 0;
	m_Effect.cbTypeSpecificParams  = sizeof( DICONSTANTFORCE );
	m_Effect.lpvTypeSpecificParams = &cf;
	m_Effect.dwStartDelay    = 0;

	GUID guid = GUID_ConstantForce;

	HRESULT hr = pFFParams->m_pDeviceCopy->CreateEffect( guid, &m_Effect, &m_pEffect, NULL );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
}


Result::Name CDIForceFeedbackEffectImpl::Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target )
{
	m_Desc = desc;

	vector<CInputDevice *>& vecInputDevice = InputDeviceHub().GetInputDeviceGroup(target.m_Group)->InputDevice();
	for( size_t i=0; i<vecInputDevice.size(); i++ )
	{
		if( vecInputDevice[i]->GetInputDeviceType() != target.m_Type )
			continue;

		// target input device
		return vecInputDevice[i]->InitForceFeedbackEffect( *this );
	}

	return Result::UNKNOWN_ERROR;
}


void CDIForceFeedbackEffectImpl::SetInfiniteDuration()
{
	if( !m_pEffect )
		return;

	m_Effect.dwDuration = INFINITE;
	HRESULT hr = m_pEffect->SetParameters( &m_Effect, DIEP_DURATION );
}


void CDIForceFeedbackEffectImpl::SetDurationMS( int duration_ms )
{
	if( !m_pEffect )
		return;

	m_Effect.dwDuration
		= (duration_ms == CForceFeedbackEffect::INFINITE_DURATION) ? INFINITE : (DWORD)duration_ms;

	HRESULT hr = m_pEffect->SetParameters( &m_Effect, DIEP_DURATION );
}


void CDIForceFeedbackEffectImpl::SetGain( uint gain )
{
	if( !m_pEffect )
		return;

	m_Effect.dwGain	= (DWORD)gain;
	HRESULT hr = m_pEffect->SetParameters( &m_Effect, DIEP_GAIN );
}


void CDIForceFeedbackEffectImpl::SetDirection( int dir )
{
}


inline DWORD ToDIFFFlags( U32 start_flags )
{
	DWORD out = 0;
	if( start_flags & CForceFeedbackEffect::SF_SOLO ) out |= DIES_SOLO;

	return out;
}


Result::Name CDIForceFeedbackEffectImpl::Start( int iterations, U32 start_flags )
{
	if( !m_pEffect )
		return Result::UNKNOWN_ERROR;

	DWORD dwIters = (iterations == CForceFeedbackEffect::INFINITE_REPEAT) ? INFINITE : (DWORD)iterations;

	DWORD dwFlags = ToDIFFFlags( start_flags );

	HRESULT hr = m_pEffect->Start( dwIters, dwFlags );

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CDIForceFeedbackEffectImpl::Stop()
{
	if( !m_pEffect )
		return Result::UNKNOWN_ERROR;

	HRESULT hr = m_pEffect->Stop();

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}
