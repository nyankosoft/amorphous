#ifndef  __MotionFSM_HPP__
#define  __MotionFSM_HPP__


#include <map>
#include <gds/MotionSynthesis/fwd.hpp>
#include <gds/MotionSynthesis/MotionPrimitive.hpp>
#include <gds/MotionSynthesis/MotionPrimitiveBlender.hpp>
#include <gds/MotionSynthesis/BlendNode.hpp>


namespace msynth
{

// forward declarations of private classes
class CMotionFSMCallback;


/// motion transition (used for setup of motion primitive nodes)
class mt
{
	std::vector< std::pair<double,std::string> > m_transitions;

public:

	mt( double interpolation_duration_in_sec, const std::string& motion_name )
	{
		m_transitions.push_back( std::pair<double,std::string>( interpolation_duration_in_sec, motion_name ) );
	}

	const mt& operator&( const mt& rhs )
	{
		m_transitions.insert( m_transitions.end(), rhs.m_transitions.begin(), rhs.m_transitions.end() );
//		m_transitions.push_back( std::pair<double,std::string>( rhs.interpolation_duration_in_sec, rhs.motion_name ) );
		return *this;
	}

	const std::vector< std::pair<double,std::string> >& GetTransitions() const { return m_transitions; }
};


/// motion transition
class MotionNodeTrans
{
public:

	float interpolation_time;
	std::string name; ///< name of a CMotionPrimitiveNode object, not the name of motion primitive stored in a CMotionPrimitiveNode object.
	boost::shared_ptr<CMotionPrimitiveNode> pNode;

public:

	MotionNodeTrans( float _interpolation_time = 0.0f, const std::string& _name = "")
		:
	interpolation_time(_interpolation_time),
	name(_name)
	{
	}
};



/// Defines how the root node of a character move in differnt motion primitives
/// Also control skeletal animations of motion primitives
class CMotionPrimitiveNode
{
	std::string m_Name;

	std::string m_MotionName;

	boost::shared_ptr<CMotionPrimitive> m_pMotionPrimitive; /// single motion primitive

//	std::vector< boost::shared_ptr<CMotionPrimitive> > m_vecpMotionPrimitive; /// holds multiple motion primitives to represent randomness

//	typedef std::map< std::string, std::vector<MotionNodeTrans> > name_trans_map;
	typedef std::map< std::string, boost::shared_ptr< std::vector<MotionNodeTrans> > > name_trans_map;

	// first == second.back().name
	name_trans_map m_mapTrans;

//	boost::shared_ptr< std::vector<MotionNodeTrans> > m_pTransToProcess;

	/// borrowed reference
	CMotionFSM *m_pFSM;

	/// borrowed reference
	CMotionPrimitiveBlender *m_pBlender;

public:

	CMotionPrimitiveNode( const std::string& name )
		:
	m_Name(name),
	m_pFSM(NULL)
	{
	}

/*	CMotionPrimitiveNode( CMotionFSM *pFSM = NULL )
		:
	m_pFSM(pFSM),
	m_pBlender(NULL)
	{}*/

	virtual ~CMotionPrimitiveNode() {}

	const std::string& GetName() { return m_Name; }

	void SetFSM( CMotionFSM *pFSM );

	// steering behavior
	// - forward(walking/running): change the orientation
	// - crouching/standing: play motion primitives for turning while standing/crouching
	// - jump: ignore?
//	virtual void TurnBy( float angle_in_degree ) {}

	// add transition to another node
	// - Client code may specify intermidiate nodes that needs to be covered before reaching the destination node
	void AddTransPath( const std::string& dest_motion_name, const mt& trans );

	virtual void EnterState( /*vector<MotionNodeTrans>& reqs*/ )
	{
//		if( 
	}

	virtual void ExitState()
	{
	}
/*
	void ProcessRequest( boost::shared_ptr< std::vector<CMotionTransRequest> > pReq, int index )
	{
		if( !pReq || pReq->empty() )
			return;

		// Add interpolation motion and motion primitive to the queue
		m_pBlender->StartTransition( m_pMotionPrimitive, GetInterpolationTime(prev_motion) );

//		req.pop_front();

		if( !req.empty() )
		{
			ChangeStateTo( req.state, req, index + 1 );
		}
	}
*/
	virtual void RequestTransition( const std::string& dest_motion_name );


	/// \param interpolation_time duration of interpolation motion playged
	///        before starting this motion
	void StartMotion( float interpolation_time )
	{
		m_pBlender->StartNewMotionPrimitive( m_pMotionPrimitive );
//		m_pBlender->AddNewMotion
	}

	void Update( float dt )
	{
/*		// 
		if( m_pSFM->Blender()->IsPlayingMotion( m_pMotionPrimitive ) )
		{
			// playing this motion
		}
		else
			m_pTransToProcess->*/
	}

	void LoadMotion( CMotionDatabase& db );

	void SetStartBlendNode( boost::shared_ptr<CBlendNode> pRootBlendNode );

	void CalculateKeyframe();
};


class CForward : public CMotionPrimitiveNode
{
public:

//	void TurnBy( float angle_in_degree )
//	{
//	}
};


class CStand : public CMotionPrimitiveNode
{
public:
};


class CCrouch : public CMotionPrimitiveNode
{
public:
};


class CStanding : public CMotionPrimitiveNode
{
public:
};


class CCrouching : public CMotionPrimitiveNode
{
public:
};


class CProne : public CMotionPrimitiveNode
{
public:
};




class CJump : public CMotionPrimitiveNode
{
public:
};



class CMotionFSM
{
	std::string m_Name;

	typedef std::map< std::string, boost::shared_ptr<CMotionPrimitiveNode> > name_motionnode_map;

	name_motionnode_map m_mapNameToMotionNode;

	boost::shared_ptr<CMotionPrimitiveNode> m_pCurrent;

	/// An array of motion primitives nodes are placed here when transition is requested.
	/// - Stores the incoming transitions in motion primitive graph
//	std::vector< boost::shared_ptr<CMotionPrimitiveNode> > m_vecpNodesToProcess;

	int m_TransIndex;

	boost::shared_ptr< std::vector<MotionNodeTrans> > m_pvecTransToProcess;

	boost::shared_ptr<CMotionPrimitiveBlender> m_pMotionPrimitivePlayer;

	std::string m_MotionDatabaseFilepath;

public:

	CMotionFSM( const std::string& name = "" );

	CMotionFSM::~CMotionFSM();

	const std::string& GetName() const { return m_Name; }

	void StartMotion( const std::string& motion_node_name );

	void Update( float dt );

	void RequestTransition( const std::string& dest_motion_name )
	{
		if( m_pCurrent )
			m_pCurrent->RequestTransition( dest_motion_name );
	}

	void ClearNodesToProcess();

	void SetTransitions( boost::shared_ptr< std::vector<MotionNodeTrans> > pvecTrans );

//	void AddNodeToProcess( boost::shared_ptr<CMotionPrimitiveNode> pNode ) { m_vecpNodesToProcess.push_back( pNode ); }

	void AddNode( boost::shared_ptr<CMotionPrimitiveNode> pNode );

	boost::shared_ptr<CMotionPrimitiveNode> AddNode( const std::string& node_name );

	boost::shared_ptr<CMotionPrimitiveNode> GetNode( const std::string& name )
	{
		name_motionnode_map::iterator itr = m_mapNameToMotionNode.find( name );
		return itr != m_mapNameToMotionNode.end() ? itr->second : boost::shared_ptr<CMotionPrimitiveNode>();
	}

	boost::shared_ptr<CMotionPrimitiveBlender>& Player() { return m_pMotionPrimitivePlayer; }

	void LoadMotions( CMotionDatabase& db );

	void LoadMotions();

	void SetMotionDatabaseFilepath( const std::string& filepath ) { m_MotionDatabaseFilepath = filepath; }

	void SetStartBlendNodeToMotionPrimitives( boost::shared_ptr<CBlendNode> pRootBlendNode );

	// Calculate the current keyframe of the currently played motion primitive,
	// and store the result to blend node tree
	void CalculateKeyframe();

	friend class CMotionFSMCallback;
};


class CMotionGraphManager
{
	std::vector< boost::shared_ptr<CMotionFSM> > m_vecpMotionFSM;

	/// Stores the root node of the blend node tree
	boost::shared_ptr<CBlendNode> m_pBlendNodeRoot;

	// Name of complete skeleton used by this motion synthesizer.
	// The blend node tree is created from the skeleton.
	std::string m_CompleteSkeletonName;

	std::string m_MotionDatabaseFilepath;

public:

	void LoadFromDatabase();

	void AddFSM( boost::shared_ptr<CMotionFSM> pFSM )
	{
		m_vecpMotionFSM.push_back( pFSM );
	}

	boost::shared_ptr<CMotionFSM> GetMotionFSM( const std::string& name )
	{
		for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		{
			if( name == m_vecpMotionFSM[i]->GetName() )
				return m_vecpMotionFSM[i];
		}

		return boost::shared_ptr<CMotionFSM>();
	}

	inline void Update( float dt );

	inline void CalculateKeyframe();

	inline void GetCurrentKeyframe( CKeyframe& dest );

	void SetMotionDatabaseFilepath( const std::string& filepath ) { m_MotionDatabaseFilepath = filepath; }

	inline void SetStartBlendNodeToMotionPrimitives();

	void InitForTest( const std::string& motion_db_filepath = "motions/default.mdb" );
};


//================================ inline implementations ================================

inline void CMotionGraphManager::Update( float dt )
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->Update( dt );
}


/// Combine different keyframes of motions currenltly being played in each FSM
/// Store the resuts to ???
inline void CMotionGraphManager::CalculateKeyframe()
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->CalculateKeyframe();

	// m_pBlendNodeRoot stores transforms for the current keyframe
}


inline void CMotionGraphManager::GetCurrentKeyframe( CKeyframe& dest )
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
	{
		// Calculate keyframe and store it to blend node tree, m_pBlendNodeRoot
		m_vecpMotionFSM[i]->CalculateKeyframe();
	}

	// Combine the keyframes of motion primitives
	m_pBlendNodeRoot->GetKeyframe( dest );
}


inline void CMotionGraphManager::SetStartBlendNodeToMotionPrimitives()
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->SetStartBlendNodeToMotionPrimitives( m_pBlendNodeRoot );
}

} // msynth


#endif /* __MotionFSM_HPP__ */
