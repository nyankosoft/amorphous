#ifndef __BuiltinMotions_HPP__
#define __BuiltinMotions_HPP__


#include <vector>
#include <boost/shared_ptr.hpp>
#include "fwd.hpp"
#include "amorphous/Support/Macro.h"


namespace amorphous
{

using namespace msynth;


//void InitBoneLocators( const msynth::Skeleton& skeleton, const char *names[], int num_bone_names );

//void CreateBuiltinMotions(
//	const msynth::Skeleton& skeleton,
//	std::vector< boost::shared_ptr<MotionPrimitive> >& pMotions );


boost::shared_ptr<MotionPrimitive> CreateWalkMotion( const msynth::Skeleton& skeleton );


} // amorphous



#endif /* __BuiltinMotions_HPP__ */
