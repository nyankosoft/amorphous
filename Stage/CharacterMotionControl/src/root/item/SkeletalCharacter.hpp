#ifndef  __SkeletalCharacter_HPP__
#define  __SkeletalCharacter_HPP__


#include "gds/Item/GameItem.hpp"
#include "gds/GameCommon/KeyBind.hpp"
#include "gds/GameCommon/3DActionCode.hpp"
#include "gds/MotionSynthesis/MotionFSM.hpp"
#include "gds/Physics/fwd.hpp"

using namespace msynth;


class CSkeletalCharacter;
class CCharacterMotionNodeAlgorithm;


class CActionCodeToGICodesMap
{
public:
	/// Why vector?
	/// - Because each action code of an action type can have one OR MORE general input codes.
	std::map<int, std::vector<int> > m_mapActionCodeToGICodes[CKeyBind::NUM_ACTION_TYPES];
};


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

	float m_fFloorHeight;

	bool m_FeetOnGround;

	boost::shared_ptr<msynth::CMotionFSMInputHandler> m_pInputHandler;

	boost::shared_ptr<CKeyBind> m_pKeyBind;

	std::vector< boost::shared_ptr<CCharacterMotionNodeAlgorithm> > m_pMotionNodes;

	Matrix34 m_PrevWorldPose;

	Matrix34 m_BasePose;

	std::vector<SPlane> m_Walls;

	boost::shared_ptr<msynth::CMotionPrimitive> m_pSkeletonSrcMotion;

	CActionCodeToGICodesMap m_ACtoGICs;

//	std::vector< boost::shared_ptr<CClothing> > m_pClothings;

//	boost::shared_ptr<CClothSystem> m_pClothSystem;
private:

	void UpdateStepHeight( CCopyEntity& entity );

	void SetCharacterWorldPose( const Matrix34& world_pose, CCopyEntity& entity, physics::CActor &actor );

public:

	CSkeletalCharacter();

	~CSkeletalCharacter() {}

//	void InitResources();

	void OnEntityCreated( CCopyEntity& entity );

	void Update( float dt );

	void Render();

	void ProcessInput( const SInputData& input, int action_code );

	void OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity &other_entity, physics::CShape& other_shape, U32 trigger_flags );

	void OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity );

	float GetFwdSpeed() const { return m_fFwdSpeed; }
	float GetTurnSpeed() const { return m_fTurnSpeed; }

	void SetFwdSpeed( float fSpeed ) { m_fFwdSpeed = fSpeed; }
	void SetTurnSpeed( float fSpeed ) { m_fTurnSpeed = fSpeed; }

	void SetKeyBind( boost::shared_ptr<CKeyBind> pKeyBind );

	CInputState::Name GetActionInputState( int action_code, CKeyBind::ActionType action_type = CKeyBind::ACTION_TYPE_PRIMARY );
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

	CInputState::Name GetActionInputState( int action_code );
};


inline bool CCharacterMotionNodeAlgorithm::HandleInput( const SInputData& input )
{
	SInputData input_copy = input;

	if( !m_pKeybind )
		return false;

	if( input.iGICode == GIC_GPD_AXIS_Y )
		input_copy.fParam1 *= -1.0f;


	int action_code = m_pKeybind->GetActionCode( input.iGICode );

	if( action_code == ACTION_NOT_ASSIGNED )
		return false;

	return HandleInput( input_copy, action_code );
}



class CFwdMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );

	void EnterState();
};


class CRunMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );

	void EnterState();
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
