#include "MotionPrimitive.hpp"
#include "BlendNode.hpp"

using namespace msynth;
using namespace boost;


void CTransformNode::SetInterpolatedTransform_r( float frac, const CTransformNode& node0, const CTransformNode& node1 )
{
	m_vTranslation = node0.m_vTranslation * ( 1.0f - frac ) + node1.m_vTranslation * frac;

//	m_Rotation = node0.m_Rotation * ( 1.0f - frac ) + node1.m_Rotation * frac;
	m_Rotation.Slerp( frac, node0.m_Rotation, node1.m_Rotation );

	const Matrix33 mat = m_Rotation.ToRotationMatrix(); // check the rotation for debugging

	const size_t num_children = node0.m_vecChildNode.size();

	if( m_vecChildNode.size() != num_children )
	{
		m_vecChildNode.resize( num_children );
	}

	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChildNode[i].SetInterpolatedTransform_r( frac, node0.m_vecChildNode[i], node1.m_vecChildNode[i] );
	}
}


//==========================================================================
// CMotionPrimitive
//==========================================================================

void CMotionPrimitive::InsertKeyframe( const CKeyframe& keyframe )
{
	if( m_vecKeyframe.empty() )
	{
		m_vecKeyframe.push_back( keyframe );
		return;
	}

	// find the right position to insert the keyframe
	const size_t num_keyframes = m_vecKeyframe.size();
	for( size_t i=0; i<num_keyframes; i++ )
	{
		if( keyframe.GetTime() <= m_vecKeyframe[i].GetTime() )
		{
			m_vecKeyframe.insert( m_vecKeyframe.begin() + i, keyframe );
			return;
		}
	}

	m_vecKeyframe.push_back( keyframe );
}


Result::Name CMotionPrimitive::GetNearestKeyframeIndices( float time, int& i0, int& i1 )
{
	if( time < 0 )
		return Result::INVALID_ARGS;

	// find the adjacent two frames which are closest to 'time'
	int i, iNumFrames = (int)m_vecKeyframe.size();
	for( i=0; i<iNumFrames; i++ )
	{
		if( time < m_vecKeyframe[i].GetTime() )
			break;
	}

	if( i == 0 || i == iNumFrames )
		return Result::UNKNOWN_ERROR;

	i0 = i - 1;
	i1 = i;

	return Result::SUCCESS;
}


Result::Name CMotionPrimitive::GetInterpolatedKeyframe( CKeyframe& dest_interpolated_keyframe,
												  float time,
												  Interpolation::Mode mode )
{
	int i0=0, i1=0;
	Result::Name res = GetNearestKeyframeIndices( time, i0, i1 );
	if( res != Result::SUCCESS )
		return res;

	const float& time0 = m_vecKeyframe[i0].GetTime();
	const float& time1 = m_vecKeyframe[i1].GetTime();

	float fFrac = ( time - time0 ) / ( time1 - time0 );

	dest_interpolated_keyframe.SetInterpolatedKeyframe( fFrac, m_vecKeyframe[i0], m_vecKeyframe[i1] );

	return Result::SUCCESS;
}


void CMotionPrimitive::CalculateInterpolatedKeyframe( float time )
{
	CKeyframe m_KeyframeBuffer;
	GetInterpolatedKeyframe( m_KeyframeBuffer, time );

	if( m_pStartBlendNode )
		m_pStartBlendNode->SetTransformNodes( m_KeyframeBuffer.GetRootNode() );
	else
	{
		LOG_PRINT_WARNING( " The start blend node is missing. Cannot set transforms to blend nodes tree." );
	}
}


Result::Name CMotionPrimitive::CreateEmptyKeyframes( uint num_keyframes )
{
	if( !m_pSkeleton )
		return Result::UNKNOWN_ERROR;

	CTransformNode root_node;

	m_pSkeleton->CreateEmptyTransformNodeTree( root_node );

	m_vecKeyframe.resize( num_keyframes );
	for( uint i=0; i<num_keyframes; i++ )
	{
		// set the copy of transform node tree at each keyframe
		m_vecKeyframe[i].RootNode() = root_node;
	}

	return Result::SUCCESS;
}


void CMotionPrimitive::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Name;
	ar & m_vecKeyframe;

	ar & m_pSkeleton;

	ar & m_bIsLoopedMotion;

	ar & m_vecAnnotation;

	if( 1 <= version )
		ar & m_StartBoneName;
}


void CMotionPrimitive::SearchAndSetStartBlendNode( shared_ptr<CBlendNode>& pRootBlendNode )
{
	m_pStartBlendNode = pRootBlendNode->GetBlendNodeByName( m_StartBoneName );

	if( !m_pStartBlendNode )
	{
		LOG_PRINT_WARNING( " The start bone '" + m_StartBoneName + "' was not found in the blend node tree." );
	}
}
