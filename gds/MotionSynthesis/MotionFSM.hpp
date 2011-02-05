#ifndef  __MotionFSM_HPP__
#define  __MotionFSM_HPP__


#include <map>
#include "gds/MotionSynthesis/fwd.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp"
#include "gds/MotionSynthesis/MotionPrimitiveBlender.hpp"
#include "gds/MotionSynthesis/BlendNode.hpp"
#include "gds/Input/InputHandler.hpp"
#include "gds/XML/fwd.hpp"
#include "Support/Serialization/Serialization.hpp"
using namespace GameLib1::Serialization;


namespace msynth
{

// forward declarations of private classes
class CMotionFSMCallback;
class CMotionNodeAlgorithm;


/// motion transition (used for setup of motion primitive nodes)
class mt
{
	std::vector< std::pair<double,std::string> > m_transitions;

public:

	mt() {}

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
class MotionNodeTrans : public IArchiveObjectBase
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

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & interpolation_time;
		ar & name;

		if( ar.GetMode() == IArchive::MODE_INPUT )
			pNode.reset();
	}
};



/// Defines how the root node of a character move in differnt motion primitives
/// Also control skeletal animations of motion primitives
class CMotionPrimitiveNode : public IArchiveObjectBase
{
	std::string m_Name;

	std::string m_MotionName;

	boost::shared_ptr<CMotionPrimitive> m_pMotionPrimitive; /// single motion primitive

//	std::vector< boost::shared_ptr<CMotionPrimitive> > m_vecpMotionPrimitive; /// holds multiple motion primitives to represent randomness

//	typedef std::map< std::string, std::vector<MotionNodeTrans> > name_trans_map;
	typedef std::map< std::string, boost::shared_ptr< std::vector<MotionNodeTrans> > > name_trans_map;

	/// first == second.back().name
	name_trans_map m_mapTrans;

//	boost::shared_ptr< std::vector<MotionNodeTrans> > m_pTransToProcess;

	/// borrowed reference
	CMotionFSM *m_pFSM;

	/// borrowed reference
	CMotionPrimitiveBlender *m_pBlender;

	/// algorithm for state transitions
	boost::shared_ptr<CMotionNodeAlgorithm> m_pAlgorithm;

	float m_fMotionPlaySpeedFactor;

	float m_fExtraSpeedFactor;

public:

	CMotionPrimitiveNode( const std::string& name = "" );

/*	CMotionPrimitiveNode( CMotionFSM *pFSM = NULL )
		:
	m_pFSM(pFSM),
	m_pBlender(NULL)
	{}*/

	virtual ~CMotionPrimitiveNode() {}

	const std::string& GetName() const { return m_Name; }

	void SetFSM( CMotionFSM *pFSM );

	void SetMotionName( const std::string& motion_name ) { m_MotionName = motion_name; }

	// steering behavior
	// - forward(walking/running): change the orientation
	// - crouching/standing: play motion primitives for turning while standing/crouching
	// - jump: ignore?
//	virtual void TurnBy( float angle_in_degree ) {}

	// add transition to another node
	// - Client code may specify intermidiate nodes that needs to be covered before reaching the destination node
	void AddTransPath( const std::string& dest_motion_name, const mt& trans );
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

	virtual void EnterState( /*vector<MotionNodeTrans>& reqs*/ );

	virtual void ExitState();


	/// \param interpolation_time duration of interpolation motion played
	///        before starting this motion
	void StartMotion( float interpolation_time )
	{
		LOG_PRINT( fmt_string( "interpolation time: %f / motion: %s", interpolation_time, m_pMotionPrimitive ? m_pMotionPrimitive->GetName().c_str() : "-" ) );

		m_pBlender->StartNewMotionPrimitive( interpolation_time, m_pMotionPrimitive );
	}

	void AddMotion( float interpolation_time )
	{
		LOG_PRINT( fmt_string( "interpolation time: %f / motion: %s", interpolation_time, m_pMotionPrimitive ? m_pMotionPrimitive->GetName().c_str() : "-" ) );

		m_pBlender->AddMotionPrimitive( interpolation_time, m_pMotionPrimitive, 0 );
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

	void SetAlgorithm( boost::shared_ptr<CMotionNodeAlgorithm> pAlgorithm );

	boost::shared_ptr<CMotionNodeAlgorithm>& GetAlgorithm() { return m_pAlgorithm; }

	float GetMotionPlaySpeedFactor() const { return m_fMotionPlaySpeedFactor; }

	void SetMotionPlaySpeedFactor( float factor ) { m_fMotionPlaySpeedFactor = factor; }

	float GetExtraSpeedFactor() const { return m_fExtraSpeedFactor; }

	void SetExtraSpeedFactor( float factor ) { m_fExtraSpeedFactor = factor; }

	boost::shared_ptr<CMotionPrimitive>& MotionPrimitive() { return m_pMotionPrimitive; }

	void LoadFromXMLDocument( CXMLNodeReader& node );

	void Serialize( IArchive& ar, const unsigned int version );
};


/**
 Base class of motion node algorithm
 - Also used to create instances of null objects.
*/
class CMotionNodeAlgorithm
{
protected:

	CMotionPrimitiveNode *m_pNode;

public:

	CMotionNodeAlgorithm() : m_pNode(NULL) {}

	virtual ~CMotionNodeAlgorithm() {}

	virtual void Update( float dt ) {}

	/// Returns true if the input was handled.
	/// - Called if m_pNode is the current motion node of the motion FSM that owns m_pNode.
	virtual bool HandleInput( const SInputData& input ) { return false; }

	void RequestTransition( const std::string& dest_motion_name )
	{
		if( m_pNode )
			m_pNode->RequestTransition( dest_motion_name );
	}

	virtual void EnterState() {}

	virtual void ExitState() {}

	friend class CMotionPrimitiveNode;
};



class CMotionFSM : public IArchiveObjectBase
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

	~CMotionFSM();

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

	void StartNewTransitions( boost::shared_ptr< std::vector<MotionNodeTrans> >& pvecTrans )
	{
		ClearNodesToProcess();
		SetTransitions( pvecTrans );
		StartNextMotion();
	}

	// Start playing the motion managed by the motion node indexed by m_TransIndex
	void StartNextMotion();

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

	void HandleInput( const SInputData& input )
	{
		if( m_pCurrent )
			m_pCurrent->GetAlgorithm()->HandleInput( input );
	}

	void GetDebugInfo( std::string& dest_text_buffer );

	void LoadFromXMLDocument( CXMLNodeReader& node );

	void Serialize( IArchive& ar, const unsigned int version );

	friend class CMotionFSMCallback;
};


class CMotionFSMManager : public IArchiveObjectBase
{
	std::vector< boost::shared_ptr<CMotionFSM> > m_vecpMotionFSM;

	/// Stores the root node of the blend node tree
	boost::shared_ptr<CBlendNode> m_pBlendNodeRoot;

	// Name of complete skeleton used by this motion synthesizer.
	// The blend node tree is created from the skeleton.
	std::string m_CompleteSkeletonName;

	// Name of motion primitive from which the complete skeleton is derived.
	// Used if above m_CompleteSkeletonName is empty
	std::string m_CompleteSkeletonSourceMotionName;

	std::string m_MotionDatabaseFilepath;

private:

	void ResetBlendNodeRoot();

public:

	CMotionFSMManager();

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

	const std::vector< boost::shared_ptr<CMotionFSM> >& GetMotionFSMs() const { return m_vecpMotionFSM; }

	std::vector< boost::shared_ptr<CMotionFSM> >& MotionFSMs() { return m_vecpMotionFSM; }

	inline void Update( float dt );

	// Transformations are stored in m_pBlendNodeRoot
	inline void CalculateKeyframe();

	inline void GetCurrentKeyframe( CKeyframe& dest );

	void SetMotionDatabaseFilepath( const std::string& filepath ) { m_MotionDatabaseFilepath = filepath; }

	inline void SetStartBlendNodeToMotionPrimitives();

	// Calls SetStartBlendNodeToMotionPrimitives() inside the function.
	void LoadMotions( CMotionDatabase& mdb );

	// Open the motion database specified by m_MotionDatabaseFilepath and load motions.
	Result::Name LoadMotions();

	inline void HandleInput( const SInputData& input );

	void GetDebugInfo( std::string& dest_text_buffer );

	void LoadFromXMLDocument( CXMLNodeReader& node );

	void LoadFromXMLFile( const std::string& xml_file_path );

	void Serialize( IArchive& ar, const unsigned int version );

	void InitForTest( const std::string& motion_db_filepath = "motions/default.mdb" );
};


//================================ inline implementations ================================

inline void CMotionFSMManager::Update( float dt )
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->Update( dt );
}


/// Combine different keyframes of motions currenltly being played in each FSM
/// Store the resuts to ???
inline void CMotionFSMManager::CalculateKeyframe()
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->CalculateKeyframe();

	// m_pBlendNodeRoot stores transforms for the current keyframe
}


inline void CMotionFSMManager::GetCurrentKeyframe( CKeyframe& dest )
{
	if( m_vecpMotionFSM.empty() )
		return;

	if( !m_pBlendNodeRoot )
		return;

	m_pBlendNodeRoot->Clear();

	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
	{
		// Calculate keyframe and store it to blend node tree, m_pBlendNodeRoot
		m_vecpMotionFSM[i]->CalculateKeyframe();
	}

	// Combine the keyframes of motion primitives
	m_pBlendNodeRoot->GetKeyframe( dest );

	Vector3 t = dest.GetRootNode().GetLocalTranslation();
	t.x = 0;
	t.z = 0;
	dest.SetRootPose( Matrix34( t, dest.GetRootNode().GetLocalRotationQuaternion().ToRotationMatrix() ) );

	// Commented out - root node pose is set to identity in CBlendMatricesLoader::UpdateShaderParams().
	// It is, instead, specified as an argument to CMeshContainerRenderMethod::RenderMeshContainer
/*
	// Overwrite the pose of the root node.
	// The motions of first FSM controls the root node pose.
	CTransformNode root_node;
	Matrix34 root_pose( m_vecpMotionFSM.front()->Player()->GetCurrentRootPose() );
	root_node.SetRotation( Quaternion( root_pose.matOrient ) );
	root_node.SetTranslation( root_pose.vPosition );
	m_pBlendNodeRoot->SetTransformNodes( root_node );*/
}


inline void CMotionFSMManager::SetStartBlendNodeToMotionPrimitives()
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->SetStartBlendNodeToMotionPrimitives( m_pBlendNodeRoot );
}


inline void CMotionFSMManager::HandleInput( const SInputData& input )
{
	for( size_t i=0; i<m_vecpMotionFSM.size(); i++ )
		m_vecpMotionFSM[i]->HandleInput( input );
}


class CMotionFSMInputHandler : public CInputHandler
{
//	CMotionFSM *m_pFSM;
	boost::shared_ptr<CMotionFSMManager> m_pMotionGraphManager;

public:

	CMotionFSMInputHandler(boost::shared_ptr<CMotionFSMManager>& pMgr) : m_pMotionGraphManager(pMgr) {}

	inline void ProcessInput(SInputData& input);
};


inline void CMotionFSMInputHandler::ProcessInput(SInputData& input)
{
	m_pMotionGraphManager->HandleInput( input );
}



} // msynth


#endif /* __MotionFSM_HPP__ */
