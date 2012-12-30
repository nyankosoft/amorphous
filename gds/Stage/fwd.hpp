#ifndef __FWD_Stage_H__
#define __FWD_Stage_H__


#include <boost/weak_ptr.hpp>


namespace amorphous
{

class CStage;
class CCopyEntity;
class CCopyEntityDesc;
class CLightEntity;
class CSoundEntity;
class CAlphaEntity;
class CScriptedCameraEntity;
class CEntityFactory;
class CCopyEntityDescFileData;
class CBaseEntity;
class CBaseEntityFactory;
class CBaseEntityHandle;
class CEntityNode;
class CEntityRenderer;
class CEntitySet;
class CEntityRenderManager;
class CStaticGeometryBase;
class CStaticGeometryArchiveFG;
class CMeshBonesUpdateCallback;
class CBSPTree;
struct STrace;
class CTrace;
class CViewFrustumTest;
class CScreenEffectManager;
class CSurfaceMaterial;
class CSurfaceMaterialManager;
class CTextMessageManager;
struct SGameMessage;
class CBEC_MotionPath;
class EntityMotionPathRequest;
//class CDynamicLightManagerForStaticGeometry;
class CScriptManager;

typedef boost::shared_ptr<CStage> CStageSharedPtr;
typedef boost::weak_ptr<CStage> CStageWeakPtr;
typedef boost::shared_ptr<CBaseEntityFactory> CBaseEntityFactorySharedPtr;

} // namespace amorphous


#endif /* __FWD_Stage_H__ */
