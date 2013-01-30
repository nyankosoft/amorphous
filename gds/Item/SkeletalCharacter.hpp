#ifndef  __SkeletalCharacter_HPP__
#define  __SkeletalCharacter_HPP__


#include "gds/Item/GameItem.hpp"
#include "gds/GameCommon/KeyBind.hpp"
#include "gds/GameCommon/3DActionCode.hpp"
#include "gds/MotionSynthesis/MotionFSM.hpp"
#include "gds/MotionSynthesis/SkeletalMeshTransform.hpp"
#include "gds/Physics/fwd.hpp"


namespace amorphous
{

using namespace msynth;


class CSkeletalCharacter;
class CCharacterMotionNodeAlgorithm;
class CClothing;
class CClothSystem;


class CSkeletalCharacterOperations// : public IArchiveObjectBase
{
	boost::weak_ptr<CSkeletalCharacter> m_pSkeletalCharacter;

protected:

	boost::weak_ptr<CSkeletalCharacter> GetSkeletalCharacter() { return m_pSkeletalCharacter; }

public:

	CSkeletalCharacterOperations() {}

	virtual ~CSkeletalCharacterOperations() {}

	virtual void Update( float dt ) {}

	virtual bool HandleInput( const SInputData& input_data, int action_code ) { return false; }

	friend class CSkeletalCharacter;
};


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
	boost::shared_ptr<msynth::CMotionFSMManager> m_pMotionFSMManager;

	boost::shared_ptr<CMeshContainerRenderMethod> m_pRenderMethod;

	boost::shared_ptr<msynth::CMotionFSM> m_pLowerLimbsMotionsFSM;

	float m_fFwdSpeed;
	float m_fTurnSpeed;

	/// The maximum speed at which the character can turn.
	/// Note that the character does not have to turn
	/// if it's already moving along the desired direction.
	float m_fMaxTurnSpeed;

	float m_fFloorHeight;

	bool m_FeetOnGround;

	boost::shared_ptr<msynth::CMotionFSMInputHandler> m_pMotionFSMInputHandler;

	boost::shared_ptr<CInputHandler> m_pInputHandler;

	boost::shared_ptr<CKeyBind> m_pKeyBind;

	std::vector< boost::shared_ptr<CCharacterMotionNodeAlgorithm> > m_pMotionNodes;

	Matrix34 m_PrevWorldPose;

	Matrix34 m_BasePose;

	std::vector<SPlane> m_Walls;

	boost::shared_ptr<msynth::CMotionPrimitive> m_pSkeletonSrcMotion;

	CTransformNodeMap m_RootTransformNodeMap;

	CActionCodeToGICodesMap m_ACtoGICs;

	std::vector< boost::shared_ptr<CClothing> > m_pClothes;

	boost::shared_ptr<CClothSystem> m_pClothSystem;

	/// The hierarchical transforms of the character's current pose updated in the last call of UpdateGraphics()
	CKeyframe m_CurrentInterpolatedKeyframe;

	std::vector< boost::shared_ptr<CGameItem> > m_pProperty;

	std::vector< boost::shared_ptr<CSkeletalCharacterOperations> > m_pOperations;

	bool m_CameraDependentMotionControl;
	
	Vector3 m_vDesiredHorizontalDirection;

//	GraphicsResourcesUpdateDelegate<CSkeletalCharacter> m_GraphicsUpdate;

private:

	float GetFloorHeight( CCopyEntity& entity, Vector3& impact_normal );

	void UpdateStepHeight( CCopyEntity& entity );

	void SetCharacterWorldPose( const Matrix34& world_pose, CCopyEntity& entity, physics::CActor &actor );

	inline boost::shared_ptr<SkeletalMesh> GetCharacterSkeletalMesh();

	void ClipMotion( CCopyEntity& entity );

public:

	CSkeletalCharacter();

	~CSkeletalCharacter() {}

	Result::Name InitMotionFSMs( const std::string& motion_fsm_file );

	Result::Name InitClothSystem();

	void InitInputHandler( int input_handler_index );

	void OnEntityCreated( CCopyEntity& entity );

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input_data );

	void OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity &other_entity, physics::CShape& other_shape, U32 trigger_flags );

	void OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity );

	void UpdateGraphics();

	Result::Name LoadCharacterMesh( const std::string& skeletal_mesh_pathname );

	float GetFwdSpeed() const { return m_fFwdSpeed; }
	float GetTurnSpeed() const { return m_fTurnSpeed; }
	float GetMaxTurnSpeed() const { return m_fMaxTurnSpeed; }

	void SetFwdSpeed( float fSpeed ) { m_fFwdSpeed = fSpeed; }
	void SetTurnSpeed( float fSpeed ) { m_fTurnSpeed = fSpeed; }
	void SetMaxTurnSpeed( float fSpeed ) { m_fMaxTurnSpeed = fSpeed; }

	void SetDesiredHorizontalDirection( Vector3 vHorizontalDirection ) { m_vDesiredHorizontalDirection = vHorizontalDirection; }

	void StartVerticalJump( const Vector3& velocity );

	void TurnIfNecessary( float dt, float turn_speed );

	const boost::shared_ptr<CKeyBind>& GetKeyBind() const { return m_pKeyBind; }

	void SetKeyBind( boost::shared_ptr<CKeyBind> pKeyBind );

	CInputState::Name GetActionInputState( int action_code, CKeyBind::ActionType action_type = CKeyBind::ACTION_TYPE_PRIMARY );

	void AddItem( boost::shared_ptr<CGameItem> pItem ) { m_pProperty.push_back( pItem ); }

	void AddItems( std::vector< boost::shared_ptr<CGameItem> >& pItems ) { if( pItems.empty() ) return; m_pProperty.insert( m_pProperty.end(), pItems.begin(), pItems.end() ); }

	void AddOperationsAlgorithm( boost::shared_ptr<CSkeletalCharacterOperations> pOperations );

	const std::vector< boost::shared_ptr<CGameItem> >& GetOwnedItems() const { return m_pProperty; }

	bool IsCameraDependentMotionControlEnabled() const { return m_CameraDependentMotionControl; }

	std::vector< boost::shared_ptr<CCharacterMotionNodeAlgorithm> >& MotionNodeAlgorithms() { return m_pMotionNodes; }

	void SetMotionNodeAlgorithm( const std::string& motion_node_name, boost::shared_ptr<CCharacterMotionNodeAlgorithm> pMotionNodeAlgorithm );

	static int ms_DefaultInputHandlerIndex;
};


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

//	boost::shared_ptr<CKeyBind> m_pKeybind;

	/// pair contains an input condition
	/// first: action code / second: input type (pressed, released or value changed)
	std::map<std::pair<int,int>,std::string> m_ActionInputsToMotionNodes;

protected:

	bool HandleInputForTransition( const SInputData& input, int action_code );

public:

	CCharacterMotionNodeAlgorithm() : m_pCharacter(NULL) {}

	virtual ~CCharacterMotionNodeAlgorithm() {}

	void SetSkeletalCharacter( CSkeletalCharacter *pCharacter ) { m_pCharacter = pCharacter; }

	const boost::shared_ptr<CKeyBind> GetKeyBind() const { return m_pCharacter ? m_pCharacter->GetKeyBind() : boost::shared_ptr<CKeyBind>(); }

//	void SetKeyBind( boost::shared_ptr<CKeyBind>& pKeybind ) { m_pKeybind = pKeybind; }

	inline bool HandleInput( const SInputData& input );

	virtual bool HandleInput( const SInputData& input, int action_code );

	CInputState::Name GetActionInputState( int action_code );

	void AddActionCodeToMotionNodeMap( int action_code, int input_type, const std::string& motion_node_name );
};


inline bool CCharacterMotionNodeAlgorithm::HandleInput( const SInputData& input )
{
	SInputData input_copy = input;

	const boost::shared_ptr<CKeyBind> pKeybind = GetKeyBind();
	if( !pKeybind )
		return false;

	if( input.iGICode == GIC_GPD_AXIS_Y )
		input_copy.fParam1 *= -1.0f;


	int action_code = pKeybind->GetActionCode( input.iGICode );

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
	boost::shared_ptr<CItemEntity> GetCharacterEntity();

public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );

	void EnterState();
};



class CStandingMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const SInputData& input, int action_code );
};


} // namespace amorphous



#endif /* __SkeletalCharacter_HPP__ */
