#ifndef  __MotionSynthesis_Fwd_H__
#define  __MotionSynthesis_Fwd_H__


namespace amorphous
{

namespace msynth
{


// motion primitive and its components
class TransformNode;
class Keyframe;
class Bone;
class Skeleton;
class MotionPrimitive;

// motion blenders
class MotionBlender;
class SteeringMotionBlender;
class MotionPrimitiveBlender;
class MotionPrimitiveBlenderStatistics;

// db and its builder
class MotionDatabase;
class MotionDatabaseBuilder;
class MotionDatabaseCompiler;

class HumanoidMotionTable;

//#include <memory>
//typedef std::shared_ptr<MotionPrimitive> MotionPrimitiveSharedPtr;


// later added classes
class BlendNode;
class MotionPrimitiveNode;
class MotionFSM;
class MotionFSMManager;
class MotionFSMInputHandler;

// more classes
class TransformCacheNode;
class TransformCacheTree;


} // namespace msynth

} // namespace amorphous


#endif /* __MotionSynthesis_Fwd_H__*/
