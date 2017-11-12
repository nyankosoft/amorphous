#include "DIForceFeedbackEffectImpl.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include <mutex>


namespace amorphous
{

using namespace std;


static std::mutex sg_IDCreationMutex;


static const char *GetDIErrorString( HRESULT hr )
{
	switch(hr)
	{
	case S_OK:                       return "S_OK";
	case DIERR_INCOMPLETEEFFECT:     return "DIERR_INCOMPLETEEFFECT";
	case DIERR_INVALIDPARAM:         return "DIERR_INVALIDPARAM";
	case DIERR_NOTEXCLUSIVEACQUIRED: return "DIERR_NOTEXCLUSIVEACQUIRED";
	case DIERR_NOTINITIALIZED:       return "DIERR_NOTINITIALIZED";
	case DIERR_UNSUPPORTED:          return "DIERR_UNSUPPORTED";
	default:
		return "<unknown error>";
	}
}


static uint GetDIFFEffectID()
{
	std::lock_guard<std::mutex>( sg_IDCreationMutex );

	static uint s_Count = 0;
	s_Count += 1;

	return s_Count;
}


class CDIEffectInitializer: public CForceFeedbackEffectDescVisitor
{
	DIEFFECT *m_pDIEffect;

	LPDIRECTINPUTDEVICE8 m_pInputDevice;

public:

	LPDIRECTINPUTEFFECT m_pCreatedEffect;

	HRESULT m_hr;

	CDIEffectInitializer( DIEFFECT *pDIEffect, LPDIRECTINPUTDEVICE8 pInputDevice )
		:
	m_pDIEffect(pDIEffect),
	m_pInputDevice(pInputDevice),
	m_pCreatedEffect(NULL),
	m_hr(S_OK)
	{}

	void Visit( CConstantForceFeedbackEffectDesc& desc )
	{
		DICONSTANTFORCE cf = { 0 };
		cf.lMagnitude = (LONG)desc.magnitude;// 5000; // [-10000,10000]
		m_pDIEffect->cbTypeSpecificParams  = sizeof( DICONSTANTFORCE );
		m_pDIEffect->lpvTypeSpecificParams = &cf;
		m_hr = m_pInputDevice->CreateEffect( GUID_ConstantForce, m_pDIEffect, &m_pCreatedEffect, NULL );
	}

	void Visit( CRampForceFeedbackEffectDesc& desc )
	{
		DIRAMPFORCE rf;
		rf.lStart = (LONG)desc.start;
		rf.lEnd   = (LONG)desc.end;
		m_pDIEffect->cbTypeSpecificParams  = sizeof( DIRAMPFORCE );
		m_pDIEffect->lpvTypeSpecificParams = &rf;
		m_hr = m_pInputDevice->CreateEffect( GUID_RampForce, m_pDIEffect, &m_pCreatedEffect, NULL );
	}

	void Visit( CPeriodicForceFeedbackEffectDesc& desc )
	{
/*		DIPERIODIC periodic;
		periodic.dwMagnitude;
		periodic.dwPeriod;
		periodic.dwPhase;
		periodic.lOffset;

		GUID guid = GUID_Square or GUID_Sine or GUID_Triangle, etc.;
		HRESULT hr = m_pInputDevice->CreateEffect( guid, m_pDIEffect, &m_pEffect, NULL );
*/
	}

	void Visit( CCustomForceFeedbackEffectDesc& desc ) {}
};



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


CDIForceFeedbackEffectImpl::~CDIForceFeedbackEffectImpl()
{
	SAFE_RELEASE( m_pEffect );
}


Result::Name CDIForceFeedbackEffectImpl::Init( std::shared_ptr<CDIFFParams> pFFParams )
{
	return Init( *(m_pDesc.get()), pFFParams );
}


DWORD ToDIEffectCoordSys( const CFFCoordSys::Name& cs )
{
	switch( cs )
	{
	case CFFCoordSys::CARTESAN:  return DIEFF_CARTESIAN;
	case CFFCoordSys::POLAR:     return DIEFF_POLAR;
	case CFFCoordSys::SPHERICAL: return DIEFF_SPHERICAL;
	default:
		return DIEFF_CARTESIAN;
	}
}


Result::Name CDIForceFeedbackEffectImpl::Init( const CForceFeedbackEffectDesc& desc, std::shared_ptr<CDIFFParams> pFFParams )
{
	m_pFFParams = pFFParams;

	DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
//	LONG rglDirection[2] = { 0, 0 };
//	DICONSTANTFORCE cf = { 0 };
//	cf.lMagnitude = 5000; // [-10000,10000]
	LONG dir[16];
	memset( dir, 0, sizeof(dir) );
	for( size_t i=0; i<take_min( numof(dir), desc.direction.size() ); i++ )
		dir[i] = (LONG)desc.direction[i];

//	DIEFFECT dieffect;
	m_Effect.dwSize          = sizeof( DIEFFECT );
	m_Effect.dwDuration      = (DWORD)( desc.duration == CForceFeedbackEffect::INFINITE_DURATION ? INFINITE : desc.duration );
//	m_Effect.dwDuration      = INFINITE;
	m_Effect.dwGain          = (DWORD)desc.gain; // [0,10000];
//	m_Effect.dwGain          = DI_FFNOMINALMAX;
//	m_Effect.cAxes           = (DWORD)desc.num_axes
	m_Effect.cAxes           = pFFParams->m_NumFFAxes; // m_dwNumForceFeedbackAxis;
	m_Effect.dwFlags         = ToDIEffectCoordSys(desc.coord_systems) | DIEFF_OBJECTOFFSETS;
	m_Effect.dwSamplePeriod  = 0;
	m_Effect.dwTriggerButton = DIEB_NOTRIGGER;
	m_Effect.dwTriggerRepeatInterval = 0;
	m_Effect.rgdwAxes        = rgdwAxes;
	m_Effect.rglDirection    = dir;//rglDirection;
	m_Effect.lpEnvelope      = 0;
//	m_Effect.cbTypeSpecificParams  = sizeof( DICONSTANTFORCE );
//	m_Effect.lpvTypeSpecificParams = &cf;
	m_Effect.dwStartDelay    = 0;

//	GUID guid = GUID_ConstantForce;

	CDIEffectInitializer initializer( &m_Effect, pFFParams->m_pDeviceCopy );
	m_pDesc->Accept( initializer );
	m_pEffect = initializer.m_pCreatedEffect;

	if( m_pEffect )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;

/*
	HRESULT hr = pFFParams->m_pDeviceCopy->CreateEffect( guid, &m_Effect, &m_pEffect, NULL );

	if( SUCCEEDED(hr) )
		return Result::SUCCESS;
	else
		return Result::UNKNOWN_ERROR;
*/
}


InputDevice *CDIForceFeedbackEffectImpl::GetTargetInputDevice()
{
	vector<InputDevice *>& vecInputDevice = GetInputDeviceHub().GetInputDeviceGroup(m_Target.m_Group)->GetInputDevices();
	for( size_t i=0; i<vecInputDevice.size(); i++ )
	{
		if( vecInputDevice[i]->GetInputDeviceType() != m_Target.m_Type )
			continue;

		// target input device
		return vecInputDevice[i];
	}

	return NULL;
}


Result::Name CDIForceFeedbackEffectImpl::Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target )
{
	m_pDesc = desc.CreateCopy();
	m_Target = target;

	InputDevice *pInputDevice = GetTargetInputDevice();

	if( pInputDevice )
		return pInputDevice->InitForceFeedbackEffect( *this );
	else
		return Result::UNKNOWN_ERROR;
}


Result::Name CDIForceFeedbackEffectImpl::OnInputDevicePlugged()
{
	InputDevice *pInputDevice = GetTargetInputDevice();

	if( pInputDevice )
		return pInputDevice->InitForceFeedbackEffect( *this );
	else
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

	if( FAILED(hr) )
	{
//		LOG_PRINT_ERROR( " IDirectInputEffect::Start() failed. Error: " + string(GetDIErrorString(hr)) );
	}

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


Result::Name CDIForceFeedbackEffectImpl::Stop()
{
	if( !m_pEffect )
		return Result::UNKNOWN_ERROR;

	HRESULT hr = m_pEffect->Stop();

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


} // namespace amorphous
