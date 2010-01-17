#include "SkeletalCharacter.hpp"
#include <gds/Input/InputHub.hpp>
#include <gds/Graphics/Mesh/SkeletalMesh.hpp>
#include <gds/MotionSynthesis/MotionDatabase.hpp>
#include <gds/MotionSynthesis/MotionPrimitiveBlender.hpp>
#include <gds/Support/DebugOutput.hpp>

using namespace std;
using namespace boost;
using namespace msynth;


void UpdateMeshBoneTransforms_r( const msynth::CBone& bone,
                                 const msynth::CTransformNode& node,
								 CSkeletalMesh& mesh )//,
//								 CMM_Bone& mesh_bone )
{
	// find the matrix index from the bone name (slow).
	int index = mesh.GetBoneMatrixIndexByName( bone.GetName() );
	if( index == -1 )
		return;

	Matrix34 local_transform;
	local_transform.vPosition = node.GetLocalTranslation();
	local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix();
	mesh.SetLocalTransformToCache( index, local_transform );

	const int num_child_bones = bone.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
//	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		UpdateMeshBoneTransforms_r(
			bone.GetChild(i),
			node.GetChildNode(i),
			mesh );
	}

/*	for( int i=0; i<num_child_bones; i++ )
	{
		for( int j=0; j<num_child_nodes; j++ )
			bone.GetImmediateChild( node.Get );
	}*/
}


void UpdateMeshBoneTransforms( const msynth::CKeyframe& keyframe, const msynth::CSkeleton& skeleton, CSkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), target_skeletal_mesh );
}


class CDebugItem_MotionFSM : public CDebugItem_ResourceManager
{
public:
	boost::shared_ptr<msynth::CMotionGraphManager> m_pMotionGraphManager;
public:

	CDebugItem_MotionFSM() {}

	void GetTextInfo()
	{
		string buffer;
		m_pMotionGraphManager->GetDebugInfo( buffer );
		strncpy( m_TextBuffer, buffer.c_str(), sizeof(m_TextBuffer) - 1 );
	}
};


CSkeletalCharacter::CSkeletalCharacter()
:
m_fFwdSpeed(0.0f),
m_fTurnSpeed(0.0f)
{
	m_pMotionGraphManager.reset( new CMotionGraphManager );
//	m_pMotionGraphManager = shared_new<CMotionGraphManager>();
	m_pMotionGraphManager->InitForTest( "motions/lws-fwd.mdb" );

	shared_ptr<CMotionFSM> pLowerLimbsFSM = m_pMotionGraphManager->GetMotionFSM( "lower_limbs" );

	m_pMotionNodes.resize( 3 );
	m_pMotionNodes[0].reset( new CFwdMotionNode );
	m_pMotionNodes[1].reset( new CStandingMotionNode );
	m_pMotionNodes[2].reset( new CJumpMotionNode );
	for( size_t i=0; i<m_pMotionNodes.size(); i++ )
	{
		m_pMotionNodes[i]->SetSkeletalCharacter( this );
	}

	pLowerLimbsFSM->GetNode( "fwd" )->SetAlgorithm( m_pMotionNodes[0] );
	pLowerLimbsFSM->GetNode( "standing" )->SetAlgorithm( m_pMotionNodes[1] );
//	pLowerLimbsFSM->GetNode( "jump" )->SetAlgorithm( m_pMotionNodes[2] );

	m_pLowerLimbsMotionsFSM = m_pMotionGraphManager->GetMotionFSM( "lower_limbs" );
	if( !m_pLowerLimbsMotionsFSM )
		m_pLowerLimbsMotionsFSM.reset( new CMotionFSM ); // avoid NULL checking

	// mesh

	shared_ptr<CMeshObjectContainer> pContainer( new CMeshObjectContainer );
	pContainer->m_MeshDesc.ResourcePath = "models/male_skinny_young.msh";
	pContainer->m_MeshDesc.MeshType = CMeshType::SKELETAL;
//	pContainer->m_MeshDesc.pMeshGenerator.reset( new CBoxMeshGenerator() );
	m_MeshContainerRootNode.SetMeshContainer( 0, pContainer );

	m_MeshContainerRootNode.LoadMeshesFromDesc();

	m_pRenderMethod.reset( new CMeshContainerRenderMethod );
//	m_pRenderMethod = shared_new<CMeshContainerRenderMethod>();

	m_pRenderMethod->MeshRenderMethod().resize( 1 );
//	m_pRenderMethod->MeshRenderMethod()[0].m_ShaderFilepath = "Shader/VertexBlend.fx";
	m_pRenderMethod->MeshRenderMethod()[0].m_ShaderFilepath = "Shader/Default.fx";
	m_pRenderMethod->MeshRenderMethod()[0].m_Technique.SetTechniqueName( "VertBlend_PVL_HSLs" );
//	m_pRenderMethod->MeshRenderMethod()[0].m_Technique.SetTechniqueName( "SingleHSDL_Specular_CTS" );
	m_pRenderMethod->LoadRenderMethodResources();

	// Init input handler
	m_pInputHandler.reset( new CMotionFSMInputHandler(m_pMotionGraphManager) );
	InputHub().SetInputHandler( 0, m_pInputHandler.get() );

	// save a motion primitive to get skeleton info in Render()
	CMotionDatabase mdb("motions/lws-fwd.mdb");
	m_pSkeletonSrcMotion = mdb.GetMotionPrimitive("standing");

	m_pMotionGraphManager->GetMotionFSM("lower_limbs")->StartMotion("standing");

	CDebugItem_MotionFSM *pDbgItem = new CDebugItem_MotionFSM;
	pDbgItem->m_pMotionGraphManager = m_pMotionGraphManager;
	DebugOutput.AddDebugItem( "motion_graph_mgr", pDbgItem );
}

/*
void CSkeletalCharacter::InitResources(){}
*/

void CSkeletalCharacter::OnEntityCreated( CCopyEntity& entity )
{
//	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
//	if( !pEntity )
//		return;

//	CCopyEntity& entity = *pEntity;

	entity.m_MeshHandle = m_MeshContainerRootNode.GetMeshContainer(0)->m_MeshObjectHandle;
	entity.m_pMeshRenderMethod = m_pRenderMethod;
}


void CSkeletalCharacter::Update( float dt )
{
	shared_ptr<CMotionFSM> pFSM = m_pMotionGraphManager->GetMotionFSM("lower_limbs");
	if( !pFSM )
		return;

	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
	if( !pEntity )
		return;

	Matrix34 world_pose = pEntity->GetWorldPose();

	// steering
	world_pose.matOrient = world_pose.matOrient * Matrix33RotationY( GetTurnSpeed() * dt );

	pFSM->Player()->SetCurrentHorizontalPose( world_pose );

	m_pMotionGraphManager->Update( dt );

	Matrix34 updated_world_pose = pFSM->Player()->GetCurrentHorizontalPose();

	// test collision

	pEntity->SetWorldPose( updated_world_pose );

//	Matrix34 world_pose = m_pMotionGraphManager->GetCurrentWorldPose();

/*
	const Matrix34 world_pose = m_pMotionGraphManager->GetRootNodeWorldPose();
	SetWorldPose( world_pose );
	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
*/
}


void CSkeletalCharacter::Render()
{
	if( m_MeshContainerRootNode.GetNumMeshContainers() == 0 )
		return;

	shared_ptr<CMeshObjectContainer> pContainer = m_MeshContainerRootNode.GetMeshContainer( 0 );
	if( !pContainer )
		return;

	shared_ptr<CBasicMesh> pMesh = pContainer->m_MeshObjectHandle.GetMesh();
	CSkeletalMesh *pSkeletalMesh = dynamic_cast<CSkeletalMesh *>(pMesh.get());
	if( !pSkeletalMesh )
	{
		int non_skeletal_mesh = 1;
//		return;
	}

//	pSkeletalMesh->Render();

	if( !m_pSkeletonSrcMotion )
		return;

	shared_ptr<CCopyEntity> pEntity = m_Entity.Get();
	if( !pEntity )
		return;

	CKeyframe m_InterpolatedKeyframe;
	CKeyframe& dest = m_InterpolatedKeyframe;
	m_pMotionGraphManager->GetCurrentKeyframe( dest );
	UpdateMeshBoneTransforms( dest, *(m_pSkeletonSrcMotion->GetSkeleton()), *pSkeletalMesh );

	pSkeletalMesh->SetLocalTransformsFromCache();
///	Matrix34 pose = Matrix34Identity();
	Matrix34 pose = pEntity->GetWorldPose();
	m_pRenderMethod->RenderMeshContainer( *(m_MeshContainerRootNode.MeshContainer( 0 )), pose );
}


void CSkeletalCharacter::SetKeyBind( shared_ptr<CKeyBind> pKeyBind )
{
	m_pKeyBind = pKeyBind;
	for( size_t i=0; i<m_pMotionNodes.size(); i++ )
		m_pMotionNodes[i]->SetKeyBind( m_pKeyBind );
}


// Moved to CCharacterMotionNodeAlgorithm::HandleInput()
/*
void CSkeletalCharacter::ProcessInput( const SInputData& input, int action_code )
{
	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fFwdSpeed =  input.fParam1;
			m_pLowerLimbsMotionsFSM->RequestTransition( "fwd" );
		}
		break;
	case ACTION_MOV_BACKWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fFwdSpeed = -input.fParam1;
//			m_pLowerLimbsMotionsFSM->RequestTransition( "bwd" );
		}
		break;
	case ACTION_MOV_TURN_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fTurnSpeed =  input.fParam1;
		}
		break;
	case ACTION_MOV_TURN_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_fTurnSpeed = -input.fParam1;
		}
		break;
	default:
		break;
	}
}
*/


bool CCharacterMotionNodeAlgorithm::HandleInput( const SInputData& input, int action_code )
{
	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pCharacter->SetFwdSpeed(  input.fParam1 );
		}
		else if( input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pCharacter->SetFwdSpeed(  input.fParam1 );
		}
		else
			m_pCharacter->SetFwdSpeed( 0 );
		break;
	case ACTION_MOV_BACKWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pCharacter->SetFwdSpeed( -input.fParam1 );
//			m_fFwdSpeed = -input.fParam1;
//			m_pLowerLimbsMotionsFSM->RequestTransition( "bwd" );
		}
		break;
	case ACTION_MOV_TURN_R:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pCharacter->SetTurnSpeed(  input.fParam1 );
//			m_fTurnSpeed =  input.fParam1;
		}
		else
			m_pCharacter->SetTurnSpeed( 0 );
		break;
	case ACTION_MOV_TURN_L:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pCharacter->SetTurnSpeed( -input.fParam1 );
//			m_fTurnSpeed = -input.fParam1;
		}
		else
			m_pCharacter->SetTurnSpeed( 0 );
		break;
	default:
		break;
	}

	return false;
}


void CFwdMotionNode::Update( float dt )
{
	float fFwdSpeed = m_pCharacter->GetFwdSpeed();
	if( fFwdSpeed < 0.2f )
	{
		RequestTransition( "standing" );
	}
/*	else if( fFwdSpeed < 0.5f )
	{
		RequestTransition( "fwd" ); // walk
	}
	else
	{
		RequestTransition( "fwd" ); // run
	}*/
}


bool CFwdMotionNode::HandleInput( const SInputData& input, int action_code )
{
	CCharacterMotionNodeAlgorithm::HandleInput( input, action_code );

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_pNode->SetMotionPlaySpeedFactor( input.fParam1 * 0.5f );
		}
		else if( input.iType == ITYPE_VALUE_CHANGED )
		{
			m_pNode->SetMotionPlaySpeedFactor( input.fParam1 * 0.5f );
		}
		break;
	case ACTION_MOV_JUMP:
		RequestTransition( "jump" );
		break;
/*	case ACTION_MOV_TURN_L:
		break;
	case ACTION_MOV_TURN_R:
		break;*/
	default:
		break;
	}

	return false;
}


void CFwdMotionNode::EnterState()
{
	// update the forward speed
	m_pNode->SetMotionPlaySpeedFactor( m_pCharacter->GetFwdSpeed() );
}


void CJumpMotionNode::Update( float dt )
{
}


bool CJumpMotionNode::HandleInput( const SInputData& input, int action_code )
{
	return false;
}



void CStandingMotionNode::Update( float dt )
{
}


bool CStandingMotionNode::HandleInput( const SInputData& input, int action_code )
{
	CCharacterMotionNodeAlgorithm::HandleInput( input, action_code );

	switch( action_code )
	{
	case ACTION_MOV_FORWARD:
		if( input.iType == ITYPE_KEY_PRESSED
		 && 0.2f < input.fParam1 )
		{
			RequestTransition( "fwd" );
		}
		else if( input.iType == ITYPE_VALUE_CHANGED
		 && 0.2f < input.fParam1 )
		{
			RequestTransition( "fwd" );
		}
		break;
	case ACTION_MOV_JUMP:
		RequestTransition( "jump" );
		break;
/*	case ACTION_MOV_TURN_L:
		break;
	case ACTION_MOV_TURN_R:
		break;*/
	default:
		break;
	}

	return false;
}
