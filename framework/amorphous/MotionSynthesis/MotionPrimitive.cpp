#include "MotionPrimitive.hpp"
#include "BlendNode.hpp"

#include "amorphous/3DMath/Matrix44.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/ParamLoader.hpp"


namespace amorphous
{

using namespace msynth;
using namespace boost;


//==========================================================================
// MotionPrimitive
//==========================================================================

void MotionPrimitive::InsertKeyframe( const Keyframe& keyframe )
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


Result::Name MotionPrimitive::GetNearestKeyframeIndices( float time, int& i0, int& i1 )
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

// The index selection for looped motions

// Looped motion and keyframes
// - When the motion is interpolated with spline interpolation, the last keyframe of a motion
//   is treated in a way different from the others.
//   - The transforms of the last keyframe are not used.
//   - The time of the last keyframe is referenced, and it represents the overall motion length.
//     The motion is wrapped at the time of the last keyframe.

// For a 6-keyframe looped motion with duplicate keyframes at 0 and n (5 in the example below)
// (n = the number of keyframes).
// 0     1     2     3     4     5
// |-----|-----|-----|-----|-----|

// 0  |  1     2     3     4     5  i2=1 -> i0=n-2, i1=0, i2=1, i3=2
// 1-----2-----3-----|-----0-----|

// 0     1  |  2     3     4     5  i2=2
// 0-----1-----2-----3-----|-----|

// 0     1     2  |  3     4     5
// |-----0-----1-----2-----3-----|

// 0     1     2     3  |  4     5  i2=n-2 -> i0=n-4, i1=n-3, i2=n-2, i3=0
// 3-----|-----0-----1-----2-----|

// 0     1     2     3     4  |  5  i2=n-1 -> i0=n-3, i1=n-2, i2=0, i3=1
// 2-----3-----|-----0-----1-----|

Result::Name MotionPrimitive::GetNearestKeyframeIndices( float time, int& i0, int& i1, int& i2, int& i3, float& frac, fixed_vector<Vector3,4>& root_position_offsets )
{
	if( time < 0 )
		return Result::INVALID_ARGS;

	const std::vector<Keyframe>& keyframes = m_vecKeyframe;

	// find the adjacent two frames which are closest to 'time'
	int i, iNumFrames = (int)m_vecKeyframe.size();
	int time1 = 0, time2 = 1;
	for( i=0; i<iNumFrames; i++ )
	{
		if( time < m_vecKeyframe[i].GetTime() )
			break;
	}

	if( i == 0 || i == iNumFrames )
		return Result::UNKNOWN_ERROR;

	if( m_bIsLoopedMotion
	 || ( 4 <= iNumFrames && 2 <= i && i < iNumFrames - 1 ) )
	{
		bool skip_last_keyframe = true;
		if( skip_last_keyframe
		 && m_bIsLoopedMotion
		 && 4 <= iNumFrames )
		{
			const Vector3 root_position_offset
				= keyframes.back().GetRootPose().vPosition
				- keyframes.front().GetRootPose().vPosition;

			if( i == 1 )
			{
				i0 = iNumFrames - 2;
				i1 = 0;
				i2 = 1;
				i3 = 2;
				time1 = 0;
				time2 = 1;
				root_position_offsets[0] = -root_position_offset;
			}
//			else if( i == 2 )
//			{
//				i0 = 0;
//				i1 = 1;
//				i2 = 2;
//				i3 = 3;
//			}
			else if( i == iNumFrames - 2 )
			{
				i0 = iNumFrames - 4;
				i1 = iNumFrames - 3;
				i2 = iNumFrames - 2;
				i3 = 0;
				time1 = i1;
				time2 = i2;
				root_position_offsets[3] = root_position_offset;
			}
			else if( i == iNumFrames - 1 )
			{
				i0 = iNumFrames - 3;
				i1 = iNumFrames - 2;
				i2 = 0;
				i3 = 1;
				time1 = iNumFrames - 2;
				time2 = iNumFrames - 1;
				root_position_offsets[2] = root_position_offset;
				root_position_offsets[3] = root_position_offset;
			}
			else if( 2 <= i )
			{
				i0 = i-2;
				i1 = i-1;
				i2 = i;
				i3 = i+1;
				time1 = i1;
				time2 = i2;
			}
			else
			{
				i0 = 0;
				i1 = 0;
				i2 = 0;
				i3 = 0;
				time1 = 0;
				time2 = 1;
			}
			frac
				= ( time - m_vecKeyframe[time1].GetTime() )
				/ ( m_vecKeyframe[time2].GetTime() - m_vecKeyframe[time1].GetTime() );
		}
		else
		{
			// ' % iNumFrames' is for looped motions
			i0 = (i - 2 + iNumFrames) % iNumFrames;
			i1 = i - 1;
			i2 = i;
			i3 = (i + 1) % iNumFrames;
		}
	}
	else
	{
		i0 = -1;
		i1 = i - 1;
		i2 = i;
		i3 = -1;
	}

	return Result::SUCCESS;
}


Result::Name MotionPrimitive::GetInterpolatedKeyframe( Keyframe& dest_interpolated_keyframe,
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

	int test_spline_interpolation = 0;
	LoadParamFromFile( ".debug/MotionSynthesis.txt", "test_spline_interpolation", test_spline_interpolation );
//	if( m_vecKeyframe.size() <= 2 )
	if( m_vecKeyframe.size() <= 2 || test_spline_interpolation == 0 )
	{
		dest_interpolated_keyframe.SetInterpolatedKeyframe( fFrac, m_vecKeyframe[i0], m_vecKeyframe[i1] );
	}
	else
	{
		fixed_vector<Vector3,4> root_position_offsets;
		root_position_offsets.resize( 4, Vector3(0,0,0) );
		float fFrac = 0;
		int i0=0, i1=0, i2=0, i3=0;
		Result::Name res = GetNearestKeyframeIndices( time, i0, i1, i2, i3, fFrac, root_position_offsets );
		if( res != Result::SUCCESS )
			return res;

		const float& time1 = m_vecKeyframe[i1].GetTime();
		const float& time2 = m_vecKeyframe[i2].GetTime();

//		float fFrac = ( time - time1 ) / ( time2 - time1 );

		if( i0 == -1 )
			dest_interpolated_keyframe.SetInterpolatedKeyframe( fFrac, m_vecKeyframe[i1], m_vecKeyframe[i2] );
		else
		{
			// copy and save the original translations
			Vector3 orig_root_positions[4] = {
				m_vecKeyframe[i0].m_RootNode.GetTransform().vTranslation,
				m_vecKeyframe[i1].m_RootNode.GetTransform().vTranslation,
				m_vecKeyframe[i2].m_RootNode.GetTransform().vTranslation,
				m_vecKeyframe[i3].m_RootNode.GetTransform().vTranslation
			};
			m_vecKeyframe[i0].m_RootNode.SetTranslation( m_vecKeyframe[i0].m_RootNode.GetLocalTranslation() + root_position_offsets[0] );
			m_vecKeyframe[i1].m_RootNode.SetTranslation( m_vecKeyframe[i1].m_RootNode.GetLocalTranslation() + root_position_offsets[1] );
			m_vecKeyframe[i2].m_RootNode.SetTranslation( m_vecKeyframe[i2].m_RootNode.GetLocalTranslation() + root_position_offsets[2] );
			m_vecKeyframe[i3].m_RootNode.SetTranslation( m_vecKeyframe[i3].m_RootNode.GetLocalTranslation() + root_position_offsets[3] );

			dest_interpolated_keyframe.SetInterpolatedKeyframe( fFrac, m_vecKeyframe[i0], m_vecKeyframe[i1], m_vecKeyframe[i2], m_vecKeyframe[i3] );

			// restore the original translations
			m_vecKeyframe[i0].m_RootNode.SetTranslation( orig_root_positions[0] );
			m_vecKeyframe[i1].m_RootNode.SetTranslation( orig_root_positions[1] );
			m_vecKeyframe[i2].m_RootNode.SetTranslation( orig_root_positions[2] );
			m_vecKeyframe[i3].m_RootNode.SetTranslation( orig_root_positions[3] );
		}
	}

	return Result::SUCCESS;
}


void MotionPrimitive::CalculateInterpolatedKeyframe( float time )
{
	Keyframe m_KeyframeBuffer;
	GetInterpolatedKeyframe( m_KeyframeBuffer, time );

	if( m_pStartBlendNode )
		m_pStartBlendNode->SetTransformNodes( m_KeyframeBuffer.GetRootNode() );
	else
	{
		LOG_PRINT_WARNING( " The start blend node is missing. Cannot set transforms to blend nodes tree." );
	}
}


Result::Name MotionPrimitive::CreateEmptyKeyframes( uint num_keyframes )
{
	if( !m_pSkeleton )
		return Result::UNKNOWN_ERROR;

	TransformNode root_node;

	m_pSkeleton->CreateEmptyTransformNodeTree( root_node );

	m_vecKeyframe.resize( num_keyframes );
	for( uint i=0; i<num_keyframes; i++ )
	{
		// set the copy of transform node tree at each keyframe
		m_vecKeyframe[i].RootNode() = root_node;
	}

	return Result::SUCCESS;
}


void MotionPrimitive::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Name;
	ar & m_vecKeyframe;

	ar & m_pSkeleton;

	ar & m_bIsLoopedMotion;

	ar & m_vecAnnotation;

	if( 1 <= version )
		ar & m_StartBoneName;
}


void MotionPrimitive::SearchAndSetStartBlendNode( shared_ptr<BlendNode>& pRootBlendNode )
{
	m_pStartBlendNode = pRootBlendNode->GetBlendNodeByName( m_StartBoneName );

	if( !m_pStartBlendNode )
	{
		LOG_PRINT_WARNING( " The start bone '" + m_StartBoneName + "' was not found in the blend node tree." );
	}
}


} // namespace amorphous
