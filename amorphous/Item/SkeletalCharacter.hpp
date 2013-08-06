#ifndef  __SkeletalCharacter_HPP__
#define  __SkeletalCharacter_HPP__


#include "amorphous/Item/GameItem.hpp"
#include "amorphous/GameCommon/KeyBind.hpp"
#include "amorphous/GameCommon/3DActionCode.hpp"
#include "amorphous/MotionSynthesis/MotionFSM.hpp"
#include "amorphous/MotionSynthesis/SkeletalMeshTransform.hpp"
#include "amorphous/Physics/fwd.hpp"


namespace amorphous
{

using namespace msynth;


class SkeletalCharacter;
class CCharacterMotionNodeAlgorithm;
class Clothing;
class CClothSystem;


class SkeletalCharacterOperations// : public IArchiveObjectBase
{
	boost::weak_ptr<SkeletalCharacter> m_pSkeletalCharacter;

protected:

	boost::weak_ptr<SkeletalCharacter> GetSkeletalCharacter() { return m_pSkeletalCharacter; }

public:

	SkeletalCharacterOperations() {}

	virtual ~SkeletalCharacterOperations() {}

	virtual void Update( float dt ) {}

	virtual bool HandleInput( const InputData& input_data, int action_code ) { return false; }

	friend class SkeletalCharacter;
};


class CActionCodeToGICodesMap
{
public:
	/// Why vector?
	/// - Because each action code of an action type can have one OR MORE general input codes.
	std::map<int, std::vector<int> > m_mapActionCodeToGICodes[KeyBind::NUM_ACTION_TYPES];
};


/**
  walk, turn
  - needs to be polling operation?
  - rationale: speed of walking and turning is determined by analog input.


*/
class SkeletalCharacter : public GameItem
{
	boost::shared_ptr<msynth::MotionFSMManager> m_pMotionFSMManager;

	boost::shared_ptr<MeshContainerRenderMethod> m_pRenderMethod;

	boost::shared_ptr<msynth::MotionFSM> m_pLowerLimbsMotionsFSM;

	float m_fFwdSpeed;
	float m_fTurnSpeed;

	/// The maximum speed at which the character can turn.
	/// Note that the character does not have to turn
	/// if it's already moving along the desired direction.
	float m_fMaxTurnSpeed;

	float m_fFloorHeight;

	bool m_FeetOnGround;

	boost::shared_ptr<msynth::MotionFSMInputHandler> m_pMotionFSMInputHandler;

	boost::shared_ptr<InputHandler> m_pInputHandler;

	boost::shared_ptr<KeyBind> m_pKeyBind;

	std::vector< boost::shared_ptr<CCharacterMotionNodeAlgorithm> > m_pMotionNodes;

	Matrix34 m_PrevWorldPose;

	Matrix34 m_BasePose;

	std::vector<SPlane> m_Walls;

	boost::shared_ptr<msynth::MotionPrimitive> m_pSkeletonSrcMotion;

	TransformNodeMap m_RootTransformNodeMap;

	CActionCodeToGICodesMap m_ACtoGICs;

	std::vector< boost::shared_ptr<Clothing> > m_pClothes;

	boost::shared_ptr<CClothSystem> m_pClothSystem;

	/// The hierarchical transforms of the character's current pose updated in the last call of UpdateGraphics()
	Keyframe m_CurrentInterpolatedKeyframe;

	std::vector< boost::shared_ptr<GameItem> > m_pProperty;

	std::vector< boost::shared_ptr<SkeletalCharacterOperations> > m_pOperations;

	bool m_CameraDependentMotionControl;
	
	Vector3 m_vDesiredHorizontalDirection;

//	GraphicsResourcesUpdateDelegate<SkeletalCharacter> m_GraphicsUpdate;

private:

	float GetFloorHeight( CCopyEntity& entity, Vector3& impact_normal );

	void UpdateStepHeight( CCopyEntity& entity );

	void SetCharacterWorldPose( const Matrix34& world_pose, CCopyEntity& entity, physics::CActor &actor );

	inline boost::shared_ptr<SkeletalMesh> GetCharacterSkeletalMesh();

	void ClipMotion( CCopyEntity& entity );

public:

	SkeletalCharacter();

	~SkeletalCharacter() {}

	Result::Name InitMotionFSMs( const std::string& motion_fsm_file );

	Result::Name InitClothSystem();

	void InitInputHandler( int input_handler_index );

	void OnEntityCreated( CCopyEntity& entity );

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input_data );

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

	const boost::shared_ptr<KeyBind>& GetKeyBind() const { return m_pKeyBind; }

	void SetKeyBind( boost::shared_ptr<KeyBind> pKeyBind );

	CInputState::Name GetActionInputState( int action_code, KeyBind::ActionType action_type = KeyBind::ACTION_TYPE_PRIMARY );

	void AddItem( boost::shared_ptr<GameItem> pItem ) { m_pProperty.push_back( pItem ); }

	void AddItems( std::vector< boost::shared_ptr<GameItem> >& pItems ) { if( pItems.empty() ) return; m_pProperty.insert( m_pProperty.end(), pItems.begin(), pItems.end() ); }

	void AddOperationsAlgorithm( boost::shared_ptr<SkeletalCharacterOperations> pOperations );

	const std::vector< boost::shared_ptr<GameItem> >& GetOwnedItems() const { return m_pProperty; }

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
	bool HandleInput( const InputData& input ) { return false; }
};



class CCharacterMotionNodeAlgorithm : public MotionNodeAlgorithm
{
protected:

	SkeletalCharacter *m_pCharacter;

//	boost::shared_ptr<KeyBind> m_pKeybind;

	/// pair contains an input condition
	/// first: action code / second: input type (pressed, released or value changed)
	std::map<std::pair<int,int>,std::string> m_ActionInputsToMotionNodes;

protected:

	bool HandleInputForTransition( const InputData& input, int action_code );

public:

	CCharacterMotionNodeAlgorithm() : m_pCharacter(NULL) {}

	virtual ~CCharacterMotionNodeAlgorithm() {}

	void SetSkeletalCharacter( SkeletalCharacter *pCharacter ) { m_pCharacter = pCharacter; }

	const boost::shared_ptr<KeyBind> GetKeyBind() const { return m_pCharacter ? m_pCharacter->GetKeyBind() : boost::shared_ptr<KeyBind>(); }

//	void SetKeyBind( boost::shared_ptr<KeyBind>& pKeybind ) { m_pKeybind = pKeybind; }

	inline bool HandleInput( const InputData& input );

	virtual bool HandleInput( const InputData& input, int action_code );

	CInputState::Name GetActionInputState( int action_code );

	void AddActionCodeToMotionNodeMap( int action_code, int input_type, const std::string& motion_node_name );
};


inline bool CCharacterMotionNodeAlgorithm::HandleInput( const InputData& input )
{
	InputData input_copy = input;

	const boost::shared_ptr<KeyBind> pKeybind = GetKeyBind();
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

	bool HandleInput( const InputData& input, int action_code );

	void EnterState();
};


class CRunMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const InputData& input, int action_code );

	void EnterState();
};


class CJumpMotionNode : public CCharacterMotionNodeAlgorithm
{
	boost::shared_ptr<ItemEntity> GetCharacterEntity();

public:

	void Update( float dt );

	bool HandleInput( const InputData& input, int action_code );

	void EnterState();
};



class CStandingMotionNode : public CCharacterMotionNodeAlgorithm
{
public:

	void Update( float dt );

	bool HandleInput( const InputData& input, int action_code );
};


} // namespace amorphous



#endif /* __SkeletalCharacter_HPP__ */
