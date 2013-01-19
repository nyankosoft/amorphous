#include "2DPrimitivesTest.hpp"
#include "AsyncLoadingTest.hpp"
#include "CustomMeshTest.hpp"
#include "EnvMapTest.hpp"
#include "GenericShaderTest.hpp"
#include "GLSLTest.hpp"
#include "GraphicsElementEffectTest.hpp"
#include "GraphicsElementsTest.hpp"
#include "HLSLEffectTest.hpp"
#include "LensFlareTest.hpp"
#include "MeshSplitterTest.hpp"
#include "MultibyteFontTest.hpp"
#include "OBBTreeTest.hpp"
#include "PerlinNoiseTextureGeneratorTest.hpp"
#include "PlanarReflectionTest.hpp"
#include "PostProcessEffectTest.hpp"
#include "PrimitiveRendererTest.hpp"
#include "PrimitiveShapeMeshesTest.hpp"
#include "ResolutionChangeTest.hpp"
#include "ShadowMapTest.hpp"
#include "SimpleMotionBlurTest.hpp"
#include "SimpleOverlayEffectsTest.hpp"
#include "TextureFontTest.hpp"
#include "TextureRenderTargetTest.hpp"
#include "gds/Support/ParamLoader.hpp"


CGraphicsTestBase *CreateTestInstance( const std::string& demo_name )
{
	if( demo_name == "" )
		return NULL;
	else if( demo_name == "2DPrimitivesTest" )                return new C2DPrimitivesTest;
	else if( demo_name == "AsyncLoadingTest" )                return new CAsyncLoadingTest;
	else if( demo_name == "CustomMeshTest" )                  return new CCustomMeshTest;
	else if( demo_name == "EnvMapTest" )                      return new CEnvMapTest;
	else if( demo_name == "GenericShaderTest" )               return new CGenericShaderTest;
	else if( demo_name == "GLSLTest" )                        return new CGLSLTest;
	else if( demo_name == "GraphicsElementEffectTest" )       return new CGraphicsElementEffectTest;
	else if( demo_name == "GraphicsElementsTest" )            return new CGraphicsElementsTest;
	else if( demo_name == "HLSLEffectTest" )                  return new CHLSLEffectTest;
	else if( demo_name == "LensFlareTest" )                   return new CLensFlareTest;
	else if( demo_name == "MeshSplitterTest" )                return new CMeshSplitterTest;
	else if( demo_name == "MultibyteFontTest" )               return new CMultibyteFontTest;
	else if( demo_name == "OBBTreeTest" )                     return new COBBTreeTest;
	else if( demo_name == "PerlinNoiseTextureGeneratorTest" ) return new CPerlinNoiseTextureGeneratorTest;
	else if( demo_name == "PlanarReflectionTest" )            return new CPlanarReflectionTest;
	else if( demo_name == "PostProcessEffectTest" )           return new CPostProcessEffectTest;
	else if( demo_name == "PrimitiveRendererTest" )           return new CPrimitiveRendererTest;
	else if( demo_name == "PrimitiveShapeMeshesTest" )        return new CPrimitiveShapeMeshesTest;
	else if( demo_name == "ResolutionChangeTest" )            return new CResolutionChangeTest;
	else if( demo_name == "ShadowMapTest" )                   return new CShadowMapTest;
	else if( demo_name == "SimpleMotionBlurTest" )            return new CSimpleMotionBlurTest;
	else if( demo_name == "SimpleOverlayEffectsTest" )        return new CSimpleOverlayEffectsTest;
	else if( demo_name == "TextureFontTest" )                 return new CTextureFontTest;
	else if( demo_name == "TextureRenderTargetTest" )         return new CTextureRenderTargetTest;
	else
		return NULL;
}


CGraphicsTestBase *CreateTestInstance()
{
	std::string demo_name;
	LoadParamFromFile( "params.txt", "demo", demo_name );
	return CreateTestInstance( demo_name );
}
