#ifndef __BuiltinMotions_HPP__
#define __BuiltinMotions_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include "fwd.hpp"
#include "../Support/Macro.h"


namespace amorphous
{

using namespace msynth;


//void InitBoneLocators( const msynth::CSkeleton& skeleton, const char *names[], int num_bone_names );

//void CreateBuiltinMotions(
//	const msynth::CSkeleton& skeleton,
//	std::vector< boost::shared_ptr<CMotionPrimitive> >& pMotions );


boost::shared_ptr<CMotionPrimitive> CreateWalkMotion( const msynth::CSkeleton& skeleton );


} // amorphous



#endif /* __BuiltinMotions_HPP__ */
