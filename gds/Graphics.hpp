// 2D primitive
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/2DPrimitive/2DFrameRect.hpp"
#include "Graphics/2DPrimitive/2DRoundRect.hpp"
#include "Graphics/2DPrimitive/2DRectSet.hpp"
#include "Graphics/2DPrimitive/2DTriangle.hpp"

// font
#include "Graphics/Font/FontBase.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/TrueTypeTextureFont.hpp"
#include "Graphics/Font/BuiltinFonts.hpp"

// mesh
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Mesh/ProgressiveMesh.hpp"
#include "Graphics/Mesh/SkeletalMesh.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Graphics/Mesh/CustomMeshRenderer.hpp"
#include "Graphics/MeshGenerators/MeshGenerators.hpp"

// others
#include "Graphics/3DGameMath.hpp"
#include "Graphics/3DRect.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/FloatRGBAColor.hpp"
#include "Graphics/FloatRGBColor.hpp"

#include "Graphics/General3DVertex.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/HemisphericLight.hpp"
#include "Graphics/IndexedPolygon.hpp"
#include "Graphics/LensFlare.hpp"
#include "Graphics/LightStructs.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Graphics/MeshObjectContainer.hpp"
#include "Graphics/RectTriListIndex.hpp"
#include "Graphics/SimpleMotionBlur.hpp"
#include "Graphics/TextureCoord.hpp"
#include "Graphics/GraphicsResourceManager.hpp"
#include "Graphics/GraphicsResourceCacheManager.hpp"
#include "Graphics/GraphicsResourceHandle.hpp"
#include "Graphics/TextureRenderTarget.hpp"

#include "Graphics/LinePrimitives.hpp"

#include "Graphics/GraphicsElementManager.hpp"
#include "Graphics/GraphicsEffectManager.hpp"


#include "Graphics/Shader/Serialization_ShaderTechniqueHandle.hpp"
#include "Graphics/Shader/ShaderLightManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/ShaderManagerHub.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
