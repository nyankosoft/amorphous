#ifndef __FWD_Graphics_H__
#define __FWD_Graphics_H__


namespace amorphous
{

class GraphicsParameters;
class GraphicsComponent;
class GraphicsComponentCollector;

class AsyncResourceLoader;
class GraphicsResource;
class GraphicsResourceEntry;
class TextureResource;
class MeshResource;
class ShaderResource;
class GraphicsResourceDesc;
class TextureResourceDesc;
class MeshResourceDesc;
class ShaderResourceDesc;
class GraphicsResourceHandle;
class TextureHandle;
class MeshHandle;
class ShaderHandle;
class GraphicsResourceManager;
class GraphicsResourceCacheManager;
class CTextureLoader;
class LockedTexture;
class TextureFillingAlgorithm;
class MeshGenerator;
class BoxMeshGenerator;
class CResourceLoadingStateHolder;
class CShaderGenerator;
class CGenericShaderDesc;

class Light;
class AmbientLight;
class DirectionalLight;
class PointLight;
class Spotlight;
class HemisphericDirectionalLight;
class HemisphericPointLight;
class HemisphericSpotlight;

class C2DRect;
class C2DFrameRect;
class C2DTriangle;
class C2DRegularPolygon;
class FontBase;
class CFont;
class TextureFont;
class TrueTypeTextureFont;
class UTFFont;
class CSimpleBitmapFontData;
class Camera;
class ShaderManager;
class CShaderLightManager;

class TextureRenderTarget;

class SimpleMotionBlur;
class CubeMapManager;
class ShadowMapManager;

class PostProcessEffect;
class PostProcessEffectManager;
class HDRLightingParams;

class CUnitCube;
class CUnitSphere;

class General3DVertex;

class LensFlare;
class CFogParams;

class TEXCOORD2;


class CMMA_VertexSet;
class CMMA_TriangleSet;
class CMMA_Material;
class CMMA_Bone;
class C3DMeshModelArchive;
class CD3DXMeshModel;
class C3DMeshModelBuilder;
class C3DModelLoader;
class CMeshBone;

class General3DMesh;



class CMeshObjectContainer;
class CMeshContainerNode;
class CMeshContainerRenderMethod;
class CMeshContainerNodeRenderMethod;
class CShaderParamsLoader;
class CBlendTransformsLoader;


class BasicMesh;
class ProgressiveMesh;
class SkeletalMesh;
class MeshImpl;

class CustomMesh;


class CGraphicsElement;
class CRectElement;
class CFillRectElement;
class CFrameRectElement;
class CCombinedRectElement;
class CRoundRectElement;
class CRoundFillRectElement;
class CRoundFrameRectElement;
class CCombinedRectElement;
class CTriangleElement;
class CFillTriangleElement;
class CFrameTriangleElement;
class CCombinedTriangleElement;
class CPolygonElement;
class CFillPolygonElement;
class CFramePolygonElement;
class CTextElement;
class CGraphicsElementGroup;
class CPrimitiveElement;
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


// D3D Implementation
class CDirect3D9;
class CD3DXMeshObjectBase;
class CD3DXMeshObject;
class CD3DXPMeshObject; // progressive mesh
class CD3DXSMeshObject; // skeletal mesh


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

} // namespace amorphous


#include <boost/shared_ptr.hpp>

namespace amorphous
{
typedef boost::shared_ptr<FontBase> CFontSharedPtr;
typedef boost::shared_ptr<CGraphicsElementManager> CGraphicsElementManagerSharedPtr;
typedef boost::shared_ptr<CGraphicsElementManagerCallback> CGraphicsElementManagerCallbackSharedPtr;
typedef boost::shared_ptr<CAnimatedGraphicsManagerBase> CAnimatedGraphicsManagerSharedPtr;
} // namespace amorphous


#endif /* __FWD_Graphics_H__ */
