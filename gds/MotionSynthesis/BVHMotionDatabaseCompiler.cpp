#include "BVHMotionDatabaseCompiler.hpp"
#include "BVH/BVHPlayer.hpp"
#include "XML/XMLDocumentLoader.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace msynth;


static void AlignLastKeyframeBVH( std::vector<CKeyframe>& vecKeyframe )
{
	if( vecKeyframe.size() == 0 )
		return;

	CKeyframe& last_keyframe = vecKeyframe.back();

	Matrix33 rotation_y = CalculateHorizontalOrientation( last_keyframe.GetRootPose() );

	Matrix34 align_h( Vector3(0,0,0), rotation_y );

	BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
	{
		keyframe.SetRootPose( align_h * keyframe.GetRootPose() );
	}
}


bool CBVHMotionDatabaseCompiler::IsValidMotionFile( const std::string& src_filepath )
{
	CBVHPlayer bvh_player;
	bool success = bvh_player.LoadBVHFile( src_filepath );

	return success;
}


/// output: a new motion primitive
void CBVHMotionDatabaseCompiler::CreateMotionPrimitive( CMotionPrimitiveDesc& desc,
												        const CMotionPrimitiveDescGroup& desc_group,
												        CBVHPlayer& bvh_player )
{
	vector<CKeyframe> vecKeyframe;
	vecKeyframe.reserve( 64 );

	// add a new motion primitive and get the reference to it
	shared_ptr<CMotionPrimitive> pMotion( new CMotionPrimitive( desc.m_Name ) );
	m_pvecpMotionPrimitive->push_back( pMotion );
	desc.m_pMotionPrimitive = pMotion;
	CMotionPrimitive& motion = *(m_pvecpMotionPrimitive->back());

	motion.SetSkeleton( desc_group.m_Skeleton );

	motion.SetLoopedMotion( desc.m_bIsLoopMotion );

	vecKeyframe.resize(0);

	int i = 0;
	int frame = 0;
	int start = desc.m_StartFrame;
	int end   = desc.m_EndFrame;
	float time_per_frame = bvh_player.GetFrameTime();

	// create keyframes
	for( frame = start, i=0;
		 frame <= end;
		 frame++,       i++ )
	{
		float frame_time = (float)frame * time_per_frame;

		bvh_player.SetWorldTransformation( frame_time );

		// extract world transforms

		CBVHBone *pRootBone = bvh_player.GetRootBone();

		// create keyframe from frametime and transformation hierarchy
		vecKeyframe.push_back( CKeyframe( (float)i * time_per_frame, CTransformNode( *pRootBone ) ) );
	}

	// modify root position
	if( 0 < vecKeyframe.size() && desc.m_bResetHorizontalRootPos )
	{
		Matrix34 root_pose = vecKeyframe[0].GetRootPose();
		Vector3 vBasePosH = root_pose.vPosition;
		vBasePosH.y = 0;

		BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
		{
			root_pose = keyframe.GetRootPose();
			root_pose.vPosition = root_pose.vPosition - vBasePosH;
			keyframe.SetRootPose( root_pose );
		}
	}

	if( desc.m_NormalizeOrientation == "AlignLastKeyframe" )
		AlignLastKeyframe( vecKeyframe );
	if( desc.m_NormalizeOrientation == "AlignLastKeyframeBVH" )
		AlignLastKeyframeBVH( vecKeyframe );

	if( 0.0001 < abs(1.0 - desc_group.m_fScalingFactor) )
	{
		BOOST_FOREACH( CKeyframe& keyframe, vecKeyframe )
		{
			keyframe.Scale( desc_group.m_fScalingFactor );
		}
	}

	motion.SetKeyframes( vecKeyframe );

	// set annotations
//	motion.m_vecAnnotation.resize( m_vecAnnotationName.size(), 0 );
	AnnotationArray(motion).resize( m_vecpAnnotationName->size(), 0 );

	BOOST_FOREACH( const std::string& annot, desc.m_vecAnnotation )
	{
		int index = GetAnnotationIndex( annot );
		if( 0 <= index )
//			motion.m_vecAnnotation[index] = 1;
			AnnotationArray(motion)[index] = 1;
	}
}


void CBVHMotionDatabaseCompiler::CreateMotionPrimitives( CMotionPrimitiveDescGroup& desc_group )
{
	CBVHPlayer bvh_player;
	bool loaded = bvh_player.LoadBVHFile( desc_group.m_Filename );
	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load the bvh file: " + desc_group.m_Filename );
		return;
	}

	// copy skeleton structure
	desc_group.m_Skeleton.CopyFromBVHSkeleton( *bvh_player.GetRootBone() );

	desc_group.m_Skeleton.Scale( desc_group.m_fScalingFactor );

	BOOST_FOREACH( CMotionPrimitiveDesc& desc, desc_group.m_Desc )
	{
		CreateMotionPrimitive( desc, desc_group, bvh_player );
	}
}
