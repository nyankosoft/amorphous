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
class TextureFilter;
class MeshGenerator;
class BoxMeshGenerator;
class ResourceLoadingStateHolder;
class ShaderGenerator;
class GenericShaderDesc;

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
class TextureFont;
class TrueTypeTextureFont;
class UTFFont;
class SimpleBitmapFontData;
class Camera;
class ShaderManager;
class ShaderLightManager;

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
class FogParams;

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



class MeshObjectContainer;
class CMeshContainerNode;
class MeshContainerRenderMethod;
class CMeshContainerNodeRenderMethod;
class ShaderParamsLoader;
class BlendTransformsLoader;


class BasicMesh;
class ProgressiveMesh;
class SkeletalMesh;
class MeshImpl;

class CustomMesh;


class GraphicsElement;
class RectElement;
class FillRectElement;
class FrameRectElement;
class CombinedRectElement;
class RoundRectElement;
class RoundFillRectElement;
class RoundFrameRectElement;
class CombinedRectElement;
class TriangleElement;
class FillTriangleElement;
class FrameTriangleElement;
class CombinedTriangleElement;
class PolygonElement;
class FillPolygonElement;
class FramePolygonElement;
class TextElement;
class GraphicsElementGroup;
class PrimitiveElement;
class GraphicsElementManager;	
class GraphicsElementManagerCallback;

class GraphicsElementAnimation;
class GraphicsElementLinearAnimation;
class GraphicsElementNonLinearAnimation;
class ColorShiftAnimation;
class AlphaShiftAnimation;
class NonLinearTranslationAnimation;
class TranslationAnimation;
class RotationAnimation;
class ScalingAnimation;
class NonLinearScalingAnimation;
class SizeChangeAnimation;
class GraphicsElementAnimationManagerBase;
class GraphicsElementAnimationManager;


// D3D Implementation
class CDirect3D9;
class CD3DXMeshObjectBase;
class CD3DXMeshObject;
class CD3DXPMeshObject; // progressive mesh
class CD3DXSMeshObject; // skeletal mesh


class MeshType
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
typedef boost::shared_ptr<GraphicsElementManager> GraphicsElementManagerSharedPtr;
typedef boost::shared_ptr<GraphicsElementManagerCallback> GraphicsElementManagerCallbackSharedPtr;
typedef boost::shared_ptr<GraphicsElementAnimationManagerBase> GraphicsElementAnimationManagerSharedPtr;
} // namespace amorphous


#endif /* __FWD_Graphics_H__ */
