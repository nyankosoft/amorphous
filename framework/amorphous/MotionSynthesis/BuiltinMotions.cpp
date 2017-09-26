#include "BuiltinMotions.hpp"
#include "Skeleton.hpp"
#include "MotionPrimitive.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/ParamLoader.hpp"


namespace amorphous
{

using namespace std;
using namespace msynth;


static const char *bone_names[] =
{
	"l-hip",
	"r-hip",
	"l-thigh",
	"r-thigh",
	"l-leg",
	"r-leg",
	"l-foot",
	"r-foot",
	"l-clavicle",
	"r-clavicle",
	"l-shoulder",
	"r-shoulder",
	"l-arm",
	"r-arm",
	"l-forearm",
	"r-forearm",
};


void CreateMirroredKeyframe(
	const msynth::Skeleton& skeleton,
	std::vector< std::vector<int> > locators,
	std::vector< std::pair<int,int> >& bone_lr_pairs,
	const msynth::Keyframe& original,
	msynth::Keyframe& mirrored
	)
{
//	const msynth::Skeleton& skeleton = ???;

//	std::vector< std::string > bone_names;

//	skeleton.GetBoneNames( bone_names );
//	for( size_t i=0; i<bone_names.size(); i++ )

	// Copy the original keyframe
	mirrored = original;

//	vector< vector<int> > locators;

	const int num_locators = (int)locators.size();

	for( size_t i=0; i<bone_lr_pairs.size(); i++ )
	{
		int index_0 = bone_lr_pairs[i].first;
		int index_1 = bone_lr_pairs[i].second;
		if( num_locators <= index_0 || num_locators <= index_1 )
			continue;

		TransformNode *pNode0 = mirrored.GetTransformNode( locators[index_0] );
		TransformNode *pNode1 = mirrored.GetTransformNode( locators[index_1] );
		if( !pNode0 || !pNode1 )
			continue;

		TransformNode to_swap = *pNode0;
		mirrored.SetTransform( pNode1->GetTransform(), locators[index_0] );
		mirrored.SetTransform( to_swap.GetTransform(), locators[index_1] );
	}
}


void ShiftNodePosition( const Skeleton& skeleton, const vector<int>& node_locator, const Vector3& dest_pos, Keyframe& target )
{
	Vector3 shift = skeleton.CalculateNodePositionInSkeletonSpace( node_locator, target );

	Matrix34 root_pose = target.GetRootPose();
	root_pose.vPosition -= shift;
	target.SetRootPose( root_pose );
}


void ShiftRootNodePosition( const Vector3& shift, Keyframe& target )
{
	Matrix34 root_pose = target.GetRootPose();
	root_pose.vPosition += shift;
	target.SetRootPose( root_pose );
}


void SetLeftAndRightArmsRotationsForWalkMotion( vector< vector<int> >& locators, Keyframe& dest_keyframe )
{
	typedef vector<int> bone_locator;
	const bone_locator& l_clavicle  = locators[8];
	const bone_locator& r_clavicle  = locators[9];
	const bone_locator& l_shoulder  = locators[10];
	const bone_locator& r_shoulder  = locators[11];
	const bone_locator& l_arm       = locators[12];
	const bone_locator& r_arm       = locators[13];
	const bone_locator& l_forearm   = locators[14];
	const bone_locator& r_forearm   = locators[15];

	dest_keyframe.SetBoneLocalRotation(  l_clavicle,  0.0f,  0.0f,  0.5f );
	dest_keyframe.SetBoneLocalRotation(  r_clavicle,  0.0f,  0.0f, -0.5f );
//	dest_keyframe.SetBoneLocalRotation(  l_shoulder,  0.0f,  0.0f,  0.4f );
//	dest_keyframe.SetBoneLocalRotation(  r_shoulder,  0.0f,  0.0f, -0.4f );
	dest_keyframe.SetBoneLocalRotation(  l_arm,       0.5f,  0.0f,  0.5f );
	dest_keyframe.SetBoneLocalRotation(  r_arm,      -0.5f,  0.0f, -0.5f );
	dest_keyframe.SetBoneLocalRotation(  l_forearm,   0.3f,  0.0f,  0.0f );
	dest_keyframe.SetBoneLocalRotation(  r_forearm,  -0.3f,  0.0f,  0.0f );

	Vector3 r[8];
	memset( r, 0, sizeof(r) );
	LoadParamFromFile( ".debug/arm_rotations.txt", "l_clavicle", r[0].x, r[0].y, r[0].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "r_clavicle", r[1].x, r[1].y, r[1].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "l_shoulder", r[2].x, r[2].y, r[2].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "r_shoulder", r[3].x, r[3].y, r[3].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "l_arm",      r[4].x, r[4].y, r[4].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "r_arm",      r[5].x, r[5].y, r[5].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "l_forearm",  r[6].x, r[6].y, r[6].z );
	LoadParamFromFile( ".debug/arm_rotations.txt", "r_forearm",  r[7].x, r[7].y, r[7].z );
	dest_keyframe.SetBoneLocalRotation(  l_clavicle, r[0].x, r[0].y, r[0].z );
	dest_keyframe.SetBoneLocalRotation(  r_clavicle, r[1].x, r[1].y, r[1].z );
	dest_keyframe.SetBoneLocalRotation(  l_shoulder, r[2].x, r[2].y, r[2].z );
	dest_keyframe.SetBoneLocalRotation(  r_shoulder, r[3].x, r[3].y, r[3].z );
	dest_keyframe.SetBoneLocalRotation(  l_arm,      r[4].x, r[4].y, r[4].z );
	dest_keyframe.SetBoneLocalRotation(  r_arm,      r[5].x, r[5].y, r[5].z );
	dest_keyframe.SetBoneLocalRotation(  l_forearm,  r[6].x, r[6].y, r[6].z );
	dest_keyframe.SetBoneLocalRotation(  r_forearm,  r[7].x, r[7].y, r[7].z );
}


shared_ptr<MotionPrimitive> CreateWalkMotion( const msynth::Skeleton& skeleton )
{
	const int num_bone_names = numof(bone_names);
	vector< vector<int> > locators;
	locators.resize( num_bone_names );
	vector<int> is_valid_locator; // boolean
	is_valid_locator.resize( num_bone_names, 0 );

	for( int i=0; i<numof(bone_names); i++ )
	{
		bool locator_created = skeleton.CreateLocator( bone_names[i], locators[i] );
		is_valid_locator[i] = locator_created ? 1 : 0;
	}

	// Find left & right pairs
	string bone_name_0, bone_name_1;
	typedef std::pair<int,int> int_pair;
	vector<int_pair> bone_lr_pairs;
	bone_lr_pairs.reserve( 16 );
	for( int i=0; i<num_bone_names; i++ )
	{
		if( !is_valid_locator[i] )
			continue;

		for( int j=i+1; j<num_bone_names; j++ )
		{
			if( !is_valid_locator[j] )
				break;

			bone_name_0 = bone_names[i];
			bone_name_1 = bone_names[j];
			if( bone_name_0.substr(2) != bone_name_1.substr(2) )
				continue; // the strings after "l-" and "r-" do not match.

			if( bone_name_0.find("l-") == 0 && bone_name_1.find("r-") == 0
			 || bone_name_0.find("r-") == 0 && bone_name_1.find("l-") == 0 )
			{
				// Add a pair
				bone_lr_pairs.push_back( int_pair(i,j) );
			}
		}

	}


	typedef vector<int> bone_locator;
	const bone_locator& l_hip       = locators[0];
	const bone_locator& r_hip       = locators[1];
	const bone_locator& l_thigh     = locators[2];
	const bone_locator& r_thigh     = locators[3];
	const bone_locator& l_leg       = locators[4];
	const bone_locator& r_leg       = locators[5];
	const bone_locator& l_foot      = locators[6];
	const bone_locator& r_foot      = locators[7];
	const bone_locator& l_clavicle  = locators[8];
	const bone_locator& r_clavicle  = locators[9];
	const bone_locator& l_shoulder  = locators[10];
	const bone_locator& r_shoulder  = locators[11];

	// Create a motion primitive object for walk motion.

	shared_ptr<MotionPrimitive> pWalkMotion( new MotionPrimitive );
	MotionPrimitive& walk_motion = *pWalkMotion;
	walk_motion.SetLoopedMotion( true );
	walk_motion.SetSkeleton( skeleton );
	walk_motion.CreateEmptyKeyframes( 4 );
	vector<Keyframe>& keyframes = pWalkMotion->GetKeyframeBuffer();
	Keyframe& on_right          = keyframes[0];
	Keyframe& full_stride_left  = keyframes[1];
	Keyframe& on_left           = keyframes[2];
	Keyframe& full_stride_right = keyframes[3];

//	Keyframe on_right(0.0f), full_stride_left(0.5f), on_left(1.0f), full_stride_right(1.5f);

	// full stride (the left leg forward)
	full_stride_left.SetBoneLocalRotation(  l_hip,       0.0f, -0.15f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  r_hip,       0.0f,  0.15f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  l_thigh,     0.0f, -0.20f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  r_thigh,     0.0f,  0.20f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  l_leg,       0.0f,  0.30f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  r_leg,       0.0f,  0.30f,  0.0f );

	SetLeftAndRightArmsRotationsForWalkMotion( locators, full_stride_left );
/*
//	full_stride_left.SetBoneLocalRotation(  l_clavicle,  0.0f,  0.5f,  0.0f );
//	full_stride_left.SetBoneLocalRotation(  r_clavicle,  0.0f, -0.5f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  l_shoulder,  0.0f,  0.0f,  0.6f );
	full_stride_left.SetBoneLocalRotation(  r_shoulder,  0.0f,  0.0f, -0.6f );
	full_stride_left.SetBoneLocalRotation(  l_arm,       0.0f,  0.2f,  0.0f );
	full_stride_left.SetBoneLocalRotation(  r_arm,       0.0f,  0.2f,  0.0f );
*/

//	Transform r_foot_transform = full_stride_left.GetTransform( r_foot );
//	Vector3 r_foot_offset = - r_foot_transform.vTranslation;
//	Matrix34 root_pose = full_stride_left.GetRootPose();
//	root_pose.vPosition += r_foot_offset;
//	full_stride_left.SetRootPose( root_pose );

	// full stride (the right leg forward)
//	full_stride_right.SetBoneLocalRotation( l_hip,    0.0f,  0.2f, 0.0f );
//	full_stride_right.SetBoneLocalRotation( r_hip,    0.0f, -0.2f, 0.0f );
//	full_stride_right.SetBoneLocalRotation( l_thigh,  0.0f,  0.2f, 0.0f );
//	full_stride_right.SetBoneLocalRotation( r_thigh,  0.0f,  0.2f, 0.0f );
//	SetLeftAndRightArmsRotationsForWalkMotion( locators, full_stride_right );

	CreateMirroredKeyframe( skeleton, locators, bone_lr_pairs, full_stride_left, full_stride_right );
	SetLeftAndRightArmsRotationsForWalkMotion( locators, full_stride_right );

	// standing on the right leg
	on_right.SetBoneLocalRotation( l_hip,   0.0f,  0.0f, 0.0f );
	on_right.SetBoneLocalRotation( r_hip,   0.0f,  0.0f, 0.0f );
	on_right.SetBoneLocalRotation( l_thigh, 0.0f, -0.5f, 0.0f );
	on_right.SetBoneLocalRotation( r_thigh, 0.0f,  0.0f, 0.0f );
	on_right.SetBoneLocalRotation( l_leg,   0.0f,  1.0f, 0.0f );
	on_right.SetBoneLocalRotation( r_leg,   0.0f,  0.1f, 0.0f );
	SetLeftAndRightArmsRotationsForWalkMotion( locators, on_right );

	// standing on the left leg
//	on_left.SetBoneLocalRotation(  l_hip,   0.0f, -0.5f, 0.0f );
//	on_left.SetBoneLocalRotation(  r_hip,   0.0f, -0.1f, 0.0f );
//	on_left.SetBoneLocalRotation(  l_thigh, 0.0f,  0.5f, 0.0f );
//	on_left.SetBoneLocalRotation(  r_thigh, 0.0f,  0.1f, 0.0f );
//	on_left.SetBoneLocalRotation(  l_leg,   0.0f,  0.0f, 0.0f );
//	on_left.SetBoneLocalRotation(  r_leg,   0.0f,  0.3f, 0.0f );
//	SetLeftAndRightArmsRotationsForWalkMotion( locators, on_left );

	CreateMirroredKeyframe( skeleton, locators, bone_lr_pairs, on_right, on_left );
	SetLeftAndRightArmsRotationsForWalkMotion( locators, on_left );

	// Shift root bone positions
//	SetFootPositionToTheGround( on_right, r_foot );
//	Vector3 r_foot_pos = on_right.GetBonePosition( r_foot );

	// Set keyframe times
	on_right.SetTime(          0.0f );
	full_stride_left.SetTime(  0.5f );
	on_left.SetTime(           1.0f );
	full_stride_right.SetTime( 1.5f );

	Vector3 stride
		= skeleton.CalculateNodePositionInSkeletonSpace( l_foot, full_stride_left )
		- skeleton.CalculateNodePositionInSkeletonSpace( r_foot, full_stride_left );

//	ShiftNodePosition( skeleton, r_foot, Vector3(0,0,0), on_right );
//	ShiftNodePosition( skeleton, r_foot, Vector3(0,0,0), full_stride_left );
//	ShiftNodePosition( skeleton, l_foot, stride, on_left );
//	ShiftNodePosition( skeleton, l_foot, stride, full_stride_right );

	Vector3 r_foot_pos = Vector3(0,0,0), l_foot_pos = Vector3(0,0,0);

	r_foot_pos = skeleton.CalculateNodePositionInSkeletonSpace( r_foot, on_right );

	ShiftRootNodePosition( Vector3(0,0,-r_foot_pos.z), on_right );

	r_foot_pos = skeleton.CalculateNodePositionInSkeletonSpace( r_foot, full_stride_left );

	ShiftRootNodePosition( Vector3(0,0,-r_foot_pos.z), full_stride_left );

	l_foot_pos = skeleton.CalculateNodePositionInSkeletonSpace( l_foot, on_left );

	ShiftRootNodePosition( Vector3(0,0,-l_foot_pos.z-r_foot_pos.z), on_left );

	l_foot_pos = skeleton.CalculateNodePositionInSkeletonSpace( l_foot, full_stride_right );

	ShiftRootNodePosition( Vector3(0,0,-l_foot_pos.z-r_foot_pos.z), full_stride_right );

	shared_ptr<MotionPrimitive> pNoMotion( new MotionPrimitive );
	pNoMotion->SetSkeleton( skeleton );
	pNoMotion->CreateEmptyKeyframes( 1 );
	vector<Keyframe>& kf = pNoMotion->GetKeyframeBuffer();
	Vector3 default_r_foot_pos = skeleton.CalculateNodePositionInSkeletonSpace( r_foot, kf[0] );

//	Vector3 r_foot_pos_diff = 

	return pWalkMotion;
}


} // amorphous
