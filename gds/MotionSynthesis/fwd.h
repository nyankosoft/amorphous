#ifndef  __MotionSynthesis_Fwd_H__
#define  __MotionSynthesis_Fwd_H__


namespace msynth
{


// motion primitive and its components
class CTransformNode;
class CKeyframe;
class CSkeleton;
class CMotionPrimitive;

// motion blenders
class CMotionBlender;
class CSteeringMotionBlender;
class CMotionPrimitiveBlender;
class CMotionPrimitiveBlenderStatistics;

// db and its builder
class CMotionDatabase;
class CMotionDatabaseBuilder;

class CHumanoidMotionTable;

#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CMotionPrimitive> CMotionPrimitiveSharedPtr;


} // namespace msynth


#endif /* __MotionSynthesis_Fwd_H__*/
