#ifndef  __MotionSynthesis_Fwd_H__
#define  __MotionSynthesis_Fwd_H__


namespace amorphous
{

namespace msynth
{


// motion primitive and its components
class CTransformNode;
class CKeyframe;
class CBone;
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
class CMotionDatabaseCompiler;

class CHumanoidMotionTable;

//#include <boost/shared_ptr.hpp>
//typedef boost::shared_ptr<CMotionPrimitive> CMotionPrimitiveSharedPtr;


// later added classes
class CBlendNode;
class CMotionPrimitiveNode;
class CMotionFSM;
class CMotionFSMManager;
class CMotionFSMInputHandler;

// more classes
class CTransformCacheNode;
class CTransformCacheTree;


} // namespace msynth

} // namespace amorphous


#endif /* __MotionSynthesis_Fwd_H__*/
