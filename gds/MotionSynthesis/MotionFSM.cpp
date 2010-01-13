#include "MotionFSM.hpp"
#include "MotionDatabase.hpp"
#include <map>

using namespace std;
using namespace boost;


namespace msynth
{

/*
----------------------------------------------------------
Motion Synthesis moduls
----------------------------------------------------------
Roles in stage
1. Calculate the world pose of the skeletal object which the client module wants to control
  with skeletal motions.
  - Client code needs to call update routines of motion synthesizer every frame
  - It is up to the client code whether to update to the calculated world pose.
    e.g., If updating to the calculated world pose causes collision to other objects,
	client code has to calculate the pose which stop the motion at the collision.
2. Calculate the keyframe of a specified time
  - Client code has to call this only when it needs positions of joints of skeletal object.
    - e.g., Rendering
  - If the client code needs all the positions of the character's body parts for accurate
    collision detection, keyframe needs to be calculated every frame
    - e.g., testing collisions of between character's hands/foots and obstacles around it, 


----------------------------------------------------------
XML data format
----------------------------------------------------------

<trans>
  <src name="forward"></src>
  <dest name="crouch"></dest>
    <path>
      <motion>crouch</motion>
      <interpolation><time>0.2</time></interpolation>
    </path>
</trans>
<trans>
  <src name="forward"></src>
  <dest name="stand"></dest>
    <path>
      <motion>stand</motion>
      <interpolation><time>0.2</time></interpolation>
    </path>
</trans>
<trans>
  <src name="forward"></src>
  <dest name="prone"></dest>
    <path>
      <motion>crouch</motion>
      <interpolation><time>0.2</time></interpolation>
    </path>
    <path>
      <motion>prone</motion>
      <interpolation><time>0.2</time></interpolation>
    </path>
</trans>
mpn  = ???;// Motion Primitive Node
mpn.AddTrans( "crouch", mt( "crouch", 0.2f) )
mpn.AddTrans( "prone",  mt( 0.2f, "crouch" ) & mt( 0.2f, "prone" ) )


----------------------------------------------------------
Uses of IK
----------------------------------------------------------

- Characer may aim different angles
- Angles shift continuously, so the system has to dynamically create motion primitives
  to shift from one aiming angle to another
- Is IK really necessary in this situation?
  - There may be some options.
  1) Use IK to calculate different angles
  2) Blend motion primitives to create target angle
  3) Combine 1) and 2)
- IK alone is probably not enough, since character's upper body poses may differ
  based on the angle
  - Aiming horizontally forward: bending forward
  - Aiming upward: not that bending forward


----------------------------------------------------------
Transitions and conditions
----------------------------------------------------------

- Some transitions have conditions
- e.g.
  - stand -> run:
    - Not possible if the character is aiming.
	- Force to "stand -> walk"?
- Which module should be responsible for forcing requested transition to another transition
  - ban run and aim or not -> game-specific settings
  - Have a higher-level module check and regulate transition
  - e.g.) If running and aiming is banned, and character started running while aiming,
    - Transition from aiming to holding.
	- Reject request to run.


----------------------------------------------------------
Motion primitive nodes and motion variations
----------------------------------------------------------

- Aiming motion has variations
  - aiming while standing
  - aiming while courching
  - aiming while prone
- Should these aiming motions represented by a single motion primitive node or different nodes?
  - At least, aiming while standing/crouching and aiming while prone should be separate nodes.


----------------------------------------------------------
Blending
----------------------------------------------------------
- Weight
  - When synthesizing motions for 'walking while aiming' upper body motion
    should completely overwrite walk motions.
    - Can skip the calculation of upper body nodes of walk motions
  - e.g.) When a character is aiming while walking, upper body motions
    of base walk motions are replaced with aiming pose
  - The pose of root node, however, should be determined by lower body motion
- Add CMotionPrimitive::m_UpdateRootPose?
  - fwd motion: true
  - aim motions: false
  - Aim motions can skip calculation of root node pose

- How the actual blending is done
  - preconditions: each keyframe has priority
    - keyframe with a higher priority are set first
  - 1) Create an empty keyframe with transform nodes that have same hierarchical structure with complete skeleton.
  - 2) Set keyframe with ...
  - inomplete motion primitive should store its entry point in order to avoid searching it every frame.


----------------------------------------------------------
Keyframe Calculation
----------------------------------------------------------
- When it is necessary
  - When graphics engine needs to render the skeletal mesh which is animated by the motion
    controlled by the current motion FSMs
- Preconditions
  - The motion players have been updated by CMotionPrimitiveBlender::Update(),
    and its queue is up to date
- Steps
  1) Calculate interpolated keyframe of each motion FSM
  2) Combine interpolated keyframes. Blend them if necessary.


----------------------------------------------------------
Saving to Disk
----------------------------------------------------------
- Where to store a complete skeleton structure?
- Need to create a file separate from motion database?


----------------------------------------------------------
Issues
----------------------------------------------------------

1) How to process successive state transitions
  - e.g. prone to forward: prone -> stand up -> forward
    - Needs to specify that 'forward' comes after 'stand up' before moving from 'prone' to 'stand up'
    - Transition may be interrupted by requests of a motion that has a higher priority,
      such as reations when the character is hit, or by player's input to cancel
	  the forward motion and go back to prone while the character is standing up.
option 1: represent state transition request as an array of states
- In the above example, the system passes array [ 'stand up', 'forward' ] as a state transition request to 'prone' state
  - 'prone' state takes and process the first request, 'stand up'
  - 'stand up' is handed the rest of array ['forward'] when the state starts,
    and start the state transition to ['forward']

Support different interpolation times between different motion primitives

text data file that describes transition rules

*/

class CMotionFSMCallback : public CMotionPrimitivePlayCallback
{
	CMotionFSM *m_pFSM;

public:

	CMotionFSMCallback( CMotionFSM *pFSM ) : m_pFSM(pFSM) {}
/*
	virtual void OnNewMotionPrimitiveStarted( boost::shared_ptr<CMotionPrimitiveNode>& pNew )
	{
	}
*/
	/// Called when a motion primitive is finished playing and a new primitive is being started playing
	void OnMotionPrimitiveChanged( boost::shared_ptr<CMotionPrimitive>& pPrev, boost::shared_ptr<CMotionPrimitive>& pNew )
	{
//		int num_nodes_to_process = (int)m_pFSM->m_vecpNodesToProcess.size();

		// Place all the necessary motion primitives for the current transition in the queue
		// in advance.
		//  - Difficult to randomly select motions of a node for variation
/*		int prev_id = pPrev->GetUserID();
		if( 0 <= prev_id )
		{
			m_pFSM->m_vecpNode[prev_id]->ExitState();
		}

		int new_id = pNew->GetUserID();
		if( 0 <= new_id )
		{
			m_pFSM->m_vecpNode[new_id]->EnterState();
		}

		m_pFSM->m_pCurrent = m_vecpNode[new_id];*/
	}

	void OnMotionPrimitiveFinished( boost::shared_ptr<CMotionPrimitive>& pPrev )
	{
/*		int num_nodes_to_process = (int)m_pFSM->m_vecpNodesToProcess.size();

		if( num_nodes_to_process == 0 )
			return;

		// copy the first element
		shared_ptr<CMotionPrimitiveNode> pMotionNode
			= m_pFSM->m_vecpNodesToProcess.front();

		// pop front
		m_pFSM->m_vecpNodesToProcess.erase( m_pFSM->m_vecpNodesToProcess.begin() );

		MotionNodeTrans trans;
		pMotionNode->StartMotion( trans.interpolation_time );*/

/*		if( m_pFSM->m_pCurrent
		 && m_pFSM->m_pCurrent->IsPlayingLoopedMotion() )
		{
		}*/

		if( !m_pFSM->m_pvecTransToProcess )
			return;

		m_pFSM->m_TransIndex++;

		vector<MotionNodeTrans>& tansitions_to_process = *(m_pFSM->m_pvecTransToProcess);
		if( m_pFSM->m_TransIndex < (int)tansitions_to_process.size() )
		{
			MotionNodeTrans& transition = tansitions_to_process[m_pFSM->m_TransIndex];
			transition.pNode->StartMotion( transition.interpolation_time );
		}
	}
};


void CMotionPrimitiveNode::SetFSM( CMotionFSM *pFSM )
{
	m_pFSM = pFSM;
	m_pBlender = m_pFSM->Player().get();
}


void CMotionPrimitiveNode::AddTransPath( const std::string& dest_motion_name, const mt& trans )
{
	if( !m_pFSM )
	{
		LOG_PRINT_ERROR( " No FSM" );
		return;
	}

	const vector< pair<double,string> >& src_trans = trans.GetTransitions();
/*
	// add a new transition pattern, get a non-const reference to it, and update it
	m_mapTrans[dest_motion_name] = vector<MotionNodeTrans>();
	name_trans_map::iterator itr = m_mapTrans.find( dest_motion_name );
	vector<MotionNodeTrans>& dest_transitions = itr->second;
	dest_transitions.resize( src_trans.size() );
*/
	shared_ptr< vector<MotionNodeTrans> > pvecTrans( new vector<MotionNodeTrans>() );
	vector<MotionNodeTrans>& dest_transitions = *pvecTrans;
	dest_transitions.resize( src_trans.size() );

	for( size_t i=0; i<src_trans.size(); i++ )
	{
		const float interpolation_time = (float)src_trans[i].first;
		const string& name = src_trans[i].second;
		dest_transitions[i] = MotionNodeTrans( interpolation_time, name );
		dest_transitions[i].pNode = m_pFSM->GetNode( name );

		if( !dest_transitions[i].pNode )
		{
			LOG_PRINT_ERROR( " A motion node for '" + name + "' was not found." );
			dest_transitions.resize( 0 );
			return;
		}
	}

	m_mapTrans[dest_motion_name] = pvecTrans;
}


void CMotionPrimitiveNode::RequestTransition( const std::string& dest_motion_name )
{
	name_trans_map::iterator itr = m_mapTrans.find( dest_motion_name );
	if( itr == m_mapTrans.end() )
	{
		return;
	}

	// 'm_mapTrans' has a transition path to the node with the name 'dest_motion_name'

/*	m_pFSM->ClearNodesToProcess();
	vector<MotionNodeTrans>& transitions = itr->second;
	for( size_t i=0; i<transitions.size(); i++ )
	{
		m_pFSM->AddNodeToProcess( transitions[i].pNode );
	}*/

	m_pFSM->StartNewTransitions( itr->second );

	// if found
	
//	ProcessRequest( itr->second, 0 );
}


void CMotionPrimitiveNode::LoadMotion( CMotionDatabase& db )
{
	if( 0 < m_MotionName.length() )
		m_pMotionPrimitive = db.GetMotionPrimitive( m_MotionName );

	if( m_pMotionPrimitive
	 && m_pMotionPrimitive->GetKeyframeBuffer().size() == 0 )
	{
		// Has no keyframe - an invalid motion primitive
		m_pMotionPrimitive.reset();
	}
}


void CMotionPrimitiveNode::SetStartBlendNode( boost::shared_ptr<CBlendNode> pRootBlendNode )
{
	if( m_pMotionPrimitive )
		m_pMotionPrimitive->SearchAndSetStartBlendNode( pRootBlendNode );
}


void CMotionPrimitiveNode::CalculateKeyframe()
{
	if( !m_pMotionPrimitive )
		return;

	// store the transform nodes of the current keyframe to blend node
	m_pFSM->Player()->CalculateKeyframe();
}


void CMotionPrimitiveNode::SetAlgorithm( boost::shared_ptr<CMotionNodeAlgorithm> pAlgorithm )
{
	m_pAlgorithm = pAlgorithm;
	m_pAlgorithm->m_pNode = this;
}



//===========================================================================
// CMotionFSM
//===========================================================================

CMotionFSM::CMotionFSM( const string& name )
:
m_Name(name),
m_TransIndex(0)
{
	m_pMotionPrimitivePlayer = shared_ptr<CMotionPrimitiveBlender>( new CMotionPrimitiveBlender() );

	// Set a callback to notify this FSM of changes in motion primitives
	// when the player finished playing one and moving on to another
	shared_ptr<CMotionPrimitivePlayCallback> pCallback( new CMotionFSMCallback(this) );
	m_pMotionPrimitivePlayer->SetCallback( pCallback );
	
}


CMotionFSM::~CMotionFSM()
{
	// Unregister callback - the callback will be a dangling pointer if this is not done
	// && m_pMotionPrimitivePlayer is held by other object.
	// m_pMotionPrimitivePlayer is not meant be shared by other objects, though.
	m_pMotionPrimitivePlayer->SetCallback( shared_ptr<CMotionPrimitivePlayCallback>() );
}


void CMotionFSM::StartMotion( const std::string& motion_node_name )
{
	name_motionnode_map::iterator itr
		= m_mapNameToMotionNode.find( motion_node_name );

	if( itr == m_mapNameToMotionNode.end() )
		return;

	m_pCurrent = itr->second;

	CMotionPrimitiveNode& node = *(itr->second);
	node.EnterState(  );

	float interpolation_motion_length = 0.1f;
	node.StartMotion( interpolation_motion_length );
}



/// Checks the motion curently being played.
void CMotionFSM::Update( float dt )
{
	m_pMotionPrimitivePlayer->Update( dt );

	if( m_pCurrent )
		m_pCurrent->GetAlgorithm()->Update( dt );

	// This will miss the motion primitive when it's too short or dt is too large
/*
	if( !m_pCurrent )
		return;

	// get the motion currently being played
	int id = -1;
	shared_ptr<CMotionPrimitive> pCurrentMotion = m_pMotionPrimitivePlayer->GetCurrentMotion();
	if( pCurrentMotion )
		id = pCurrentMotion->GetUserID();

	// motion -> id
	if( id == -1 )
		return;

	if( m_pCurrent->GetCurrentMotion() == pCurrentMotion )
	{
		m_pCurrent->Update( dt );
	}
	else
	{
		// The motion currently being played does not match the motion of the current state
		// - Change states
//		m_pCurrent->ExitState();
		m_vecpNode[id]->EnterState();
		m_vecpNode[id]->Update( dt );
		m_pCurrent = m_vecpNode[id];
	}*/
}

void CMotionFSM::ClearNodesToProcess()
{
//	m_vecpNodesToProcess.resize( 0 );
	m_pvecTransToProcess = shared_ptr< vector<MotionNodeTrans> >();
	m_TransIndex = 0;
}

void CMotionFSM::SetTransitions( shared_ptr< vector<MotionNodeTrans> > pvecTrans )
{
	m_pvecTransToProcess = pvecTrans;
	m_TransIndex = 0;
}


void CMotionFSM::AddNode( shared_ptr<CMotionPrimitiveNode> pNode )
{
	pNode->SetFSM( this );
	m_mapNameToMotionNode[pNode->GetName()] = pNode;
}


shared_ptr<CMotionPrimitiveNode> CMotionFSM::AddNode( const string& node_name )
{
	shared_ptr<CMotionPrimitiveNode> pNode( new CMotionPrimitiveNode(node_name) );
	AddNode( pNode );
	return pNode;
}


void CMotionFSM::CalculateKeyframe()
{
	if( !m_pCurrent )
		return;

	m_pCurrent->CalculateKeyframe();
}


void CMotionFSM::LoadMotions( CMotionDatabase& db )
{
	name_motionnode_map::iterator itr;
	for( itr = m_mapNameToMotionNode.begin();
		 itr != m_mapNameToMotionNode.end();
		 itr++ )
	{
		itr->second->LoadMotion( db );
	}
}


void CMotionFSM::LoadMotions()
{
	CMotionDatabase db;
	bool db_ready = db.LoadFromFile( m_MotionDatabaseFilepath );
	if( !db_ready )
		return;

	LoadMotions( db );
}


void CMotionFSM::SetStartBlendNodeToMotionPrimitives( shared_ptr<CBlendNode> pRootBlendNode )
{
	name_motionnode_map::iterator itr;
	for( itr = m_mapNameToMotionNode.begin();
		 itr != m_mapNameToMotionNode.end();
		 itr++ )
	{
		itr->second->SetStartBlendNode( pRootBlendNode );
	}
}


//=======================================================================================
// CMotionGraphManager
//=======================================================================================

CMotionGraphManager::CMotionGraphManager()
{
	m_pBlendNodeRoot = shared_ptr<CBlendNode>( new CBlendNode );
	m_pBlendNodeRoot->SetSelf( m_pBlendNodeRoot );
}


void CMotionGraphManager::LoadMotions( CMotionDatabase& mdb )
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->LoadMotions( mdb );

	SetStartBlendNodeToMotionPrimitives();

//	shared_ptr<CMotionPrimitive> pMotion = mdb.GetMotionPrimitive( m_CompleteSkeletonName );
//	if( pMotion )
//		m_pBlendNodeRoot->CreateFromSkeleton( pMotion->GetSkeleton()->GetRootBone() );
}


void CMotionGraphManager::InitForTest( const string& motion_db_filepath )
{
	shared_ptr<CMotionFSM> pFSM( new CMotionFSM("lower_limbs") );

	AddFSM( pFSM );

	pFSM->SetMotionDatabaseFilepath( motion_db_filepath );

	shared_ptr<CMotionPrimitiveNode> pNodes[16];
	pNodes[0] = pFSM->AddNode("fwd");
	pNodes[1] = pFSM->AddNode("standing");
	pNodes[2] = pFSM->AddNode("crouch");
	pNodes[3] = pFSM->AddNode("prone");
	pNodes[4] = pFSM->AddNode("bwd");
	pNodes[5] = pFSM->AddNode("sidestep-right");
	pNodes[6] = pFSM->AddNode("sidestep-left");
	pNodes[7] = pFSM->AddNode("crouch-sidestep-right");
	pNodes[8] = pFSM->AddNode("crouch-sidestep-left");

	pNodes[0]->SetMotionName( "walk-legs" );

	// moving forward (walk/run)
	pNodes[0]->AddTransPath( "standing", mt( 0.1, "stand" ) );
	pNodes[0]->AddTransPath( "crouch",   mt( 0.1, "crouch" ) );

	// standing
	pNodes[1]->AddTransPath( "fwd",      mt( 0.1, "fwd" ) );
	pNodes[1]->AddTransPath( "crouch",   mt( 0.1, "crouch" ) );

	// crouching
	pNodes[2]->AddTransPath( "fwd",      mt( 0.1, "fwd" ) ); // move forward while crouching
	pNodes[2]->AddTransPath( "standing", mt( 0.1, "stand" ) );
	pNodes[2]->AddTransPath( "prone",    mt( 0.1, "prone" ) );

	// prone
	pNodes[3]->AddTransPath( "fwd",      mt( 0.2, "crouch" ) & mt( 0.1, "fwd" ) ); // move forward while crouching
	pNodes[3]->AddTransPath( "standing", mt( 0.1, "stand" ) );
	pNodes[3]->AddTransPath( "crouch",   mt( 0.1, "crouch" ) );

	CMotionDatabase mdb( motion_db_filepath );
	m_pBlendNodeRoot->CreateFromSkeleton( mdb.GetMotionPrimitive( "standing" )->GetSkeleton()->GetRootBone() );

	SetMotionDatabaseFilepath( motion_db_filepath );
	// mdb is already open and loaded, so call LoadMotions() to load motions from it
	// rather than open the file again
	LoadMotions( mdb );
/*
	pFSM->AddNode( "fwd",  );
	pFSM->AddNode( "stand" );
	pFSM->AddNode( "fwd-crouching" );
	pFSM->AddNode( "sidestep-right" );
	pFSM->AddNode( "sidestep-left" );
	pFSM->AddNode( "prone" );
	pFSM->AddNode( "fwd-prone" );
	pFSM->AddNode( "supine" );
	pFSM->AddNode( "fwd-supine" );
*/
}


void CMotionGraphManager::LoadFromDatabase()
{
	CMotionDatabase db;
	bool db_ready = db.LoadFromFile( m_MotionDatabaseFilepath );
	if( !db_ready )
		return;

	LoadMotions( db );

	LOG_PRINT_WARNING( "Need to initialize m_pBlendNodeRoot with a complete skeleton" );

//	shared_ptr<CSkeleton> pSkeleton;// = db.GetSkeleton( m_CompleteSkeletonName );
//	m_pBlendNodeRoot->CreateFromSkeleton( pSkeleton->GetRootBone() );
}



} // namespace msynth
