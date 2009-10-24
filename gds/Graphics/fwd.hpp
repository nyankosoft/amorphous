#ifndef __FWD_Graphics_H__
#define __FWD_Graphics_H__


class CGraphicsParameters;
class CGraphicsComponent;
class CGraphicsComponentCollector;

// TODO: RenderSystem for cross platform
class CDirect3D9;


class CAsyncResourceLoader;
class CGraphicsResource;
class CGraphicsResourceEntry;
class CTextureResource;
class CMeshResource;
class CShaderResource;
class CGraphicsResourceDesc;
class CTextureResourceDesc;
class CMeshResourceDesc;
class CShaderResourceDesc;
class CGraphicsResourceHandle;
class CTextureHandle;
class CMeshObjectHandle;
class CShaderHandle;
class CGraphicsResourceManager;
class CGraphicsResourceCacheManager;
class CTextureLoader;
class CLockedTexture;
class CTextureFillingAlgorithm;
class CMeshGenerator;

class CLight;
class CAmbientLight;
class CDirectionalLight;
class CPointLight;
class CHemisphericDirectionalLight;
class CHemisphericPointLight;

class C2DRect;
class C2DFrameRect;
class C2DTriangle;
class C2DRegularPolygon;
class CFontBase;
class CFont;
class CTextureFont;
class CTrueTypeTextureFont;
class CCamera;
class CShaderManager;
class CShaderLightManager;
class CPostProcessManager;
class CSimpleMotionBlur;
class CCubeMapManager;
class CShadowMapManager;

class CUnitCube;
class CUnitSphere;

class CGeneral3DVertex;

class CLensFlare;


namespace MeshModel
{
class CMMA_VertexSet;
class CMMA_TriangleSet;
class CMMA_Material;
class CMMA_Bone;
class C3DMeshModelArchive;
class CD3DXMeshModel;
class C3DMeshModelBuilder;
class C3DModelLoader;
}

class CGeneral3DMesh;


// mesh object classes
// - TODO: change class names
//   CD3DXMeshObjectBase -> ???
//   CD3DXMeshObject -> C3DMeshObject
//   CD3DXMeshObject -> C3DProgressiveMeshObject
//   CD3DXMeshObject -> C3DSkeletalMeshObject
class CD3DXMeshObjectBase;
class CD3DXMeshObject;
class CD3DXPMeshObject; // progressive mesh
class CD3DXSMeshObject; // skeletal mesh

class CD3DXMeshVerticesLoader;
class CD3DXMeshIndicesLoader;


class CBasicMesh;
class CProgressiveMesh;
class CSkeletalMesh;
class CMeshImpl;


class CGraphicsElement;
class CGE_Rect;
class CGE_Triangle;
class CGE_Polygon;
class CGE_Text;
class CGE_Group;
class CGE_Primitive;
class CGraphicsElementManager;	
class CGraphicsElementManagerCallback;

class CGraphicsElementEffect;
class CGraphicsElementLinearEffect;
class CGraphicsElementNonLinearEffect;
class CE_ColorShift;
class CE_AlphaChange;
class CE_TranslateCD;
class CE_Translate;
class CE_Rotate;
class CE_Scale;
class CE_ScaleCD;
class CE_SizeChange;
class CAnimatedGraphicsManagerBase;
class CAnimatedGraphicsManager;
class CGraphiceEffectManagerCallback;


class CElementEffectDesc;
class CED_ChangeColor;
class CED_Translate;


#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CFontBase> CFontSharedPtr;
typedef boost::shared_ptr<CGraphicsElementManager> CGraphicsElementManagerSharedPtr;
typedef boost::shared_ptr<CGraphicsElementManagerCallback> CGraphicsElementManagerCallbackSharedPtr;
typedef boost::shared_ptr<CAnimatedGraphicsManagerBase> CAnimatedGraphicsManagerSharedPtr;


class CMeshType
{
public:
	enum Name
	{
		BASIC,
		PROGRESSIVE,
		SKELETAL,
		INVALID,
		NUM_MESH_TYPES
	};
};


class CRegularPolygonStyle
{
public:

	enum Name
	{
		VERTEX_AT_TOP,
		EDGE_AT_TOP,
		NUM_INIT_LAYOUTS
	};
};


#endif /* __FWD_Graphics_H__ */
