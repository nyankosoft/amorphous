#ifndef  __SkeletalCharacter_HPP__
#define  __SkeletalCharacter_HPP__


#include <gds/Item/GameItem.hpp>
#include <gds/GameCommon/KeyBind.hpp>
#include <gds/GameCommon/3DActionCode.hpp>
#include <gds/MotionSynthesis/MotionFSM.hpp>

using namespace msynth;


class CSkeletalCharacter;
class CCharacterMotionNodeAlgorithm;

/**
  walk, turn
  - needs to be polling operation?
  - rationale: speed of walking and turning is determined by analog input.


*/
class CSkeletalCharacter : public CGameItem
{
	boost::shared_ptr<msynth::CMotionGraphManager> m_pMotionGraphManager;

	boost::shared_ptr<CMeshContainerRenderMethod> m_pRenderMethod;

	boost::shared_ptr<msynth::CMotionFSM> m_pLowerLimbsMotionsFSM;

	float m_fFwdSpeed;
	float m_fTurnSpeed;

	boost::shared_ptr<msynth::CMotionFSMInputHandler> m_pInputHandler;

	boost::shared_ptr<CKeyBind> m_pKeyBind;

	std::vector< boost::shared_ptr<CCharacterMotionNodeAlgorithm> > m_pMotionNodes;

	boost::shared_ptr<msynth::CMotionPrimitive> m_pSkeletonSrcMotion;

public:

	CSkeletalCharacter();

	~CSkeletalCharacter() {}

//	void InitResources();

	void OnEntityCreated( CCopyEntity& entity );

	void Update( float dt );

	void Render();

	void ProcessInput( const SInputData& input, int action_code );

	float GetFwdSpeed() const { return m_fFwdSpeed; }
	float GetTurnSpeed() const { return m_fTurnSpeed; }

	void SetFwdSpeed( float fSpeed ) { m_fFwdSpeed = fSpeed; }
	void SetTurnSpeed( float fSpeed ) { m_fTurnSpeed = fSpeed; }

	void SetKeyBind( boost::shared_ptr<CKeyBind> pKeyBind );
};



/*
class CCharacterMotionInputHandler : public CInputHandler
{
	boost::shared_ptr<CKeyBind> m_pKeyBind;

	boost::shared_ptr<CSkeletalCharacter> m_pCharacter;

public:

	CCharacterMotionInputHandler(
		boost::shared_ptr<CSkeletalCharacter> pCharacter,
		boost::shared_ptr<CKeyBind> pKeyBind )
		:
	m_pCharacter(pCharacter),
	m_pKeyBind(pKeyBind)
	{
	}

	void ProcessInput( SInputData& input );
};
*/


/// Used to avoid null checking at runtime
class CNullMotionNodeAlgorithm
{
public:

	CNullMotionNodeAlgorithm() {}

	void Update( float dt ) {}

	/// Returns true if the input was handled.
	bool HandleInput( const SInputData& input ) { return false; }
};





class CCharacterMotionNodeAlgorithm : public CMotionNodeAlgorithm
{
protected:

	CSkeletalCharacter *m_pCharacter;

	boost::shared_ptr<CKeyBind> m_pKeybind;

public:

	CCharacterMotionNodeAlgorithm() : m_pCharacter(NULL) {}

	void SetSkeletalCharacter( CSkeletalCharacter *pCharacter ) { m_pCharacter = pCharacter; }

	void SetKeyBind( boost::shared_ptr<CKeyBind>& pKeybind ) { m_pKeybind = pKeybind; }

	inline bool HandleInput( const SInputData& input );

	virtual bool HandleInput( const SInputData& input, int action_code );
};


inline bool CCharacterMotionNodeAlgorithm::HandleInput( const SInputData& input )
{
	if( !m_pKeybind )
		return false;

	int action_code = m_pKeybind->GetActionCode( input.iGICode );

	if( action_code == ACTION_NOT_ASSIGNED )
		return false;

	return HandleInput( input, action_code );
}



class CFwdMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );
};



class CJumpMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );
};



class CStandingMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );
};



#endif /* __SkeletalCharacter_HPP__ */
