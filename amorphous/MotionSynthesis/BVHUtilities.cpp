#include "BVHUtilities.hpp"
#include "Skeleton.hpp"
#include "../BVH/BVHPlayer.hpp"


namespace amorphous
{

using boost::shared_ptr;
using namespace msynth;


static void CopyBonesFromBVH_r( const BVHBone& src_bone, msynth::Bone& dest )
{
	dest.SetOffset( src_bone.GetLocalOffset() );

	dest.SetName( src_bone.GetName() );

	const int num_children = src_bone.GetNumChildren();
	dest.Children().resize( num_children );

	for( int i=0; i<num_children; i++ )
	{
		CopyBonesFromBVH_r( *src_bone.GetChild(i), dest.Children()[i] );
	}
}


void CopySkeletonFromBVH( const BVHPlayer& src_bvh, msynth::Skeleton& target )
{
	const BVHBone *pBone = src_bvh.GetRootBone();
	if( !pBone )
		return;

	Bone root_bone;
	CopyBonesFromBVH_r( *pBone, root_bone );

	target.SetBones( root_bone );
}


Result::Name CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname, msynth::Skeleton& dest_skeleton )
{
	BVHPlayer bvh_player;
	bvh_player.LoadBVHFile( bvh_file_pathname );

	if( !bvh_player.GetRootBone() )
		return Result::UNKNOWN_ERROR;

	Bone root_bone;
//	Bone& root_bone = pSkeleton->RootBone;

	CopyBonesFromBVH_r( *bvh_player.GetRootBone(), root_bone );

	dest_skeleton.SetBones( root_bone );
	
	return Result::SUCCESS;
}


boost::shared_ptr<msynth::Skeleton> CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname )
{
	shared_ptr<Skeleton> pSkeleton( new Skeleton );

	Result::Name res = CreateSkeletonFromBVHFile( bvh_file_pathname, *pSkeleton );

	if( res == Result::SUCCESS )
		return pSkeleton;
	else
		return shared_ptr<Skeleton>();
}


} // namespace amorphous
