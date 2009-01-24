#ifndef __FWD_Stage_H__
#define __FWD_Stage_H__


class CStage;
class CCopyEntity;
class CCopyEntityDesc;
class CLightEntity;
class CAlphaEntity;
class CScriptedCameraEntity;
class CEntityFactory;
class CCopyEntityDescFileData;
class CBaseEntity;
class CBaseEntityFactory;
class CBaseEntityHandle;
class CEntityNode;
class CEntitySet;
class CEntityRenderManager;
class CStaticGeometryBase;
class CStaticGeometryArchiveFG;
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
//class CDynamicLightManagerForStaticGeometry;

class CScriptManager;

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
typedef boost::shared_ptr<CStage> CStageSharedPtr;
typedef boost::weak_ptr<CStage> CStageWeakPtr;
typedef boost::shared_ptr<CBaseEntityFactory> CBaseEntityFactorySharedPtr;


#endif /* __FWD_Stage_H__ */
