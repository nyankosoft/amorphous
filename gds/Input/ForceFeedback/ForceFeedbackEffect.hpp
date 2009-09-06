#ifndef __ForceFeedbackEffect_HPP__
#define __ForceFeedbackEffect_HPP__


#include <boost/shared_ptr.hpp>
#include "../../base.hpp"
#include "fwd.hpp"
#include "ForceFeedbackTargetDevice.hpp"
#include "Input/fwd.hpp"


class CForceFeedbackEffectDesc
{
public:

	int duration;
	uint gain;
	uint num_axes;

public:

	inline CForceFeedbackEffectDesc();
};


class CForceFeedbackEffectImpl
{
public:

	virtual ~CForceFeedbackEffectImpl() {}

	virtual bool IsValid() { return true; }

	virtual Result::Name Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target ) = 0;

	virtual void SetDuration( double duration_sec ) = 0;

	virtual Result::Name Start( int iterations, U32 start_flags ) = 0;

	virtual Result::Name Stop() = 0;

	virtual uint GetID() const { return 0; }
};


class CForceFeedbackEffect
{
	boost::shared_ptr<CForceFeedbackEffectImpl> m_pImpl;

public:

	enum StartFlags
	{
		SF_SOLO = (1 << 0),
	};

	enum Params
	{
		INFINITE_REPEAT = -1,
		INFINITE_DURATION = -1,
	};

public:

	CForceFeedbackEffect();

	~CForceFeedbackEffect();

	bool IsValid() { return m_pImpl ? m_pImpl->IsValid() : false; }

	inline Result::Name Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target = CForceFeedbackTargetDevice() );

	void SetDuration( double duration_sec ) { m_pImpl->SetDuration( duration_sec ); }

	Result::Name Start( int iterations, U32 start_flags ) { return m_pImpl->Start( iterations, start_flags ); }

	Result::Name Stop() { return m_pImpl->Stop(); }

	friend class CInputDevice;
};


/*
class CForceFeedbackEffectManager
{
public:
};
*/

//========================= inline implementations =========================

inline CForceFeedbackEffectDesc::CForceFeedbackEffectDesc()
:
duration(CForceFeedbackEffect::INFINITE_DURATION),
gain(100),
num_axes(1)
{
}

inline Result::Name CForceFeedbackEffect::Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target )
{
	return m_pImpl->Init( desc, target );
}



#endif /* __ForceFeedbackEffect_HPP__ */
