#ifndef __FWD_Stage_H__
#define __FWD_Stage_H__


#include <boost/weak_ptr.hpp>


namespace amorphous
{

class CStage;
class CCopyEntity;
class CCopyEntityDesc;
class LightEntity;
class SoundEntity;
class AlphaEntity;
class ScriptedCameraEntity;
class EntityFactory;
class CCopyEntityDescFileData;
class BaseEntity;
class BaseEntityFactory;
class BaseEntityHandle;
class EntityNode;
class EntityRenderer;
class EntityManager;
class EntityRenderManager;
class CStaticGeometryBase;
class CStaticGeometryArchiveFG;
class CMeshBonesUpdateCallback;
class BSPTree;
struct STrace;
class CTrace;
class CViewFrustumTest;
class ScreenEffectManager;
class CSurfaceMaterial;
class CSurfaceMaterialManager;
class CTextMessageManager;
struct GameMessage;
class CBEC_MotionPath;
class EntityMotionPathRequest;
//class CDynamicLightManagerForStaticGeometry;
class ScriptManager;

typedef boost::shared_ptr<CStage> CStageSharedPtr;
typedef boost::weak_ptr<CStage> CStageWeakPtr;
typedef boost::shared_ptr<BaseEntityFactory> BaseEntityFactorySharedPtr;

} // namespace amorphous


#endif /* __FWD_Stage_H__ */
