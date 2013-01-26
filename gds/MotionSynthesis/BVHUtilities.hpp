#ifndef __BVHUtilities_HPP__
#define __BVHUtilities_HPP__


#include <boost/shared_ptr.hpp>
#include "../base.hpp"
#include "../BVH/fwd.hpp"
#include "fwd.hpp"


namespace amorphous
{


void CopySkeletonFromBVH( const BVHPlayer& src_bvh, msynth::CSkeleton& target );

Result::Name CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname, msynth::CSkeleton& dest_skeleton );

boost::shared_ptr<msynth::CSkeleton> CreateSkeletonFromBVHFile( const std::string& bvh_file_pathname );


} // namespace amorphous



#endif /* __BVHUtilities_HPP__ */
