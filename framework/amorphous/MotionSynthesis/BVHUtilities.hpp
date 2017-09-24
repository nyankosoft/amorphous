#ifndef __BVHUtilities_HPP__
#define __BVHUtilities_HPP__


#include <memory>
#include "amorphous/base.hpp"
#include "amorphous/MotionCapture/BVH/fwd.hpp"
#include "fwd.hpp"


namespace amorphous
{


void CopySkeletonFromBVH( const BVHPlayer& src_bvh, msynth::Skeleton& target );

Result::Name CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname, msynth::Skeleton& dest_skeleton );

std::shared_ptr<msynth::Skeleton> CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname );


} // namespace amorphous



#endif /* __BVHUtilities_HPP__ */
