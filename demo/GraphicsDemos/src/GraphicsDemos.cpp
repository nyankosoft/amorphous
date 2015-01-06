#include "GraphicsDemoFactory.hpp"
#include "2DPrimitivesDemo.hpp"
#include "AsyncLoadingDemo.hpp"
#include "BrickWallTextureGeneratorDemo.hpp"
#include "CustomMeshDemo.hpp"
#include "CubeMapDemo.hpp"
#include "EnvMapDemo.hpp"
#include "GenericShaderDemo.hpp"
#include "GLSLDemo.hpp"
#include "GraphicsElementEffectDemo.hpp"
#include "GraphicsElementsDemo.hpp"
#include "HLSLEffectDemo.hpp"
#include "LensFlareDemo.hpp"
#include "LightingDemo.hpp"
#include "MeshSplitterDemo.hpp"
#include "MiscShaderDemo.hpp"
#include "MultibyteFontDemo.hpp"
#include "OBBTreeDemo.hpp"
#include "PerlinNoiseTextureGeneratorDemo.hpp"
#include "PlanarReflectionDemo.hpp"
#include "PostProcessEffectDemo.hpp"
#include "PrimitiveRendererDemo.hpp"
#include "PrimitiveShapeMeshesDemo.hpp"
#include "ResolutionChangeDemo.hpp"
#include "ShadowMapDemo.hpp"
#include "SimpleMotionBlurDemo.hpp"
#include "SimpleOverlayEffectsDemo.hpp"
#include "SkeletalMeshDemo.hpp"
#include "TextureFontDemo.hpp"
#include "TextureRenderTargetDemo.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using std::string;


DemoFactory *CreateDemoFactory() { return new GraphicsDemoFactory; }


static const char *sg_demos[] =
{
	"2DPrimitivesDemo",
	"AsyncLoadingDemo",
	"BrickWallTextureGeneratorDemo",
	"CustomMeshDemo",
	"CubeMapDemo",
	"EnvMapDemo",
	"GenericShaderDemo",
	"GLSLDemo",
	"GraphicsElementEffectDemo",
	"GraphicsElementsDemo",
	"HLSLEffectDemo",
	"LensFlareDemo",
	"LightingDemo",
	"MeshSplitterDemo",
	"MiscShaderDemo",
	"MultibyteFontDemo",
	"OBBTreeDemo",
	"PerlinNoiseTextureGeneratorDemo",
	"PlanarReflectionDemo",
	"PostProcessEffectDemo",
	"PrimitiveRendererDemo",
	"PrimitiveShapeMeshesDemo",
	"ResolutionChangeDemo",
	"ShadowMapDemo",
	"SimpleMotionBlurDemo",
	"SimpleOverlayEffectsDemo",
	"SkeletalMeshDemo",
	"TextureFontDemo",
	"TextureRenderTargetDemo"
};


unsigned int GraphicsDemoFactory::GetNumDemos()
{
	return sizeof(sg_demos) / sizeof(sg_demos[0]);
}


const char **GraphicsDemoFactory::GetDemoNames()
{
	return sg_demos;
}


CGraphicsTestBase *GraphicsDemoFactory::CreateDemoInstance( const std::string& demo_name )
{
	if( demo_name == "" )
		return NULL;
	else if( demo_name == "2DPrimitivesDemo" )                return new C2DPrimitivesDemo;
	else if( demo_name == "AsyncLoadingDemo" )                return new AsyncLoadingDemo;
	else if( demo_name == "BrickWallTextureGeneratorDemo" )   return new BrickWallTextureGeneratorDemo;
	else if( demo_name == "CustomMeshDemo" )                  return new CCustomMeshDemo;
	else if( demo_name == "CubeMapDemo" )                     return new CubeMapDemo;
	else if( demo_name == "EnvMapDemo" )                      return new EnvMapDemo;
	else if( demo_name == "GenericShaderDemo" )               return new GenericShaderDemo;
	else if( demo_name == "GLSLDemo" )                        return new GLSLDemo;
	else if( demo_name == "GraphicsElementEffectDemo" )       return new GraphicsElementEffectDemo;
	else if( demo_name == "GraphicsElementsDemo" )            return new GraphicsElementsDemo;
	else if( demo_name == "HLSLEffectDemo" )                  return new HLSLEffectDemo;
	else if( demo_name == "LensFlareDemo" )                   return new LensFlareDemo;
	else if( demo_name == "LightingDemo" )                    return new LightingDemo;
	else if( demo_name == "MeshSplitterDemo" )                return new MeshSplitterDemo;
	else if( demo_name == "MiscShaderDemo" )                  return new MiscShaderDemo;
	else if( demo_name == "MultibyteFontDemo" )               return new MultibyteFontDemo;
	else if( demo_name == "OBBTreeDemo" )                     return new OBBTreeDemo;
	else if( demo_name == "PerlinNoiseTextureGeneratorDemo" ) return new PerlinNoiseTextureGeneratorDemo;
	else if( demo_name == "PlanarReflectionDemo" )            return new PlanarReflectionDemo;
	else if( demo_name == "PostProcessEffectDemo" )           return new PostProcessEffectDemo;
	else if( demo_name == "PrimitiveRendererDemo" )           return new PrimitiveRendererDemo;
	else if( demo_name == "PrimitiveShapeMeshesDemo" )        return new PrimitiveShapeMeshesDemo;
	else if( demo_name == "ResolutionChangeDemo" )            return new ResolutionChangeDemo;
	else if( demo_name == "ShadowMapDemo" )                   return new ShadowMapDemo;
	else if( demo_name == "SimpleMotionBlurDemo" )            return new SimpleMotionBlurDemo;
	else if( demo_name == "SimpleOverlayEffectsDemo" )        return new SimpleOverlayEffectsDemo;
	else if( demo_name == "SkeletalMeshDemo" )                return new SkeletalMeshDemo;
	else if( demo_name == "TextureFontDemo" )                 return new TextureFontDemo;
	else if( demo_name == "TextureRenderTargetDemo" )         return new TextureRenderTargetDemo;
	else
		return NULL;
}


CGraphicsTestBase *GraphicsDemoFactory::CreateDemoInstance( unsigned int index )
{
	return CreateDemoInstance( GetDemoNames()[index] );
}
