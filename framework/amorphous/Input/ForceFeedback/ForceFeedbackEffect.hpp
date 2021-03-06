#ifndef __ForceFeedbackEffect_HPP__
#define __ForceFeedbackEffect_HPP__


#include <memory>
#include "amorphous/base.hpp"
#include "fwd.hpp"
#include "ForceFeedbackTargetDevice.hpp"
#include "amorphous/Input/fwd.hpp"


namespace amorphous
{

class CForceFeedbackEffectDescVisitor
{
public:

	virtual void Visit( CConstantForceFeedbackEffectDesc& desc ) {}
	virtual void Visit( CRampForceFeedbackEffectDesc& desc ) {}
	virtual void Visit( CPeriodicForceFeedbackEffectDesc& desc ) {}
	virtual void Visit( CCustomForceFeedbackEffectDesc& desc ) {}
};


class CFFCoordSys
{
public:
	enum Name
	{
		CARTESAN,
		POLAR,
		SPHERICAL,
		NUM_COORDINATE_SYSTEMS
	};
};


class CForceFeedbackEffectDesc
{
public:

	int duration;
	uint gain;
	CFFCoordSys::Name coord_systems;
	uint num_axes;
	std::vector<int> direction;

public:

	inline CForceFeedbackEffectDesc();

	virtual std::shared_ptr<CForceFeedbackEffectDesc> CreateCopy() const = 0;

	virtual void Accept( CForceFeedbackEffectDescVisitor& visitor ) {}
};


class CConstantForceFeedbackEffectDesc : public CForceFeedbackEffectDesc
{
public:

	int magnitude; ///< [-10000,10000]

public:

	CConstantForceFeedbackEffectDesc()
		:
	magnitude(0)
	{}

	std::shared_ptr<CForceFeedbackEffectDesc> CreateCopy() const { return std::shared_ptr<CForceFeedbackEffectDesc>( new CConstantForceFeedbackEffectDesc(*this) ); }

	void Accept( CForceFeedbackEffectDescVisitor& visitor ) { visitor.Visit( *this ); }
};


class CRampForceFeedbackEffectDesc : public CForceFeedbackEffectDesc
{
public:

	int start; ///< [-10000,10000]
	int end;   ///< [-10000,10000]

public:
	CRampForceFeedbackEffectDesc()
		:
	start(0),
	end(0)
	{}

	std::shared_ptr<CForceFeedbackEffectDesc> CreateCopy() const { return std::shared_ptr<CForceFeedbackEffectDesc>( new CRampForceFeedbackEffectDesc(*this) ); }

	void Accept( CForceFeedbackEffectDescVisitor& visitor ) { visitor.Visit( *this ); }
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

	virtual Result::Name OnInputDevicePlugged() { return Result::UNKNOWN_ERROR; }
};


class CForceFeedbackEffect
{
	std::shared_ptr<CForceFeedbackEffectImpl> m_pImpl;

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

	friend class InputDevice;
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
coord_systems(CFFCoordSys::CARTESAN),
num_axes(1)
{
}

inline Result::Name CForceFeedbackEffect::Init( const CForceFeedbackEffectDesc& desc, const CForceFeedbackTargetDevice& target )
{
	return m_pImpl->Init( desc, target );
}


} // namespace amorphous



#endif /* __ForceFeedbackEffect_HPP__ */
